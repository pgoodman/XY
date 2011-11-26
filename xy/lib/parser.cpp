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
#include "xy/include/cstring.hpp"

#include "xy/include/io/line_highlight.hpp"

namespace xy {

    ast *pop(std::vector<ast *> &stack) throw() {
        ast *back(stack.back());
        stack.pop_back();
        return back;
    }

    const parser::precedence_parser parser::expression_parsers[] = {

        {10,    T_NAME,             &parser::parse_literal,         &parser::parse_fail_s},
        {10,    T_TYPE_NAME,        &parser::parse_type_name,       &parser::parse_fail_s},
        {10,    T_STRING_LITERAL,   &parser::parse_literal,         &parser::parse_fail_s},
        {10,    T_INTEGER_LITERAL,  &parser::parse_literal,         &parser::parse_fail_s},
        {10,    T_RATIONAL_LITERAL, &parser::parse_literal,         &parser::parse_fail_s},

        // logical/bitwise operators
        {200,   T_AMPERSAND,        &parser::parse_fail_p,          &parser::parse_infix},
        {190,   T_HAT,              &parser::parse_fail_p,          &parser::parse_infix},
        {180,   T_PIPE,             &parser::parse_fail_p,          &parser::parse_infix},
        {180,   T_DOUBLE_ARROW,     &parser::parse_fail_p,          &parser::parse_infix},

        {100,   T_OPEN_PAREN,       &parser::parse_paren_expression,&parser::parse_application},
        {100,   T_OPEN_BRACKET,     &parser::parse_array_literal,   &parser::parse_array_access},
        {100,   T_OPEN_BRACE,       &parser::parse_record_literal,  &parser::parse_type_instantiation},

        // method-like
        {100,   T_PERIOD,           &parser::parse_fail_p,          &parser::parse_infix},

        {80,    T_ASTERISK,         &parser::parse_fail_p,          &parser::parse_infix},
        {80,    T_FORWARD_SLASH,    &parser::parse_prefix,          &parser::parse_infix},
        {80,    T_PERCENT,          &parser::parse_fail_p,          &parser::parse_infix},

        {75,    T_ARROW,            &parser::parse_fail_p,          &parser::parse_infix},

        {70,    T_PLUS,             &parser::parse_fail_p,          &parser::parse_infix},
        {70,    T_MINUS,            &parser::parse_prefix,          &parser::parse_infix},

        {40,    T_EQUAL,            &parser::parse_fail_p,          &parser::parse_infix},
        {40,    T_NOT_EQUAL,        &parser::parse_fail_p,          &parser::parse_infix},
        {40,    T_LESS_THAN,        &parser::parse_fail_p,          &parser::parse_infix},
        {40,    T_GREATER_THAN,     &parser::parse_fail_p,          &parser::parse_infix},

        {255,   T_INVALID,          &parser::parse_fail_p,          &parser::parse_fail_s},
    };

    const parser::precedence_parser parser::type_parsers[] = {
        //{10,    T_FUNCTION,         &parser::parse_type_function,   &parser::parse_fail_s},
        {100,   T_OPEN_PAREN,       &parser::parse_type_group,      &parser::parse_type_params},
        {80,    T_ASTERISK,         &parser::parse_fail_p,          &parser::parse_infix_type_operator<arrow_type_decl>},
        {75,    T_ARROW,            &parser::parse_fail_p,          &parser::parse_infix_type_operator<product_type_decl>},
        {70,    T_PLUS,             &parser::parse_fail_p,          &parser::parse_infix_type_operator<sum_type_decl>},

        {10,    T_TYPE_NAME,        &parser::parse_type_name,       &parser::parse_fail_s},
        {10,    T_UNION,            &parser::parse_union,           &parser::parse_fail_s},
        {10,    T_RECORD,           &parser::parse_record,          &parser::parse_fail_s},

        {255,   T_INVALID,          &parser::parse_fail_p,          &parser::parse_fail_s},
    };

