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

    /// the set of tokens recognized by the tokenizer
    typedef enum : uint8_t {

        /*T_BEGIN,                    // begin
        T_END,                      // end
        //T_NOT,                      // not
        T_INTO,                     // into
        T_GIVE,                     // give
        T_TAKE,                     // take
        T_OWN,                      // own
        T_SHARE,                    // share
        T_SHARED,                   // shared
        T_LET,                      // let
        T_IF,                       // if
        T_THEN,                     // then
        T_ELSE,                     // else*/
        T_LET,                      // let
        T_IMPORT,                   // import
        T_RETURN,                   // return
        T_YIELD,                    // yield
        T_UNION,
        T_RECORD,
        T_FUNCTION,
        T_TYPE_TYPE,
        T_TYPE_UNIT,

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
        T_ARROW,                    // ->
        T_COLON,                    // :
        T_SEMICOLON,                // ;
        T_AT,                       // @

        T_LESS_THAN,                // <
        T_GREATER_THAN,             // >
        T_EQUAL,                    // =
        T_DOUBLE_ARROW,             // =>
        T_NOT_EQUAL,                // /=   .
        T_ASSIGN,                   // :=
        T_DECLARE,                  // ::

        //T_NEW_LINE,                 // \n   .

        T_NAME,
        T_TYPE_NAME,
        T_STRING_LITERAL,
        T_INTEGER_LITERAL,
        T_RATIONAL_LITERAL,

        T_EOF,
        T_INVALID
    } token_type;



    // forward declaration
    class tokenizer;
    class token_stream;

    /// represents a single lexical unit (e.g. a word). This stores the kind of
    /// the word and the position of the word inside its file.
    class token {
    private:
        friend class tokenizer;
        friend class token_stream;

        token_type type_;
        uint16_t num_columns_;
        uint32_t col_:24;
        uint32_t line_;

    public:

        /// the maximum byte length of any given token
        enum : unsigned {
            MAX_LENGTH = 4096U
        };

        token(void) throw();

        token(const token &o) throw();

        token &operator=(const token &o) throw();

        token &operator=(const token &&o) throw();

        token_type type(void) const throw();

        uint32_t line(void) const throw();

        uint32_t column(void) const throw();

        uint32_t end_column(void) const throw();

        uint32_t num_columns(void) const throw();

        const char *name(void) const throw();

        static const char *name(token_type) throw();

        void extend(const token &) throw();
    };

}

#endif /* XY_TOKEN_HPP_ */
