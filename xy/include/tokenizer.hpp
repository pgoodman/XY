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
#include "xy/include/diagnostic_context.hpp"
#include "xy/include/utf8/codepoint.hpp"

namespace xy {

    class tokenizer {
    private:

        lexer ll;
        int chr;
        utf8::codepoint cp;

        static_assert(20U < MAX_TOKEN_LENGTH,
            "The maximum length of a token must be larger than 20 bytes. This "
            "requirement is to allow a bit of overflow to occur."
        );

        enum {
            NAME_LENGTH = 32U,
            BUFFER_LENGTH = MAX_TOKEN_LENGTH - 20U
        };

        static_assert(NAME_LENGTH <= MAX_TOKEN_LENGTH,
            "The maximum length of a name (type name, identifier) must be less "
            "than or equal to the maximum length of a token (usually the length "
            "of a string literal)."
        );

        enum {
            READ_NEXT_CODEPOINT,
            HAVE_NEXT_CODEPOINT,
            HAVE_ASCII_CODEPOINT,
            HAVE_NON_ASCII_CODEPOINT,
            DONE,
        } state;

    private:

        void push_file_line_col(diagnostic_context &ctx) throw();
        void push_file_line_col_point(diagnostic_context &ctx) throw();
        void push_file_line_col_under(diagnostic_context &ctx, uint32_t start_col) throw();
        void push_file_line_col_left(diagnostic_context &ctx, uint32_t start_col) throw();
        bool get_octal_digit(diagnostic_context &ctx, char *, uint32_t start_col) throw();
        bool get_hex_digit(diagnostic_context &ctx, char *, uint32_t start_col) throw();

    public:

        tokenizer(void) throw();

        /// get the next token
        bool get_token(
            io::file<io::read_tag> &f,
            diagnostic_context &ctx,
            token &tok,
            char (&scratch)[MAX_TOKEN_LENGTH]
        ) throw();
    };
}

#endif /* XY_TOKENIZER_HPP_ */