    const parser::precedence_parser *parser::get_precedence_parser(const parser::precedence_parser *parsers, const token &tok) {
        for(; T_INVALID != parsers->pivot; ++parsers) {
            if(parsers->pivot == tok.type()) {
                break;
            }
        }
        return parsers;
    }

    /// parse an expression
    bool parser::parse_paren_expression(const token &, const char *) throw() {
        if(!parse(expression_parsers, 0)) {
            return false;
        }

        return consume(T_CLOSE_PAREN);
    }

    /// top-down operator precedence parser
    bool parser::parse(const parser::precedence_parser *parsers, uint8_t rbp) throw() {

#define INDENT \
    indent[++indent_index] = '\t';

#define DEDENT \
    indent[indent_index--] = '\0';

        static char indent[] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
        static int indent_index(-1);

        INDENT

        token curr;
        const char *curr_data;

        printf("%srbp is %u\n", indent, rbp);

        if(!stream.accept(curr, curr_data)) {
            DEDENT
            return false;
        }

        printf("%sa: got token %s with data '%s' at line %u, col %u\n", indent, curr.name(), curr_data, curr.line(), curr.column());

        const precedence_parser *prev_parser(get_precedence_parser(parsers, curr));

        printf("%s---parser is %s\n", indent, token::name(prev_parser->pivot));

        // no supported parser or failed to parse
        if(!(this->*(prev_parser->prefix))(curr, curr_data)) {
            DEDENT
            return false;
        }

        printf("%s---parsed prefix.\n", indent);

        // get the parser for the next token
        stream.accept(curr, curr_data);
        printf("%sb: got token %s with data '%s' at line %u, col %u\n", indent, curr.name(), curr_data, curr.line(), curr.column());
        const precedence_parser *curr_parser(get_precedence_parser(parsers, curr));
        printf("%s---parser is %s with left precedence %u\n", indent, token::name(curr_parser->pivot), curr_parser->precedence);

        // parsing won't get us anywhere, but we've successfully parsed
        // something already.
        if(&parser::parse_fail_s == curr_parser->postfix) {
            stream.undo();
            DEDENT
            return true;
        }

        for(; rbp < curr_parser->precedence; ) {

            printf("%s---parsing suffix with right precedence %u\n", indent, curr_parser->precedence & ~1);

            // failed the sub-parse
            if(!(this->*(curr_parser->postfix))(curr_parser->precedence & ~1, curr, curr_data)) {
                DEDENT
                return false;
            }

            printf("%s---parsed suffix.\n", indent);

            // we extended this parse :D who cares if there's another token or
            // not? let's leave that to the caller :D
            if(!stream.accept(curr, curr_data)) {
                DEDENT
                return true;
            }

            printf("%sc: got token %s with data '%s' at line %u, col %u\n", indent, curr.name(), curr_data, curr.line(), curr.column());

            curr_parser = get_precedence_parser(parsers, curr);

            printf("%s---parser is %s with left precedence %u\n", indent, token::name(curr_parser->pivot), curr_parser->precedence);

            if(&parser::parse_fail_s == curr_parser->postfix) {
                stream.undo();
                DEDENT
                return true;
            }
        }

        printf("%sinput precedence %u was not less than token precedence %u \n", indent, rbp, curr_parser->precedence);

        // we exited the loop, i.e. we had something that could be parsed but
        // whose precedence level is incorrect, i.e. should be handled by
        // someone else, so unread the token.
        stream.undo();

        DEDENT

        return true;
    }

    /// parse a function call or template instantiation
    bool parser::parse_application(uint8_t, const token &paren, const char *) throw() {
        ast *node(pop(stack));

        // function call
        if(node->is_instance<expression>()) {
            expression *function(node->reinterpret<expression>());
            stack.push_back(new function_call_expr(function));

        // template instantiation
        } else if(node->is_instance<type_decl>()) {
            type_decl *decl(node->reinterpret<type_decl>());
            stack.push_back(new template_instance_type_decl(decl));

        // wtf?
        } else {
            ctx.report_here(paren, io::e_type_decl_or_func_expr_expected);
            delete node;
            node = nullptr;
            return false;
        }

        return true;
    }

