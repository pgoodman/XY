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

#include "xy/include/io/line_highlight.hpp"

namespace xy {

    const parser::expression_parser parser::expression_parsers[]{

        {10,    T_NAME,             &parser::parse_literal,         &parser::parse_fail_s},
        {10,    T_TYPE_NAME,        &parser::parse_literal,         &parser::parse_fail_s},
        {10,    T_STRING_LITERAL,   &parser::parse_literal,         &parser::parse_fail_s},
        {10,    T_INTEGER_LITERAL,  &parser::parse_literal,         &parser::parse_fail_s},
        {10,    T_RATIONAL_LITERAL, &parser::parse_literal,         &parser::parse_fail_s},

        {100,   T_OPEN_PAREN,       &parser::parse_paren_expression,&parser::parse_application},
        {100,   T_OPEN_BRACKET,     &parser::parse_array_literal,   &parser::parse_array_access},
        {100,   T_OPEN_BRACE,       &parser::parse_record_literal,  &parser::parse_fail_s},
        {100,   T_PERIOD,           &parser::parse_fail_p,          &parser::parse_infix},

        {80,    T_ASTERISK,         &parser::parse_fail_p,          &parser::parse_infix},
        {80,    T_FORWARD_SLASH,    &parser::parse_prefix,          &parser::parse_infix},
        {80,    T_PERCENT,          &parser::parse_fail_p,          &parser::parse_infix},

        {70,    T_PLUS,             &parser::parse_fail_p,          &parser::parse_infix},
        {70,    T_MINUS,            &parser::parse_prefix,          &parser::parse_infix},

        {40,    T_EQUAL,            &parser::parse_fail_p,          &parser::parse_infix},
        {40,    T_NOT_EQUAL,        &parser::parse_fail_p,          &parser::parse_infix},
        {40,    T_LESS_THAN,        &parser::parse_fail_p,          &parser::parse_infix},
        {40,    T_GREATER_THAN,     &parser::parse_fail_p,          &parser::parse_infix},

        {255,   T_INVALID,          &parser::parse_fail_p,          &parser::parse_fail_s},
    };

    const parser::expression_parser *parser::get_sub_parser(const token &tok) {
        for(const expression_parser &p : expression_parsers) {
            if(p.pivot == tok.type()) {
                return &p;
            }
        }
        return nullptr;
    }

    /// parse an expression
    bool parser::parse_paren_expression(const token &, const char *) throw() {
        if(!parse_expression(0)) {
            return false;
        }

        return consume(T_CLOSE_PAREN);
    }

    /// top-down operator precedence parser
    bool parser::parse_expression(uint8_t prec) throw() {
        token curr;
        const char *curr_data;

        if(!stream.accept(curr, curr_data)) {
            return false;
        }

        const expression_parser *prev_parser(get_sub_parser(curr));

        // no supported parser or failed to parse
        if(nullptr == prev_parser || !(this->*(prev_parser->prefix))(curr, curr_data)) {
            return false;
        }

        printf("parsed with %s = '%s'\n", curr.name(), curr_data);

        // get the parser for the next token
        stream.accept(curr, curr_data);
        const expression_parser *curr_parser(get_sub_parser(curr));

        // parsing won't get us anywhere, but we've successfully parsed
        // something already.
        if(nullptr == curr_parser
        || &parser::parse_fail_s == curr_parser->postfix) {
            printf("undid with %s = '%s'\n", curr.name(), curr_data);
            stream.undo();
            return true;
        }

        printf("trying to continue with %s = '%s'\n", curr.name(), curr_data);

        for(; prec < curr_parser->precedence; ) {

            // failed the sub-parse
            if(!(this->*(curr_parser->postfix))(curr_parser->precedence & -1, curr, curr_data)) {
                return false;
            }

            // we extended this parse :D who cares if there's another token or
            // not? let's leave that to the caller :D
            if(!stream.accept(curr, curr_data)) {
                return true;
            }

            curr_parser = get_sub_parser(curr);
            if(nullptr == curr_parser
            || &parser::parse_fail_s == curr_parser->postfix) {
                stream.undo();
                return true;
            }
        }

        printf("existing tdop loop\n");

        // we exited the loop, i.e. we had something that could be parsed but
        // whose precedence level is incorrect, i.e. should be handled by
        // someone else, so unread the token.
        stream.undo();

        return true;
    }

