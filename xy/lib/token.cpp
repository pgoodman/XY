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
        , num_columns_(1)
        , col_(0)
        , line_(0)
    { }

    token::token(const token &o) throw()
        : type_(o.type_)
        , num_columns_(o.num_columns_)
        , col_(o.col_)
        , line_(o.line_)
    { }

    token &token::operator=(const token &o) throw() {
        type_ = o.type_;
        num_columns_ = o.num_columns_;
        line_ = o.line_;
        col_ = o.col_;
        return *this;
    }

    token &token::operator=(const token &&o) throw() {
        type_ = o.type_;
        num_columns_ = o.num_columns_;
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

    uint32_t token::end_column(void) const throw() {
        return col_ + num_columns_ - 1;
    }

    uint32_t token::num_columns(void) const throw() {
        return num_columns_;
    }

    const char *token::name(token_type type) throw() {
        static const char * const names[]{
            /*"T_BEGIN",                    // begin
            "T_END",                      // end
            //"T_NOT",                      // not
            "T_INTO",                     // into
            "T_GIVE",                     // give
            "T_TAKE",                     // take
            "T_OWN",                      // own
            "T_SHARE",                    // share
            "T_SHARED",                   // shared
            "T_LET",                      // let
            "T_IF",                       // if
            "T_THEN",                     // then
            "T_ELSE",                     // else*/

            "'let' symbol",                      // let
            "'import' symbol",                   // import
            "'return' symbol",                   // return
            "'yield' symbol",                    // yield
            "'union' symbol",
            "'record' symbol",
            "'function' symbol",

            "left parenthesis",               // (
            "right parenthesis",              // )

            "left bracket",             // [
            "right bracket",            // ]

            "left brace",               // {
            "right brace",              // }

            "plus symbol",                     // +
            "minus symbol",                    // -
            "asterisk",                 // *
            "forward slash",            // /
            "backward slash",           // \\ '
            "comma",                    // ,
            "period",                   // .
            "ampersand",                // &
            "pipe",                     // |
            "hat",                      // ^
            "percent symbol",                  // %
            "tilde",                    // ~
            "arrow",                    // ->
            "semicolon",                // ;
            //"colon",                    // :
            "at symbol",                       // @

            "less-than symbol",                // <
            "greater-than symbol",             // >
            "equal symbol",                    // =
            "double-arrow",
            "not-equal symbol",                // /=
            "assignment symbol",                   // :=
            "declaration symbol",       // ::

            //"new line",                 // \n

            "variable/function name",
            "type name",
            "string literal",
            "integer literal",
            "rational number literal",

            "end of file",
            "invalid symbol",
            "???1",
            "???2",
            "???3",
            "???4",
            "???5",
            "???6",
        };
        return names[type];
    }

    const char *token::name(void) const throw() {
        return token::name(type_);
    }
}