    /// parse the { ...} parse of a type instantiation, e.g. Int{1}, or Array(Int, 3){1,2,3}.
    bool parser::parse_type_instantiation(uint8_t, const token &tok, const char *) throw() {

        ast *decl_(pop(stack));

        if(!decl_->is_instance<type_decl>()) {
            delete decl_;
            return report_simple(io::e_not_a_type_for_instance, tok);
        }

        bool consume_comma(false);
        type_instance_expr *inst(new type_instance_expr(decl_->reinterpret<type_decl>()));
        token expr_begin;
        ast *val(nullptr);

        for(; !stream.check(T_CLOSE_BRACE); consume_comma = true, val = nullptr) {
            if(consume_comma && !consume(T_COMMA)) {
                delete inst;
                return false;
            }

            if(stream.check(T_CLOSE_BRACE)) {
                break;
            }

            // just so that we know the position of where the expression was
            // meant to begin
            stream.accept(expr_begin);
            stream.undo();

            if(!parse(expression_parsers, 0)) {
                delete inst;
                return false;
            }

            val = pop(stack);

            // we didn't get an expression
            if(!val->is_instance<expression>()) {
                delete val;
                delete inst;
                return report_simple(io::e_not_an_expression_for_type_instance, expr_begin);
            }

            // collect the sub-expression :D
            inst->values.push_back(val->reinterpret<expression>());
        }

        stack.push_back(inst);

        return consume(T_CLOSE_BRACE);
    }

    bool parser::parse_array_access(uint8_t, const token &tok, const char *) throw() {
        ast *left_(pop(stack));
        ast *right_(nullptr);

        if(!left_->is_instance<expression>()) {
            delete left_;
            return report_simple(io::e_array_access_on_non_expr, tok);
        }

        expression *left(left_->reinterpret<expression>());

        if(!parse(expression_parsers, 0) || !consume(T_CLOSE_BRACKET)) {
            delete left;
            return false;
        }

        right_ = pop(stack);

        if(!right_->is_instance<expression>()) {
            delete left_;
            delete right_;
            return report_simple(io::e_array_access_on_non_expr, tok);
        }

        stack.push_back(new array_access_expr(left, right_->reinterpret<expression>()));
        return true;
    }

    bool parser::parse_infix(uint8_t prec, const token &tok, const char *data) throw() {

        // looks like we're parsing an inline type declaration
        if(stack.back()->is_instance<type_decl>()) {
            switch(tok.type()) {
            case T_ARROW: return parse_infix_type_operator<arrow_type_decl>(prec, tok, data);
            case T_PLUS: return parse_infix_type_operator<sum_type_decl>(prec, tok, data);
            case T_ASTERISK: return parse_infix_type_operator<product_type_decl>(prec, tok, data);
            default:
                return report_simple(io::e_bad_type_operator, tok);
            }
        }

        // arrow in expression context
        if(T_ARROW == tok.type()) {
            return report_simple(io::e_arrow_in_expr_context, tok);
        }

        // parse the right thing
        if(!parse(expression_parsers, prec)) {
            return false;
        }

        expression *right(pop(stack)->reinterpret<expression>());
        expression *left(pop(stack)->reinterpret<expression>());

        stack.push_back(new infix_expr(left, right, tok.type()));
        return true;
    }

    bool parser::parse_prefix(const token &, const char *) throw() {
        return false;
    }