    /// parse a function call
    bool parser::parse_application(uint8_t prec, const token &, const char *) throw() {
        (void) prec;
        return false;
    }

    bool parser::parse_array_access(uint8_t prec, const token &, const char *) throw() {
        (void) prec;
        return false;
    }

    bool parser::parse_infix(uint8_t prec, const token &, const char *) throw() {
        return parse_expression(prec);
    }

    bool parser::parse_prefix(const token &, const char *) throw() {
        return false;
    }

    bool parser::parse_array_literal(const token &, const char *) throw() {
        return false;
    }

    bool parser::parse_record_literal(const token &, const char *) throw() {
        return false;
    }

    bool parser::parse_literal(const token &, const char *ss) throw() {
        printf("parsed '%s'\n", ss);
        return true;
    }

    bool parser::parse_fail_p(const token &, const char *) throw() {
        return false;
    }

    bool parser::parse_fail_s(uint8_t, const token &, const char *) throw() {
        return false;
    }

    /// expect a new line after somehting
    /*bool parser::accept_expected_newline(void) throw() {
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
    }*/

    /// error that we got an unexpected token after some first token that we
    /// would normally use as a prefix
    bool parser::unexpected_follow_symbol(
        token &first, token &got, token_type expected
    ) throw() {

        ctx.diag.push(io::e_unexpected_follow_symbol,
            got.name(), token::name(expected), first.name()
        );
        ctx.diag.push(io::c_file_line_col, ctx.file(), got.line(), got.column());
        ctx.diag.push(io::c_highlight, io::highlight_line(
            ctx.file(), got.line(), got.column(), got.end_column()
        ));
        return false;
    }

    /// error that we got an unexpected token after some first token that we
    /// would normally use as a prefix
    bool parser::unexpected_follow_symbol_2(
        token &first, token &got, token_type expected1, token_type expected2
    ) throw() {

        ctx.diag.push(io::e_unexpected_follow_symbol_2,
            got.name(), token::name(expected1), token::name(expected2), first.name()
        );
        ctx.diag.push(io::c_file_line_col, ctx.file(), got.line(), got.column());
        ctx.diag.push(io::c_highlight, io::highlight_line(
            ctx.file(), got.line(), got.column(), got.end_column()
        ));
        return false;
    }

    /// go get a comma-separated list of names, e.g. variable names or type
    /// names
    bool parser::parse_name_list(
        token_type expected,
        std::vector<std::string> &names
    ) throw() {
        token prev_token, got;
        const char *name_buff;
        std::string name;

        for(bool check_comma(false); ; check_comma = true) {

            // make sure names are comma-separated
            if(check_comma) {
                if(stream.check(T_ASSIGN)) {
                    return true;
                } else if(!stream.check(T_COMMA)) {
                    stream.accept(got);
                    stream.undo();
                    unexpected_follow_symbol_2(prev_token, got, T_COMMA, T_ASSIGN);
                    return false;
                } else {
                    stream.accept(prev_token);
                }
            }

            // didn't get the name
            if(!stream.check(expected)) {
                assert(check_comma);

                stream.accept(got);
                unexpected_follow_symbol(prev_token, got, expected);
                return false;
            }

            // collect the name
            stream.accept(prev_token, name_buff);
            name = name_buff;
            names.push_back(name);
        }

        return true;
    }

