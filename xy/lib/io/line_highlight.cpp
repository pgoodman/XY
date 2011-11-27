/*
 * line_highlight.cpp
 *
 *  Created on: Aug 5, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include <limits>
#include <cstdio>
#include <cctype>

#include "xy/include/array.hpp"
#include "xy/include/io/line_highlight.hpp"
#include "xy/include/color.hpp"
#include "xy/include/cstring.hpp"

namespace xy { namespace io { namespace detail {

    static char NULL_STRING[] = {'\0'};

    /// initialize the line highlighting strategy
    line_highlight_strategy::line_highlight_strategy(const char *file_name_, uint32_t line_) throw()
        : ref_count(0)
        , file_name(cstring::copy(file_name_))
        , find_line(line_)
        , found_line(nullptr)
        , i(0)
        , read_size(BLOCK_SIZE)
    { }

    // clear out the info
    line_highlight_strategy::~line_highlight_strategy(void) throw() {
        if(nullptr != found_line && NULL_STRING != found_line) {
            delete [] found_line;
            found_line = NULL_STRING;
        }

        cstring::free(file_name);
        file_name = nullptr;

        ref_count = -1;
    }

    /// read everything up until the line that
    bool line_highlight_strategy::read_up_to_line(
        file<read_tag> &f,
        uint8_t (&scratch)[BLOCK_SIZE]
    ) const throw() {

        uint32_t line_num(1);
        utf8::codepoint chr;
        bool seen_carriage_return(false);

        for(; read_size == BLOCK_SIZE; ) {

            read_size = f.read_block(scratch, array::size(scratch));
            if(0U == read_size) {
                return false;
            }

            i = 0;

            if(line_num >= find_line) {
                return true;
            }

            for(; i < read_size; i += chr.byte_length()) {

                if(seen_carriage_return && '\n' != scratch[i]) {
                    seen_carriage_return = false;
                    line_num += 1;
                    if(line_num >= find_line) {
                        return true;
                    }
                }

                // continue until we read a full codepoint
                if(!decoder.next_state(scratch[i], chr)) {
                    continue;
                }

                if('\n' == chr) {
                    line_num += 1;
                    seen_carriage_return = false;
                    if(line_num >= find_line) {
                        ++i;
                        return true;
                    }
                } if('\r' == chr) {
                    seen_carriage_return = true;
                } else if('\0' == chr) {
                    return false;
                }
            }
        }

        return false;
    }

    void line_highlight_strategy::acquire(line_highlight_strategy *s) throw() {
        ++(s->ref_count);
    }

    void line_highlight_strategy::release(line_highlight_strategy *s) throw() {
        if(0 >= (--(s->ref_count))) {
            delete s;
        }
    }

    highlight_column::highlight_column(const char *file_name_, uint32_t line_, uint32_t col_) throw()
        : line_highlight_strategy(file_name_, line_)
        , find_col(col_)
    { }

    highlight_column::~highlight_column(void) throw() { }

    /// build the underlined string
    /// returns o, the index of next character that we can write to
    /// takes a reference to p, the byte offset of column we want to
    /// highlight in the first line of self.found_line.
    size_t highlight_column::get_line(
        io::file<io::read_tag> &f,
        const highlight_column &self,
        size_t &p
    ) {

        // used for chunks from the file, and then constructing our
        // unhighlighted string
        uint8_t scratch[BLOCK_SIZE] = {'\0'};

        // something went wrong
        if(!self.read_up_to_line(f, scratch)) {
            self.found_line = NULL_STRING;
            return 0;
        }

        char buff[MAX_NUM_BYTES + TAB_SIZE] = {'\0'};
        uint32_t col(1);
        size_t j(0);
        utf8::codepoint chr;
        size_t col_max(std::numeric_limits<size_t>::max());
        size_t k(std::numeric_limits<size_t>::max());

        // go find column find_col at byte offset k in the ring 'buff', and get
        // no more than LINE_NUM_CPS codepoints following the codepoint at
        // column find_col.
        for(size_t num_conts(0); self.read_size > 0; ) {
            for(; self.i < self.read_size && col < col_max; ++(self.i), ++j) {

                buff[j % MAX_NUM_BYTES] = scratch[self.i];
                //printf("buff[%lu] = %c\n", j % MAX_NUM_BYTES, scratch[self.i]);

                if(std::numeric_limits<size_t>::max() == k
                && col == self.find_col) {
                    col_max = col + LINE_NUM_CPS;
                    k = j % MAX_NUM_BYTES;
                    //printf("k = %lu\n", k);
                }

                // continue until we read a full codepoint
                if(!self.decoder.next_state(scratch[self.i], chr)) {
                    ++num_conts;
                    continue;
                }

                // replace the bad codepoint with our error codepoint
                if(self.decoder.found_error()) {
                    j = j - num_conts;
                    const char *bcp(chr.to_cstring());
                    for(; '\0' != *bcp; ++j) {
                        //printf("buff[%lu] = *bcp = %c\n", j % MAX_NUM_BYTES, *bcp);
                        buff[j % MAX_NUM_BYTES] = *bcp++;
                    }
                    j -= 1; // because the loop around will auto increment
                            // anyway
                }

                num_conts = 0;

                if('\n' == chr || '\r' == chr) {
                    self.read_size = 0;
                    buff[j % MAX_NUM_BYTES] = '\0';
                    //printf("buff[%lu] <nl> = 000\n", j % MAX_NUM_BYTES);
                    goto process_line;

                // replace tabs with four spaces
                } else if('\t' == chr) {
                    buff[j % MAX_NUM_BYTES] = ' ';
                    buff[++j % MAX_NUM_BYTES] = ' ';
                    buff[++j % MAX_NUM_BYTES] = ' ';
                    buff[j % MAX_NUM_BYTES] = ' ';
                    col += TAB_SIZE - 1;
                }

                ++col;
            }

            self.i = 0;
            buff[j % MAX_NUM_BYTES] = '\0';

            //printf("buff[%lu] <bl> = 000\n", j % MAX_NUM_BYTES);

            if(self.read_size < BLOCK_SIZE) {
                break;
            }

            self.read_size = f.read_block(scratch, array::size(scratch));
        }

    process_line:

        // something went wrong
        if(std::numeric_limits<size_t>::max() == col_max) {
            self.found_line = NULL_STRING;
            return 0;
        }

        static_assert(MAX_BYTES_PER_MARGIN <= BLOCK_SIZE,
            "BLOCK_SIZE must be bigger to properly handle margins on "
            "either side of the highlighted column."
        );

        static_assert(MAX_BYTES_PER_MARGIN <= MAX_NUM_BYTES,
            "The maximum line margin size (in bytes) must be smaller than "
            "the line buffer length."
        );

        size_t o(0);
        bool seen_non_ws(false);
        size_t extra_on_right(0);

        // copy things up to and including the byte at the column, ignoring
        // any leading whitespace

        // we haven't read over one line (and so haven't wrapped around the ring)
        if(j < MAX_BYTES_PER_MARGIN) {

            for(size_t pp(0); pp <= k && '\0' != buff[pp]; ++pp) {
                if(!seen_non_ws && isspace(buff[pp])) {
                    ++extra_on_right;
                    continue;
                }
                //printf("scratch[p = %lu] = buff[pp = %lu] = %c\n", p, pp, buff[pp]);
                seen_non_ws = true;
                scratch[p] = buff[pp];
                ++p;
            }
            o = p;

        // we have read over one line and so might need to handle wrap around.
        } else {
            size_t n((MAX_NUM_BYTES + k - MAX_BYTES_PER_MARGIN) % MAX_NUM_BYTES);
            size_t m(n);
            for(; m != k && '\0' != buff[m]; m = (++n) % MAX_NUM_BYTES) {
                if(!seen_non_ws && isspace(buff[m])) {
                    ++extra_on_right;
                    continue;
                }
                //printf("scratch[o++ = %lu] = buff[m = %lu] = %c\n", o, m, buff[m]);
                seen_non_ws = true;
                scratch[o++] = buff[m];
            }
            p = o;

            //printf("scratch[o++ = %lu] = buff[k = %lu] = %c\n", o, k, buff[k]);
            scratch[o++] = buff[k];
        }

        // copy things after the byte at the column
        for(size_t m(0); m < (extra_on_right + MAX_BYTES_PER_MARGIN); ++m) {
            scratch[o] = buff[((k + 1) + m) % MAX_BYTES_PER_MARGIN];
            //printf("scratch[o = %lu] = buff[... = %lu] = %c\n", o, ((k + 1) + m) % MAX_BYTES_PER_MARGIN, buff[((k + 1) + m) % MAX_BYTES_PER_MARGIN]);
            if('\0' == scratch[o++]) {
                break;
            }
            scratch[o] = '\0';
        }

        // shouldn't be possible
        if(0 == o) {
            self.found_line = NULL_STRING;
            return 0;
        }

        // chop off leading extended bytes
        size_t n(0);
        for(; ; ++n) {
            if(0x80 != (0xC0 & scratch[n])) {
                break;
            }
            scratch[n] = '\0';
            //printf("scratch[%lu] <lead> = 000\n", n);
        }

        // chop off trailing extended bytes if a unicode character cannot be
        // formed
        for(; 0x80 == (0xC0 & scratch[--o]);) {
            // twidde :-P
        }

        self.decoder.reset();

        for(; '\0' != scratch[o]; ) {
            self.decoder.next_state(scratch[o++], chr);
        }

        if(self.decoder.is_in_use()) {
            for(; 0x80 == (0xC0 & scratch[--o]);) {
                scratch[o] = '\0';
                //printf("scratch[%lu] <tail> = 000\n", o);
            }
            scratch[o] = '\0';
            //printf("scratch[%lu] <end-tail> = 000\n", o);
        }

        // we've now got the line right :D
        const size_t byte_len(cstring::byte_length(&(scratch[n])));
        size_t line_len((byte_len * 2) + 20);

        //printf("line n=%lu p=%lu byte_len=%lu line_len=%lu   '%s'\n", n, p, byte_len, line_len, &(scratch[n]));
        self.found_line = new char[line_len];
        memset(self.found_line, 0, line_len);

        self.found_line[0] = self.found_line[1] = ' ';
        self.found_line[2] = self.found_line[3] = ' ';

        memcpy(&(self.found_line[4]), &(scratch[n]), byte_len);
        o = 4 + byte_len;
        self.found_line[o++] = '\n';

        p = (p - n - 1) + 4;

        return o;
    }

    /// build the underlined string
    void highlight_column::point_to_column(io::file<io::read_tag> &f, const highlight_column &self) {
        size_t o(0), p(0);
        o = get_line(f, self, p);

        if(NULL_STRING == self.found_line) {
            return;
        }

        for(size_t i(0); i < p; ++i) {
            self.found_line[o++] = ' ';
        }

        size_t style_len(cstring::byte_length(XY_F_GREEN));
        memcpy(&(self.found_line[o]), XY_F_GREEN, style_len);
        o += style_len;
        self.found_line[o++] = '^';

        style_len = cstring::byte_length(XY_F_DEF);
        memcpy(&(self.found_line[o]), XY_F_DEF, style_len);
        o += style_len;
        self.found_line[o++] = '\n';
        self.found_line[o++] = '\0';
    }

    /// get the pointed out column
    const char *highlight_column::get_highlighted_line(void) const throw() {
        if(nullptr != this->found_line) {
            return this->found_line;
        }

        if(!io::read::open_file(this->file_name, highlight_column::point_to_column, *this)) {
            this->found_line = NULL_STRING;
        }

        return this->found_line;
    }

    underline::underline(const char *file_name_, uint32_t line_, uint32_t col_start_, uint32_t col_end_, char first_, char last_) throw()
        : highlight_column(file_name_, line_, col_end_)
        , find_col_start(col_start_)
        , first(first_)
        , last(last_)
    { }

    underline::~underline(void) throw() { }

    /// build the underlined string
    void underline::stylize(io::file<io::read_tag> &f, const underline &self) {
        size_t o(0), p(0);
        o = get_line(f, self, p);

        if(NULL_STRING == self.found_line) {
            return;
        }

        // figure out how many columns there are up to the column we want to
        // point out
        char old_val = self.found_line[p];
        self.found_line[p] = '\0';

        size_t num_cols(utf8::decoder::length(self.found_line));
        self.found_line[p] = old_val;

        // when we can start putting the green in
        num_cols -= (self.find_col - self.find_col_start);

        size_t i(0);
        for(; i < num_cols; ++i) {
            self.found_line[o++] = ' ';
        }

        size_t style_len(cstring::byte_length(XY_F_GREEN));
        memcpy(&(self.found_line[o]), XY_F_GREEN, style_len);
        o += style_len;

        // underline to the left
        self.found_line[o++] = self.first;
        for(++i; i < p; ++i) {
            self.found_line[o++] = '~';
        }

        self.found_line[o++] = self.last;

        style_len = cstring::byte_length(XY_F_DEF);
        memcpy(&(self.found_line[o]), XY_F_DEF, style_len);
        o += style_len;
        self.found_line[o++] = '\n';
        self.found_line[o++] = '\0';
    }

    /// get the underlined line plus a right pointer on the column
    const char *underline::get_highlighted_line(void) const throw() {
        if(nullptr != this->found_line) {
            return this->found_line;
        }

        if(!io::read::open_file(this->file_name, underline::stylize, *this)) {
            this->found_line = NULL_STRING;
        }

        return this->found_line;
    }

}}}

namespace xy { namespace io {

    /// allow for highlighting of a column with a ^
    line_highlight highlight_column(
        const char *file_name,
        const uint32_t line,
        const uint32_t column
    ) throw() {
        return line_highlight(new detail::highlight_column(file_name, line, column));
    }

    /// highlighting of something, e.g. ~~~~^
    line_highlight highlight_left(
        const char *file_name,
        uint32_t line,
        uint32_t from_col,
        uint32_t to_col
    ) throw() {
        if(from_col > to_col) {
            from_col = to_col;
        } else if(detail::highlight_column::MARGIN < (to_col - from_col)) {
            from_col = to_col;
        }
        return line_highlight(new detail::underline(file_name, line, from_col, to_col, '~', '^'));
    }

    /// highlighting of something, e.g. ^~~~~
    line_highlight highlight_right(
        const char *file_name,
        uint32_t line,
        uint32_t from_col,
        uint32_t to_col
    ) throw() {
        if(from_col > to_col) {
            from_col = to_col;
        } else if(detail::highlight_column::MARGIN < (to_col - from_col)) {
            from_col = to_col;
        }
        return line_highlight(new detail::underline(file_name, line, from_col, to_col, '^', '~'));
    }

    /// underlining of something, e.g. ~~~~~
    line_highlight highlight_line(
        const char *file_name,
        uint32_t line,
        uint32_t from_col,
        uint32_t to_col
    ) throw() {
        if(from_col > to_col) {
            from_col = to_col;
        } else if(detail::highlight_column::MARGIN < (to_col - from_col)) {
            from_col = to_col;
        }

        const char right((from_col + 1) >= to_col ? ' ' : '~');
        const char left(' ' == right ? '^' : '~');

        return line_highlight(new detail::underline(file_name, line, from_col, to_col, left, right));
    }

    /// single line highlighter

    line_highlight::line_highlight(detail::line_highlight_strategy *h) throw()
        : highlighter(h)
    {
        detail::line_highlight_strategy::acquire(highlighter);
    }

    line_highlight::line_highlight(const line_highlight &that) throw()
        : highlighter(that.highlighter)
    {
        detail::line_highlight_strategy::acquire(highlighter);
    }

    line_highlight::~line_highlight(void) throw() {
        detail::line_highlight_strategy::release(highlighter);
        highlighter = nullptr;
    }

    line_highlight &line_highlight::operator=(const line_highlight &that) throw() {
        if(highlighter != that.highlighter) {
            detail::line_highlight_strategy::release(highlighter);
            highlighter = that.highlighter;
            detail::line_highlight_strategy::acquire(highlighter);
        }
        return *this;
    }

    line_highlight::operator const char *(void) const throw() {
        return highlighter->get_highlighted_line();
    }
}}