    /// parse something that looks like an array literal. This might actually
    /// end up parsing a inline type declaration if the first thing in the
    /// array literal is a type declararion, making this an array type
    /// declaration.
    bool parser::parse_array_literal(const token &, const char *) throw() {

        // empty array
        if(stream.check(T_CLOSE_BRACKET)) {
            stream.accept();
            stack.push_back(new array_expr);
            return true;
        }

        if(!parse(expression_parsers, 0)) {
            return false;
        }

        ast *first(pop(stack));

        if(first->is_instance<type_decl>()) {
            stack.push_back(new array_type_decl(first->reinterpret<type_decl>()));
            return consume(T_CLOSE_BRACKET);

        } else if(first->is_instance<expression>()) {
            array_expr *arr(new array_expr);
            arr->elements.push_back(first->reinterpret<expression>());

            token expr_head;
            for(; !stream.check(T_CLOSE_BRACKET);) {
                if(!consume(T_COMMA)) {
                    delete arr;
                    return false;
                }

                if(stream.check(T_CLOSE_BRACKET)) {
                    break;
                }

                stream.accept(expr_head);
                stream.undo();

                if(!parse(expression_parsers, 0)) {
                    delete arr;
                    return false;
                }

                first = pop(stack);

                // add the array element into the array
                if(first->is_instance<expression>()) {
                    arr->elements.push_back(first->reinterpret<expression>());

                // found a non-expression array element
                } else {
                    delete first;
                    delete arr;
                    return report_simple(io::e_array_element_must_be_expr, expr_head);
                }
            }

            stack.push_back(arr);

            return consume(T_CLOSE_BRACKET);
        } else {
            return false;
        }
    }

    bool parser::parse_record_literal(const token &, const char *) throw() {
        return false;
    }

    bool parser::parse_literal(const token &tok, const char *data) throw() {
        switch(tok.type()) {
        case T_INTEGER_LITERAL:
            stack.push_back(new integer_literal_expr(cstring::copy(data)));
            return true;
        case T_STRING_LITERAL:
            stack.push_back(new string_literal_expr(cstring::copy(data)));
            return true;
        case T_RATIONAL_LITERAL:
            stack.push_back(new rational_literal_expr(cstring::copy(data)));
            return true;
        default:
            return false;
        }
    }

    bool parser::parse_type_name(const token &, const char *name) throw() {
        stack.push_back(new named_type_decl(stab[name]));
        return true;
    }
    bool parser::parse_union(const token &, const char *) throw() {
        return true;
    }
    bool parser::parse_record(const token &, const char *) throw() {
        return true;
    }
    bool parser::parse_type_function(const token &, const char *) throw() {
        return true;
    }
    bool parser::parse_type_group(const token &, const char *) throw() {
        return true;
    }
    bool parser::parse_type_params(uint8_t, const token &, const char *) throw() {
        return true;
    }

    /*
    bool parser::parse_sum_type(uint8_t, const token &, const char *) throw() {
        return true;
    }
    bool parser::parse_product_type(uint8_t, const token &, const char *) throw() {
        return true;
    }*/

    bool parser::parse_ref_type(const token &, const char *) throw() {
        return true;
    }
    bool parser::parse_array_type(const token &, const char *) throw() {
        return true;
    }

    bool parser::parse_fail_p(const token &, const char *) throw() {
        return false;
    }

    bool parser::parse_fail_s(uint8_t, const token &, const char *) throw() {
        return false;
    }

