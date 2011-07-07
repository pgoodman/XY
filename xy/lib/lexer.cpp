/*
 * lexer.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#include <cstring>
#include <cctype>

#include "xy/include/lexer.hpp"
#include "xy/include/array.hpp"

namespace xy {

    lexer::lexer(void) throw()
        : chr()
        , decoder()
    {
        line_tracker[0] = line_tracker[1] = 1;
        column_tracker[0] = column_tracker[1] = 1;
        curr = 0;
        next = 1;
        read_size = BLOCK_SIZE;
        state = READ_BLOCK;
        seen_carriage_return = false;
        memset(scratch, 0, array::size(scratch));
    }

    bool lexer::get_codepoint(
        io::file<io::read_tag> &f,
        diagnostic_context &ctx,
        utf8::codepoint &cp
    )  throw() {
        int first_chr{'\0'};
        switch(state) {
        case READ_BLOCK:

            for(; read_size == BLOCK_SIZE; ) {

                read_size = f.read_block<BLOCK_SIZE>(scratch);
                if(0U == read_size) {
                    state = DONE_READING;
                    goto check_trailing_mb;
                }

                for(i = 0; i < read_size; ++i) {

                    // continue until we read a full codepoint
                    if(!decoder.next_state(scratch[i], chr)) {
                        continue;
                    }

                    if(decoder.found_error()) {
                        ctx.diag.push(io::w_invalid_utf8_cp, chr.to_cstring());
                        ctx.diag.push(
                            io::c_file_line_col, ctx.top_file(),
                            line_tracker[curr], column_tracker[curr]
                        );
                    }

                    // we've read a codepoint
                    line_tracker[next] = line_tracker[curr];
                    column_tracker[next] = column_tracker[curr] + 1;

                    if(chr.is_ascii()) {
                        first_chr = chr.to_cstring()[0];
                        if('\n' == first_chr) {
                            line_tracker[next] += 1;
                            column_tracker[next] = 1;
                        } else {

                            // if we had a \r without having \r\n
                            if(seen_carriage_return) {
                                seen_carriage_return = false;
                                line_tracker[curr] = line_tracker[curr] + 1;
                                line_tracker[next] = line_tracker[curr];
                                column_tracker[curr] = 1;
                                column_tracker[next] = 2;
                            }

                            if('\t' == first_chr) {
                                column_tracker[next] += 3;
                            } else if('\r' == first_chr) {
                                seen_carriage_return = true;
                            } else if('\0' != first_chr) {
                                if(!isprint(first_chr) && !isspace(first_chr)) {
                                    ctx.diag.push(io::e_non_graph_char, first_chr);
                                    ctx.diag.push(
                                        io::c_file_line_col, ctx.top_file(),
                                        line_tracker[curr], column_tracker[curr]
                                    );
                                    state = READ_NEXT_CODEPOINT;
                                    goto read_next_codepoint;
                                }
                            } else {
                                state = DONE_READING;
                                return false;
                            }
                        }
                    }

                    state = READ_NEXT_CODEPOINT;
                    cp = chr;
                    return true;

        case READ_NEXT_CODEPOINT:
        read_next_codepoint:
                    curr = next;
                    next = 1 - curr;
                }
            }
            break;

        case DONE_READING:
            return false;
        }

    check_trailing_mb:

        // todo: this is an error condition
        if(decoder.is_in_use()) {
            ctx.diag.push(io::e_invalid_trailing_utf8_cp);
            ctx.diag.push(
                io::c_file_line_col, ctx.top_file(),
                line_tracker[curr], column_tracker[curr]
            );
        }

        return false;
    }

    uint32_t lexer::line(void) const throw() {
        return line_tracker[curr];
    }

    uint32_t lexer::lexer::column(void) const throw() {
        return column_tracker[curr];
    }
}
