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

#include "xy/include/utf8/decoder.hpp"

namespace xy {

    /// expect a new line after somehting
    static bool accept_expected_newline(diagnostic_context &ctx, token_stream &stream) throw() {
        if(stream.check(T_NEW_LINE)) {
            stream.accept();
            return true;
        }

        // get the previous line
        stream.undo();
        const char *data(nullptr);
        token prev_tok;

        stream.accept(prev_tok, data);
        size_t col(prev_tok.column() + utf8::decoder::length(data));

        // diagnost pointing to immediately after this token
        ctx.diag.push(io::e_required_new_line);
        ctx.diag.push(io::c_file_line_col, ctx.file(), prev_tok.line(), col);
        ctx.diag.push(io::c_highlight,
            io::highlight_column(ctx.file(), prev_tok.line(), col)
        );

        return false;
    }

    /// parse a variable definition
    //          let module := import "file/name.xy"
    //          let one := 1
    //
    bool parse_let(diagnostic_context &ctx, token_stream &stream) throw() {
        assert(stream.check(T_LET));
        stream.accept();

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

    /// token stream for a file
    class file_token_stream : public token_stream {
    private:

        enum {
            NUM_BACKUP = 2,
            NUM_TOKENS = NUM_BACKUP + 1,
        };

        diagnostic_context &ctx;
        tokenizer &tt;
        io::file<io::read_tag> &f;

        char scratch[NUM_TOKENS][MAX_TOKEN_LENGTH];
        token tokens[NUM_TOKENS];
        size_t num_seen;
        unsigned backed_up;
        unsigned curr;

    public:

        file_token_stream(
            diagnostic_context &ctx_,
            tokenizer &tt_,
            io::file<io::read_tag> &f_
        ) throw()
            : token_stream()
            , ctx(ctx_)
            , tt(tt_)
            , f(f_)
            , num_seen(0)
            , backed_up(0)
            , curr(0)
        {
            memset(scratch[0], 0, array::size(scratch[0]));
            memset(scratch[1], 0, array::size(scratch[1]));
        }

        virtual ~file_token_stream(void) throw() { }

        /// check if we have a token
        virtual bool check(void) throw() {
            if(0 == backed_up) {
                if(!tt.get_token(f, ctx, tokens[curr], scratch[curr])) {
                    return false;
                }
                ++backed_up;
                ++num_seen;
            }
            return true;
        }

        /// check if we have a token and if the current token has a specific
        /// type
        virtual bool check(token_type type_) throw() {
            if(check()) {
                return type_ == tokens[curr % NUM_TOKENS].type();
            }
            return false;
        }

        /// accept and move past the current token
        virtual bool accept(void) throw() {
            if(check()) {
                curr = (curr + 1) % NUM_TOKENS;
                --backed_up;
                return true;
            }
            return false;
        }

        /// bind the current token, accept it, and move past
        virtual bool accept(token &tok, const char *&data) throw() {
            if(check()) {
                tok = tokens[curr];
                data = scratch[curr];

                curr = (curr + 1) % NUM_TOKENS;
                --backed_up;
                return true;
            }
            return false;
        }

        /// undo the acceptance of a token
        virtual void undo(void) throw() {
            if(num_seen > 0 && backed_up < NUM_BACKUP) {
                ++backed_up;
                curr = (curr + NUM_TOKENS - 1) % NUM_TOKENS;
            }
        }
    };

    /// parse an open file
    static void parse_open_file(
        io::file<xy::io::read_tag> &ff,
        diagnostic_context &ctx,
        tokenizer &tt,
        bool &ret
    ) {
        file_token_stream stream(ctx, tt, ff);
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