    /// go get a comma-separated list of names, e.g. variable names or type
    /// names
    bool parser::parse_name_list(
        token_type expected,
        std::vector<std::pair<token, support::mapped_name> > &names
    ) throw() {
        token prev_token, got;
        const char *name_buff;

        for(bool check_comma(false); ; check_comma = true) {

            // make sure names are comma-separated
            if(check_comma) {
                if(stream.check(T_ASSIGN)) {
                    return true;
                } else if(!stream.check(T_COMMA)) {
                    stream.accept(got);
                    stream.undo();
                    report_unexpected_follow_symbol_2(prev_token, got, T_COMMA, T_ASSIGN);
                    return false;
                } else {
                    stream.accept(prev_token);
                }
            }

            // didn't get the name
            if(!stream.check(expected)) {
                assert(check_comma);

                stream.accept(got);
                report_unexpected_follow_symbol(prev_token, got, expected);
                return false;
            }

            // collect the name
            stream.accept(prev_token, name_buff);
            names.push_back(std::make_pair(prev_token, stab[name_buff]));
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
        std::vector<std::pair<token, support::mapped_name> > names;
        bool check_sep(false);
        unsigned last_seen(0);

        stream.accept(prev);

        if(!stream.check(T_NAME) && !stream.check(T_TYPE_NAME)){
            stream.accept(curr);
            return report_unexpected_follow_symbol_2(prev, curr, T_NAME, T_TYPE_NAME);

        // variable/func declaration
        } else if(stream.check(T_NAME)) {
            if(!parse_name_list(T_NAME, names) || !consume(T_ASSIGN)) {
                return false;
            }

            // for each variable name
            //for(auto &name_pair : names) {
            for(size_t i(0); i < names.size(); ++i) {
                if(check_sep && !consume(T_COMMA)) {
                    break;
                }

                if(!parse(expression_parsers, 0)) {
                    break;
                }

                (void) names[i]; // TODO

                check_sep = true;
                ++last_seen;
            }

            if(last_seen < names.size()) {
                ctx.report_here(names[last_seen].first, io::e_missing_expr_in_let, stab[names[last_seen].second]);
                return false;
            }

        // type declaration
        } else {
            if(!parse_name_list(T_TYPE_NAME, names) || !consume(T_ASSIGN)) {
                return false;
            }

            // for each variable name
            for(size_t i(0); i < names.size(); ++i) {
                if(check_sep && !consume(T_COMMA)) {
                    break;
                }

                // TODO
                if(!parse(type_parsers, 0)) {
                    break;
                }

                (void) names[i]; // TODO

                check_sep = true;
                ++last_seen;
            }

            if(last_seen < names.size()) {

                ctx.report_here(names[last_seen].first, io::e_missing_type_decl_in_let, stab[names[last_seen].second]);
                return false;
            }
        }

        return true;
    }

    bool parser::consume(token_type expected) throw() {
        if(!stream.check(expected)) {
            token got;
            stream.accept(got);

            ctx.report(io::e_expected_different_symbol, got.name(), token::name(expected));
            ctx.report(io::c_file_line_col, ctx.file(), got.line(), got.column());
            ctx.report(io::c_highlight, io::highlight_line(
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
        , stack()
    { }

    parser::~parser(void) throw() {
        while(!stack.empty()) {
            delete stack.back();
            stack.back() = nullptr;
            stack.pop_back();
        }
    }

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

            // done parsing this file
            if(stream.check(T_EOF)) {
                break;
            }

            // variable definition
            else if(stream.check(T_LET)) {
                last_parsed = p.parse_let();

            // report_unexpected token
            } else if(stream.check() ){
                token got;
                stream.accept(got);

                p.report_simple(io::e_unexpected_symbol, got);

                break;
            } else {
                break;
            }
        }

        p.consume(T_EOF);

        if(ctx.has_message(io::message_type::error)
        || ctx.has_message(io::message_type::recoverable_error)
        || ctx.has_message(io::message_type::failed_assertion)) {
            return false;
        }

        p.stab.pop_context();

        return true;
    }

    /// -----------------------------------------------------------------------
    /// -----------------------------------------------------------------------
    /// -----------------------------------------------------------------------

    bool parser::report_simple(io::message_id id, const token &got) throw() {
        ctx.report_here(got, id, got.name());
        return false;
    }

    /// error that we got an report_unexpected token after some first token that we
    /// would normally use as a prefix
    bool parser::report_unexpected_follow_symbol(
        const token &first, const token &got, token_type expected
    ) throw() {
        ctx.report_here(got, io::e_unexpected_follow_symbol,
            got.name(), token::name(expected), first.name()
        );
        return false;
    }

    /// error that we got an report_unexpected token after some first token that we
    /// would normally use as a prefix
    bool parser::report_unexpected_follow_symbol_2(
        const token &first, const token &got, token_type expected1, token_type expected2
    ) throw() {
        ctx.report_here(got, io::e_unexpected_follow_symbol_2,
            got.name(), token::name(expected1), token::name(expected2), first.name()
        );
        return false;
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
            ctx.report(io::e_open_file, file_name);
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

