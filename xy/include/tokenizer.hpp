/*
 * tokenizer.hpp
 *
 *  Created on: Jul 4, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef XY_TOKENIZER_HPP_
#define XY_TOKENIZER_HPP_

#include "xy/include/token.hpp"
#include "xy/include/lexer.hpp"
#include "xy/include/utf8/codepoint.hpp"

namespace xy {

    class tokenizer {
    private:

        lexer ll;
        int chr;
        utf8::codepoint cp;

        enum {
            BUFFER_LENGTH = 4096U,
            NAME_LENGTH = 32U
        };

        char scratch[BUFFER_LENGTH + 20];

        enum {
            READ_NEXT_CODEPOINT,
            HAVE_NEXT_CODEPOINT,
            HAVE_ASCII_CODEPOINT,
            HAVE_NON_ASCII_CODEPOINT,
            DONE,
        } state;

    private:

        void push_line_file_col(diagnostic_context &ctx) throw();
        bool get_octal_digit(diagnostic_context &ctx, size_t i) throw();
        bool get_hex_digit(diagnostic_context &ctx, size_t i) throw();

    public:

        tokenizer(void) throw();

        bool get_token(io::file<io::read_tag> &f, diagnostic_context &ctx, token &tok) throw();

        const char *get_value(void) const throw();
    };
}

#endif /* XY_TOKENIZER_HPP_ */
