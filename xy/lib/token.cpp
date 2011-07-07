/*
 * token.cpp
 *
 *  Created on: Jul 4, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#include "xy/include/token.hpp"

namespace xy {

    token::token(void) throw()
        : type_(T_INVALID)
        , line_(0)
        , col_(0)
    { }

    token::token(const token &o) throw()
        : type_(o.type_)
        , line_(o.line_)
        , col_(o.col_)
    { }

    token &token::operator=(const token &o) throw() {
        type_ = o.type_;
        line_ = o.line_;
        col_ = o.col_;
        return *this;
    }

    token &token::operator=(const token &&o) throw() {
        type_ = o.type_;
        line_ = o.line_;
        col_ = o.col_;
        return *this;
    }

    token_type token::type(void) const throw() {
        return type_;
    }

    uint32_t token::line(void) const throw() {
        return line_;
    }

    uint32_t token::column(void) const throw() {
        return col_;
    }

    const char *token::name(void) const throw() {
        static const char * const names[]{
            "T_BEGIN",                    // begin
            "T_END",                      // end
            "T_NOT",                      // not
            "T_IS",                       // is
            "T_GIVE",                     // give
            "T_TAKE",                     // take
            "T_OWN",                      // own
            "T_LET",                      // let
            "T_IF",                       // if
            "T_THEN",                     // then
            "T_ELSE",                     // else

            "T_OPEN_PAREN",               // (
            "T_CLOSE_PAREN",              // )

            "T_OPEN_BRACKET",             // [
            "T_CLOSE_BRACKET",            // ]

            "T_OPEN_BRACE",               // {
            "T_CLOSE_BRACE",              // }

            "T_PLUS",                     // +
            "T_MINUS",                    // -
            "T_ASTERISK",                 // *
            "T_FORWARD_SLASH",            // /
            "T_BACKWARD_SLASH",           // \\ '
            "T_COMMA",                    // ,
            "T_PERIOD",                   // .
            "T_AMPERSAND",                // &
            "T_PIPE",                     // |
            "T_HAT",                      // ^
            "T_PERCENT",                  // %
            "T_TILDE",                    // ~
            "T_COLON",                    // :
            "T_SEMICOLON",                // ;

            "T_LESS_THAN",                // <
            "T_GREATER_THAN",             // >
            "T_EQUAL",                    // =
            "T_NOT_EQUAL",                // /=

            "T_NEW_LINE",                 // \n

            "T_NAME",
            "T_TYPE_NAME",
            "T_STRING_LITERAL",
            "T_INTEGER_LITERAL",
            "T_RATIONAL_LITERAL",

            "T_INVALID"
        };
        return names[type_];
    }
}
