/*
 * token.hpp
 *
 *  Created on: Jul 4, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef XY_TOKEN_HPP_
#define XY_TOKEN_HPP_

#include <stdint.h>

namespace xy {

    typedef enum : uint8_t {

        T_BEGIN,                    // begin
        T_END,                      // end
        T_NOT,                      // not
        T_INTO,                     // into
        T_GIVE,                     // give
        T_TAKE,                     // take
        T_OWN,                      // own
        T_LET,                      // let
        T_IF,                       // if
        T_THEN,                     // then
        T_ELSE,                     // else

        T_OPEN_PAREN,               // (
        T_CLOSE_PAREN,              // )

        T_OPEN_BRACKET,             // [
        T_CLOSE_BRACKET,            // ]

        T_OPEN_BRACE,               // {
        T_CLOSE_BRACE,              // }

        T_PLUS,                     // +
        T_MINUS,                    // -
        T_ASTERISK,                 // *
        T_FORWARD_SLASH,            // /
        T_BACKWARD_SLASH,           // \   .
        T_COMMA,                    // ,
        T_PERIOD,                   // .
        T_AMPERSAND,                // &
        T_PIPE,                     // |
        T_HAT,                      // ^
        T_PERCENT,                  // %
        T_TILDE,                    // ~
        //T_COLON,                    // :
        T_SEMICOLON,                // ;

        T_LESS_THAN,                // <
        T_GREATER_THAN,             // >
        T_EQUAL,                    // =
        T_NOT_EQUAL,                // /=   .

        T_NEW_LINE,                 // \n   .

        T_NAME,
        T_TYPE_NAME,
        T_STRING_LITERAL,
        T_INTEGER_LITERAL,
        T_RATIONAL_LITERAL,

        T_INVALID
    } token_type;

    // forward declaration
    class tokenizer;

    class token {
    private:
        friend class tokenizer;

        token_type type_;
        uint32_t line_;
        uint32_t col_;

    public:

        token(void) throw();

        token(const token &o) throw();

        token &operator=(const token &o) throw();

        token &operator=(const token &&o) throw();

        token_type type(void) const throw();

        uint32_t line(void) const throw();

        uint32_t column(void) const throw();

        const char *name(void) const throw();
    };

}

#endif /* XY_TOKEN_HPP_ */