    /// parse a variable definition
    //          let file:name := import "file/name.xy"
    //          let file:name:foo := (import "file/name.xy").foo
    //          let one := 1
    //          let two : Int = 2
    // or a function definition
    //          let foo :: ...
    // or a type declaration
    //          let Foo
    bool parser::parse_let(void) throw() {
        assert(stream.check(T_LET));
        token prev;
        token curr;
        std::vector<std::string> names;
        bool check_sep(false);

        stream.accept(prev);

        if(!stream.check(T_NAME) && !stream.check(T_TYPE_NAME)){
            stream.accept(curr);
            return unexpected_follow_symbol_2(prev, curr, T_NAME, T_TYPE_NAME);

        // variable/func declaration
        } else if(stream.check(T_NAME)) {
            if(!parse_name_list(T_NAME, names)) {
                return false;
            }

            assert(stream.check(T_ASSIGN));
            stream.accept(prev);

            // for each variable name
            for(std::string &name : names) {

                printf("about to look for sep\n");

                if(check_sep && !consume(T_COMMA)) {
                    printf("didn't get the comma ??\n");
                    return false;
                }

                printf("parsing expression for name '%s'\n", name.c_str());
                if(!parse_expression(0)) {
                    printf("failed to parse :(\n");
                    return false;
                }

                check_sep = true;
            }

        // type declaration
        } else {
            if(!parse_name_list(T_TYPE_NAME, names)) {
                return false;
            }

            //is_type_decl = true;
        }

        return true;
    }

    bool parser::consume(token_type expected) throw() {
        if(!stream.check(expected)) {
            token got;
            stream.accept(got);

            ctx.diag.push(io::e_unexpected_symbol, got.name(), token::name(expected));
            ctx.diag.push(io::c_file_line_col, ctx.file(), got.line(), got.column());
            ctx.diag.push(io::c_highlight, io::highlight_line(
                ctx.file(), got.line(), got.column(), got.end_column()
            ));

            return false;
        }

        return stream.accept();
    }

    /// constructor
    parser::parser(diagnostic_context &ctx_, token_stream &stream_) throw()
        : ctx(ctx_)
        , stab()
        , stream(stream_)
    { }

    /// parse a stream of tokens
    bool parser::parse(diagnostic_context &ctx, token_stream &stream) throw() {

        parser p(ctx, stream);
        stream.ctx = ctx;
        p.stab.push_context();

        bool last_parsed(true);
        while(last_parsed && stream.check()) {

            while(stream.check(T_STRING_LITERAL)
               || stream.check(T_INTEGER_LITERAL)
               || stream.check(T_RATIONAL_LITERAL)) {
                stream.accept();
            }

            // variable definition
            if(stream.check(T_LET)) {
                last_parsed = p.parse_let();

            // unexpected token
            } else if(stream.check() ){
                token got;
                stream.accept(got);

                ctx.diag.push(io::e_unexpected_symbol, got.name());
                ctx.diag.push(io::c_file_line_col, ctx.file(), got.line(), got.column());
                ctx.diag.push(io::c_highlight, io::highlight_line(
                    ctx.file(), got.line(), got.column(), got.end_column()
                ));

                break;
            } else {
                break;
            }

        }

        if(ctx.diag.has_message(io::message_type::error)
        || ctx.diag.has_message(io::message_type::recoverable_error)
        || ctx.diag.has_message(io::message_type::failed_assertion)) {
            return false;
        }

        p.stab.pop_context();

        return true;
    }

    /// -----------------------------------------------------------------------
    /// -----------------------------------------------------------------------
    /// -----------------------------------------------------------------------

    /// parse an open file
    void parser::parse_open_file(io::file<xy::io::read_tag> &ff, diagnostic_context &ctx, bool &ret) throw() {
        tokenizer tt;
        token_stream stream(ctx, tt, ff);
        ret = parse(ctx, stream);
    }

    /// parse a file given a file name
    bool parser::parse_file(diagnostic_context &ctx, const char *file_name) throw() {
        bool ret(false);

        if(!io::read::open_file(file_name, parser::parse_open_file, ctx, ret)) {
            ctx.diag.push(io::e_open_file, file_name);
        }
        return ret;
    }

    /// -----------------------------------------------------------------------
    /// -----------------------------------------------------------------------
    /// -----------------------------------------------------------------------

    bool parser::parse_buffer(diagnostic_context &ctx, const char * const buffer) throw() {
        (void) ctx;
        (void) buffer;
        return false;
    }
}
