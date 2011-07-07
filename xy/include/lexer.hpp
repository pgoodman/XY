/*
 * lexer.hpp
 *
 *  Created on: Jul 1, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef XY_LEXER_HPP_
#define XY_LEXER_HPP_

#include <stdint.h>

#include "xy/include/io/file.hpp"
#include "xy/include/diagnostic_context.hpp"
#include "xy/include/utf8/codepoint.hpp"
#include "xy/include/utf8/decoder.hpp"

namespace xy {

    /// read in utf8 codepoints from a file.
    /// todo: in future, this should be decoupled from files and be based
    ///       on a byte reader that does the blocking.
    class lexer {
    private:

        enum {
            BLOCK_SIZE = 4096
        };

        utf8::codepoint chr;
        utf8::decoder decoder;

        uint8_t scratch[BLOCK_SIZE];
        uint32_t line_tracker[2];
        uint32_t column_tracker[2];
        unsigned curr;
        unsigned next;
        size_t read_size;
        size_t i;
        bool seen_carriage_return;

        enum {
            READ_BLOCK,
            DONE_READING,
            READ_NEXT_CODEPOINT,
        } state;

    public:

        lexer(void) throw();

        bool get_codepoint(
            io::file<io::read_tag> &f,
            diagnostic_context &ctx,
            utf8::codepoint &cp
        ) throw();

        uint32_t line(void) const throw();
        uint32_t column(void) const throw();
    };

}

#endif /* XY_LEXER_HPP_ */
