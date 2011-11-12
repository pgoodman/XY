/*
 * line_highlight.hpp
 *
 *  Created on: Aug 5, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_LINE_HIGHLIGHT_HPP_
#define XY_LINE_HIGHLIGHT_HPP_

#include <cassert>
#include <cstring>
#include <stdint.h>

#include "xy/include/io/file.hpp"

#include "xy/include/utf8/codepoint.hpp"
#include "xy/include/utf8/decoder.hpp"

namespace xy { namespace io {

    // forward declaration
    class line_highlight;

    namespace detail {

        /// the actual highlighting class. contains the basic info needed to
        /// find some particular line. a line
        class line_highlight_strategy {
        private:

            int ref_count;

        protected:

            enum {
                BLOCK_SIZE = 4096U,
                LINE_NUM_CPS = 80U,
            };

            mutable char *file_name;
            mutable uint32_t find_line;
            mutable char *found_line;
            mutable utf8::decoder decoder;
            mutable size_t i;
            mutable size_t read_size;

            /// read everything up to the start of a line.
            bool read_up_to_line(
                file<read_tag> &f,
                uint8_t (&scratch)[BLOCK_SIZE]
            ) const throw();

        public:

            line_highlight_strategy(const char *file_name_, uint32_t line_) throw();
            virtual ~line_highlight_strategy(void) throw();

            static void acquire(line_highlight_strategy *) throw();
            static void release(line_highlight_strategy *) throw();

            virtual const char *get_highlighted_line(void) const throw() = 0;
        };

        /// underline a single token
        class highlight_column : public line_highlight_strategy {
        protected:

            uint32_t find_col;

            static size_t get_line(io::file<io::read_tag> &f, const highlight_column &self, size_t &p);
            static void point_to_column(io::file<io::read_tag> &f, const highlight_column &self);

        public:

            enum {
                MAX_LINE_NUM_BYTES = LINE_NUM_CPS * 4U,
                MAX_NUM_BYTES = LINE_NUM_CPS * 4U + 1U,
                MARGIN = (LINE_NUM_CPS / 2) - 2,
                MAX_BYTES_PER_MARGIN = MARGIN * 4,
                TAB_SIZE = 4
            };

            highlight_column(const char *file_name_, uint32_t line_, uint32_t col_) throw();
            virtual ~highlight_column(void) throw();

            virtual const char *get_highlighted_line(void) const throw();
        };

        /// underline a single token
        class underline : public highlight_column {
        private:

            uint32_t find_col_start;
            char first;
            char last;

            static void stylize(io::file<io::read_tag> &f, const underline &self);

        public:
            underline(const char *file_name_, uint32_t line_, uint32_t col_start_, uint32_t col_end_, char first_, char last_) throw();
            virtual ~underline(void) throw();

            virtual const char *get_highlighted_line(void) const throw();
        };
    }

    /// return a line highlighter whose strategy highlights a single token in
    /// the line.
    line_highlight highlight_column(
        const char *file_name,
        const uint32_t line,
        const uint32_t column
    ) throw();

    /// return a line highlighter whose strategy highlights a single token in
    /// the line.
    line_highlight highlight_left(
        const char *file_name,
        uint32_t line,
        uint32_t from_col,
        uint32_t to_col
    ) throw();

    /// return a line highlighter whose strategy highlights a single token in
    /// the line.
    line_highlight highlight_right(
        const char *file_name,
        uint32_t line,
        uint32_t from_col,
        uint32_t to_col
    ) throw();

    /// return a line highlighter whose strategy highlights a single token in
    /// the line.
    line_highlight highlight_line(
        const char *file_name,
        uint32_t line,
        uint32_t from_col,
        uint32_t to_col
    ) throw();

    /// lazily evaluated line highlighter, i.e. it will go get some buffer
    /// (possibly with more than one line) where each line of the buffer has
    /// no more than MAX_LINE_HIGHLIGHT_NUM_BYTES bytes.
    class line_highlight {
    private:

        mutable detail::line_highlight_strategy *highlighter;

        line_highlight(detail::line_highlight_strategy *) throw();

        friend line_highlight highlight_column(
            const char *file_name,
            const uint32_t line,
            const uint32_t column
        ) throw();

        friend line_highlight highlight_left(
            const char *file_name,
            const uint32_t line,
            const uint32_t from_col,
            const uint32_t to_col
        ) throw();

        friend line_highlight highlight_right(
            const char *file_name,
            const uint32_t line,
            const uint32_t from_col,
            const uint32_t to_col
        ) throw();

        friend line_highlight highlight_line(
            const char *file_name,
            const uint32_t line,
            const uint32_t from_col,
            const uint32_t to_col
        ) throw();

    public:

        line_highlight(const line_highlight &) throw();
        ~line_highlight(void) throw();

        line_highlight &operator=(const line_highlight &) throw();

        /// conversion to string
        operator const char *(void) const throw();
    };
}}

#endif /* XY_LINE_HIGHLIGHT_HPP_ */
