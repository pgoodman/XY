/*
 * parser.cpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include <cassert>

#include "xy/include/parser.hpp"
#include "xy/include/tokenizer.hpp"
#include "xy/include/array.hpp"

#include "xy/include/io/file.hpp"
#include "xy/include/io/line_highlight.hpp"

namespace xy {

    /// expect a new line after somehting
    static bool accept_expected_newline(diagnostic_context &ctx, token_stream &stream) throw() {
        if(stream.check(T_NEW_LINE)) {
            stream.accept();
            return true;
        }

        // re-get the last seen token to calculate the number columns its
        // representation requires
        token prev_tok;
        stream.undo();
        stream.accept(prev_tok);
        const size_t col(prev_tok.column() + prev_tok.num_columns());

        // diagnostic pointing to immediately after this token
        ctx.diag.push(io::e_required_new_line);
        ctx.diag.push(io::c_file_line_col, ctx.file(), prev_tok.line(), col);
        ctx.diag.push(io::c_highlight,
            io::highlight_column(ctx.file(), prev_tok.line(), col)
        );

        return false;
    }

    /// error that we got an unexpected token after some first token that we
    /// would normally use as a prefix
    static bool unexpected_follow_symbol(
        diagnostic_context &ctx, token &first, token &got, token_type expected
    ) throw() {

        ctx.diag.push(io::e_unexpected_follow_symbol,
            got.name(), token::name(expected), first.name()
        );
        ctx.diag.push(io::c_file_line_col, ctx.file(), got.line(), got.column());
        ctx.diag.push(io::c_highlight, io::highlight_line(
            ctx.file(), got.line(), got.column(), got.column() + got.num_columns() - 1
        ));
        return false;
    }

    /// parse a variable definition
    //          let file:name := import "file/name.xy"
    //          let file:name:foo := (import "file/name.xy").foo
    //          let one := 1
    //          let two : Int = 2
    bool parse_let(diagnostic_context &ctx, token_stream &stream) throw() {
        assert(stream.check(T_LET));
        token prev;
        token curr;
        stream.accept(prev);

        if(!stream.check(T_NAME)) {
            stream.accept(curr);
            return unexpected_follow_symbol(ctx, prev, curr, T_NAME);
        }

        stream.accept(curr);

        token var_loc;
        const char *var_name(nullptr);
        stream.accept(var_loc, var_name);

        return accept_expected_newline(ctx, stream);
    }

    bool parse_defun(diagnostic_context &ctx, token_stream &stream) throw() {
        assert(stream.check(T_DEF_FUNCTION));
        stream.accept();

        return accept_expected_newline(ctx, stream);
    }

    bool parse_deftype(diagnostic_context &ctx, token_stream &stream) throw() {
        assert(stream.check(T_DEF_TYPE));
        stream.accept();

        return accept_expected_newline(ctx, stream);
    }

    /// parse a function assignment to some expression.
    ///             fib(1) := 1
    ///             fib(0) := 1
    bool parse_func_assign(diagnostic_context &ctx, token_stream &stream) throw() {
        assert(stream.check(T_NAME));

        // get the function name
        token tok;
        const char *func_name(nullptr);
        stream.accept(tok, func_name);

        return accept_expected_newline(ctx, stream);
    }

    /// parse a stream of tokens
    bool parse(diagnostic_context &ctx, token_stream &stream) throw() {
        bool last_parsed(true);
        while(last_parsed && stream.check()) {

            // variable definition
            if(stream.check(T_LET)) {
                last_parsed = parse_let(ctx, stream);

            // function definition
            } else if(stream.check(T_DEF_FUNCTION)) {
                last_parsed = parse_defun(ctx, stream);

            // type definition
            } else if(stream.check(T_DEF_TYPE)) {
                last_parsed = parse_deftype(ctx, stream);

            // function assignment
            } else if(stream.check(T_NAME)) {
                last_parsed = parse_func_assign(ctx, stream);

            } else if(stream.check(T_NEW_LINE) || stream.check(T_STRING_LITERAL)) {
                stream.accept();
                continue;

            } else {
                break;
            }

        }

        return false;
    }

    /// -----------------------------------------------------------------------
    /// -----------------------------------------------------------------------
    /// -----------------------------------------------------------------------

    /// parse an open file
    static void parse_open_file(
        io::file<xy::io::read_tag> &ff,
        diagnostic_context &ctx,
        tokenizer &tt,
        bool &ret
    ) {
        token_stream stream(ctx, tt, ff);
        ret = parse(ctx, stream);
    }

    /// parse a file given a file name
    bool parse_file(diagnostic_context &ctx, const char *file_name) throw() {
        tokenizer tt;
        bool ret(false);

        if(!io::read::open_file(file_name, parse_open_file, ctx, tt, ret)) {
            ctx.diag.push(io::e_open_file, file_name);
        }

        if(!ret
        || ctx.diag.has_message(io::message_type::error)
        || ctx.diag.has_message(io::message_type::recoverable_error)
        || ctx.diag.has_message(io::message_type::failed_assertion)) {
            return false;
        }

        return true;
    }

    /// -----------------------------------------------------------------------
    /// -----------------------------------------------------------------------
    /// -----------------------------------------------------------------------

    bool parse_buffer(diagnostic_context &ctx, const char * const buffer) throw() {
        (void) ctx;
        (void) buffer;
        return false;
    }
}
