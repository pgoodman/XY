/*
 * parser.cpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include <cassert>
#include <sstream>

#define D(x) x

#include "xy/include/parser.hpp"
#include "xy/include/tokenizer.hpp"
#include "xy/include/array.hpp"
#include "xy/include/cstring.hpp"
#include "xy/include/support/cstring_reader.hpp"
#include "xy/include/repl/repl.hpp"

#include "xy/include/io/line_highlight.hpp"

namespace xy {

    ast *pop(std::vector<ast *> &stack) throw() {
        ast *back(stack.back());
        printf("popped %p\n", reinterpret_cast<void *>(back));
        stack.back() = support::unsafe_cast<ast *>(reinterpret_cast<void *>(0xDEADBEEF));
        stack.pop_back();
        return back;
    }

    const parser::precedence_parser parser::expression_parsers[] = {

        {10,    T_NAME,             &parser::parse_literal,         &parser::parse_fail_s},
        {10,    T_TYPE_NAME,        &parser::parse_type_name,       &parser::parse_fail_s},
        {10,    T_STRING_LITERAL,   &parser::parse_literal,         &parser::parse_string_concat},
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

        {80,    T_ASTERISK,         &parser::parse_prefix,          &parser::parse_infix},
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
        {100,   T_OPEN_PAREN,       &parser::parse_type_group,      &parser::parse_application},
        {80,    T_ASTERISK,         &parser::parse_fail_p,          &parser::parse_infix_type_operator<product_type_declaration>},
        {75,    T_ARROW,            &parser::parse_fail_p,          &parser::parse_arrow_type_operator},
        {70,    T_PLUS,             &parser::parse_fail_p,          &parser::parse_infix_type_operator<sum_type_declaration>},

        {10,    T_TYPE_NAME,        &parser::parse_type_name,       &parser::parse_fail_s},
        {10,    T_TYPE_TYPE,        &parser::parse_type_type_declaration,  &parser::parse_fail_s},
        {10,    T_TYPE_UNIT,        &parser::parse_unit_type_declaration,  &parser::parse_fail_s},
        //{10,    T_UNION,            &parser::parse_union,           &parser::parse_fail_s},
        //{10,    T_RECORD,           &parser::parse_record,          &parser::parse_fail_s},

        {255,   T_INVALID,          &parser::parse_fail_p,          &parser::parse_fail_s},
    };

    const parser::precedence_parser *parser::get_precedence_parser(
        const parser::precedence_parser *parsers,
        const token &tok
    ) throw() {
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
    bool parser::parse(const parser::precedence_parser *parsers, unsigned rbp) throw() {

#define INDENT \
    indent[++indent_index] = '\t';

#define DEDENT \
    indent[indent_index--] = '\0';

        static char indent[] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
        static int indent_index(-1);

        INDENT

        token curr;
        const char *curr_data;

        D( printf("%srbp is %u\n", indent, rbp); )

        if(!stream.accept(curr, curr_data)) {
            DEDENT
            return false;
        }

        D( printf("%sa: got token %s with data '%s' at line %u, col %u\n", indent, curr.name(), curr_data, curr.line(), curr.column()); )

        const precedence_parser *prev_parser(get_precedence_parser(parsers, curr));

        D( printf("%s---parser is %s\n", indent, token::name(prev_parser->pivot)); )

        // no supported parser or failed to parse
        repl::wait();
        if(!(this->*(prev_parser->prefix))(curr, curr_data)) {
            DEDENT
            repl::accept();
            return false;
        }
        repl::accept();

        D( printf("%s---parsed prefix.\n", indent); )

        // get the parser for the next token
        stream.accept(curr, curr_data);
        D( printf("%sb: got token %s with data '%s' at line %u, col %u\n", indent, curr.name(), curr_data, curr.line(), curr.column()); )
        const precedence_parser *curr_parser(get_precedence_parser(parsers, curr));
        D( printf("%s---parser is %s with left precedence %u\n", indent, token::name(curr_parser->pivot), curr_parser->precedence); )

        // parsing won't get us anywhere, but we've successfully parsed
        // something already.
        if(&parser::parse_fail_s == curr_parser->postfix) {
            stream.undo();
            DEDENT
            return true;
        }

        for(; rbp < curr_parser->precedence; ) {

            D( printf("%s---parsing suffix with right precedence %u\n", indent, curr_parser->precedence & ~1); )

            // failed the sub-parse
            repl::wait();
            if(!(this->*(curr_parser->postfix))(curr_parser->precedence & ~1, curr, curr_data)) {
                DEDENT
                repl::accept();
                return false;
            }
            repl::accept();

            D( printf("%s---parsed suffix.\n", indent); )

            // we extended this parse :D who cares if there's another token or
            // not? let's leave that to the caller :D
            if(!stream.accept(curr, curr_data)) {
                DEDENT
                return true;
            }

            D( printf("%sc: got token %s with data '%s' at line %u, col %u\n", indent, curr.name(), curr_data, curr.line(), curr.column()); )

            curr_parser = get_precedence_parser(parsers, curr);

            D( printf("%s---parser is %s with left precedence %u\n", indent, token::name(curr_parser->pivot), curr_parser->precedence); )

            if(&parser::parse_fail_s == curr_parser->postfix) {
                stream.undo();
                DEDENT
                return true;
            }
        }

        D( printf("%sinput precedence %u was not less than token precedence %u \n", indent, rbp, curr_parser->precedence); )

        // we exited the loop, i.e. we had something that could be parsed but
        // whose precedence level is incorrect, i.e. should be handled by
        // someone else, so unread the token.
        stream.undo();

        DEDENT

        return true;
    }

    /// parse out two types for a type operator
    bool parser::parse_type_operands(unsigned prec, const token &op, type_declaration **ll, type_declaration **rr) throw() {
        ast *left_(pop(stack));

        if(!left_->is_instance<type_declaration>()) {
            delete left_;
            return report_simple(io::e_type_declaration_expected_before_type_op, op);
        }

        token location;
        type_declaration *left(left_->reinterpret<type_declaration>());
        location = left->location;
        location.extend(op);

        // get a "pivot" token just in case we run into an error.
        token decl_tail;
        stream.accept(decl_tail);
        stream.undo();

        printf("about to parse right operand...\n");

        // parse the right-hand operand
        if(!parse(type_parsers, prec)) {
            delete left;
            if(T_TYPE_NAME == decl_tail.type()) {
                ctx.report_here(decl_tail, io::e_incomplete_tpl_inst);
            } else {
                ctx.report_here(decl_tail, io::e_bad_suffix_type_declaration, decl_tail.name());
            }
            return false;
        }

        type_declaration *right(pop(stack)->reinterpret<type_declaration>());

        printf("left=%p  right=%p\n", reinterpret_cast<void *>(left), reinterpret_cast<void *>(right));

        *ll = left;
        *rr = right;

        return true;
    }

    /// parse an arrow type operator, which is right associative
    bool parser::parse_arrow_type_operator(unsigned prec, const token &op, const char *) throw() {

        type_declaration *left(nullptr);
        type_declaration *right(nullptr);

        printf("parsing arrow type\n");

        if(!parse_type_operands(prec, op, &left, &right)) {
            return false;
        }

        arrow_type_declaration *left_op(left->reinterpret<arrow_type_declaration>());
        arrow_type_declaration *right_op(right->reinterpret<arrow_type_declaration>());

        printf("left_op=%p  left=%p\n", reinterpret_cast<void *>(left_op), reinterpret_cast<void *>(left));
        printf("right_op=%p  right=%p\n", reinterpret_cast<void *>(right_op), reinterpret_cast<void *>(right));

        if(nullptr == right_op) {
            right_op = new arrow_type_declaration;
            right_op->types.push_back(right);
        }

        printf("right_op=%p  right=%p\n", reinterpret_cast<void *>(right_op), reinterpret_cast<void *>(right));

        /*std::ostringstream lss;
        std::ostringstream rss;

        left->print(lss, stab);
        right->print(rss, stab);
        */

        token location(left->location);
        location.extend(op);
        location.extend(right->location);

        // left is non-arrow, or wrapped arrow
        if(nullptr == left_op || left_op->is_wrapped) {
            //printf("unshifting '%s' into '%s'\n", lss.str().c_str(), rss.str().c_str());
            unshift(right_op->types, left);

        // unusual: left is arrow, but non-wrapped
        } else {
            extend(left_op->types, right_op->types);
            //printf("extending '%s' with '%s'\n", lss.str().c_str(), rss.str().c_str());
            right_op->types.clear();
            delete right_op;
            right_op = left_op;
        }

        right_op->location = location;
        stack.push_back(right_op);

        return true;
    }

    /// parse a function call or template instantiation
    bool parser::parse_application(unsigned, const token &paren, const char *) throw() {
        ast *node(pop(stack));

        // function call
        if(node->is_instance<expression>()) {

            //D( printf("looks like a function application!\n"); )

            expression *function(node->reinterpret<expression>());
            function_call_expression *funcall(new function_call_expression(function));

            if(!parse_params<ast>(io::e_tpl_arg_not_ast, T_SEMICOLON, funcall->template_parameters)) {
                delete funcall;
                return false;
            }

            // looks like a function template
            if(stream.check(T_SEMICOLON)) {

                funcall->might_be_ambiguous = false;

                token semicolon;
                stream.accept(semicolon);

                if(!parse_params<expression>(io::e_func_arg_not_expr, T_CLOSE_PAREN, funcall->arguments)) {
                    delete funcall;
                    return false;
                }

                // empty type parameters to template function and function arguments,
                // e.g. foo(;)
                if(funcall->template_parameters.empty() && funcall->arguments.empty()) {
                    ctx.report_here(semicolon, io::w_bin_empty_tpl_and_func_args);
                }

            } else {
                funcall->might_be_ambiguous = true;
                for(size_t i(0); i < funcall->template_parameters.size(); ++i) {
                    if(funcall->template_parameters[i]->is_instance<type_declaration>()) {
                        funcall->might_be_ambiguous = false;
                        break;
                    }
                }
            }

            consume(T_CLOSE_PAREN);

            // function is clearly not a function
            if(node->is_instance<literal_expression>()) {

                // try to merge adjacent string literals
                if(node->is_instance<string_literal_expression>()
                && funcall->might_be_ambiguous
                && 1 == funcall->template_parameters.size()
                && funcall->template_parameters[0]->is_instance<string_literal_expression>()) {

                    string_literal_expression *lhs_str(node->reinterpret<string_literal_expression>());
                    string_literal_expression *rhs_str(funcall->template_parameters[0]->reinterpret<string_literal_expression>());

                    funcall->function = nullptr;
                    funcall->template_parameters[0] = nullptr;
                    funcall->template_parameters.clear();
                    delete funcall;

                    stack.push_back(lhs_str);

                    parse_string_concat(0, paren, rhs_str->data);
                    delete rhs_str;

                    rhs_str = nullptr;
                    funcall = nullptr;

                    return true;
                }

                // error!
                delete funcall;
                ctx.report_here(paren, io::e_func_lhs_is_literal);
                return false;
            }

            stack.push_back(funcall);

        // template instantiation
        } else if(node->is_instance<type_declaration>()) {
            type_declaration *decl(node->reinterpret<type_declaration>());
            template_instance_type_declaration *tplinst(new template_instance_type_declaration(decl));
            tplinst->location = decl->location;
            tplinst->location.extend(paren);

            if(!parse_params<ast>(io::e_tpl_arg_not_ast, T_CLOSE_PAREN, tplinst->parameters)) {
                delete tplinst;
                return false;
            }

            // try to extend to the end of the template instantiation
            token close_paren;
            stream.accept(close_paren);
            stream.undo();

            if(!consume(T_CLOSE_PAREN)) {
                delete tplinst;
                return false;
            }

            tplinst->location.extend(close_paren);

            if(tplinst->parameters.empty()) {
                ctx.report_here(paren, io::e_tpl_must_have_args);
                delete tplinst;
                return false;
            }

            stack.push_back(tplinst);

        // wtf?
        } else {
            ctx.report_here(paren, io::e_type_declaration_or_func_expr_expected);
            delete node;
            node = nullptr;
            return false;
        }

        return true;
    }

    /// parse the { ...} parse of a type instantiation, e.g. Int{1}, or Array(Int, 3){1,2,3}.
    bool parser::parse_type_instantiation(unsigned, const token &tok, const char *) throw() {

        ast *decl_(pop(stack));

        if(!decl_->is_instance<type_declaration>()) {
            delete decl_;
            return report_simple(io::e_not_a_type_for_instance, tok);
        }

        bool consume_comma(false);
        type_declaration *type_of_inst(decl_->reinterpret<type_declaration>());
        type_instance_expression *inst(new type_instance_expression(type_of_inst));

        inst->location = type_of_inst->location;
        inst->location.extend(tok);

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
            inst->location.extend(expr_begin);
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
            expression *sub_expr(val->reinterpret<expression>());
            inst->location.extend(sub_expr->location);
            inst->values.push_back(sub_expr);
        }

        stack.push_back(inst);

        // try to extend location to closing brace, assuming it's a closing
        // brace
        stream.accept(expr_begin);
        stream.undo();
        inst->location.extend(expr_begin);

        return consume(T_CLOSE_BRACE);
    }

    /// parse an array access, e.g. "hello"[0] or [...][0].
    bool parser::parse_array_access(unsigned, const token &tok, const char *) throw() {
        ast *left_(pop(stack));
        ast *right_(nullptr);

        if(!left_->is_instance<expression>()) {
            delete left_;
            return report_simple(io::e_array_access_on_non_expr, tok);
        }

        expression *left(left_->reinterpret<expression>());

        if(!parse(expression_parsers, 0)) {
            delete left;
            return false;
        }

        // record location of closing bracket
        token close_bracket;
        stream.accept(close_bracket);
        stream.undo();

        if(!consume(T_CLOSE_BRACKET)) {
            delete left;
            return false;
        }

        right_ = pop(stack);

        if(!right_->is_instance<expression>()) {
            delete left_;
            delete right_;
            return report_simple(io::e_array_access_on_non_expr, tok);
        }

        expression *sub_expr(right_->reinterpret<expression>());
        array_access_expression *arr_expr(new array_access_expression(left, sub_expr));

        // try to extend the location
        arr_expr->location = left->location;
        arr_expr->location.extend(tok);
        arr_expr->location.extend(sub_expr->location);
        arr_expr->location.extend(close_bracket);

        stack.push_back(arr_expr);
        return true;
    }

    /// parse an infix expression. This could have different meanings based
    /// on the lhs. E.g. if the lhs is a type, then this could be product/sum/
    /// arrow type, otherwise it could be a binary expression or something
    /// more than a binary expression.
    bool parser::parse_infix(unsigned prec, const token &tok, const char *data) throw() {

        // looks like we're parsing an inline type declaration
        if(stack.back()->is_instance<type_declaration>()) {
            switch(tok.type()) {
            case T_ARROW: return parse_arrow_type_operator(prec, tok, data);
            case T_PLUS: return parse_infix_type_operator<sum_type_declaration>(prec, tok, data);
            case T_ASTERISK: return parse_infix_type_operator<product_type_declaration>(prec, tok, data);
            default:
                return report_simple(io::e_bad_type_operator, tok);
            }
        }

        // arrow in expression context
        if(T_ARROW == tok.type()) {
            return report_simple(io::e_arrow_in_expr_context, tok);
        }

        token right_head;
        stream.accept(right_head);
        stream.undo();

        // parse the right thing
        if(!parse(expression_parsers, prec)) {
            ctx.report_here(right_head, io::e_bin_rhs_not_expr, tok.name());
            return false;
        }

        expression *right(pop(stack)->reinterpret<expression>());
        expression *left(pop(stack)->reinterpret<expression>());

        infix_expression *expr(new infix_expression(left, right));
        expr->location = tok;
        expr->location.extend(left->location);
        expr->location.extend(right->location);
        stack.push_back(expr);
        return true;
    }

    bool parser::parse_prefix(const token &tok, const char *) throw() {

        expression *expr(nullptr);

        if(!parse(expression_parsers, 0)) {
            return false;
        }

        expr = pop(stack)->reinterpret<expression>();

        assert(nullptr != expr);

        switch(tok.type()) {

        // logical negation
        case T_FORWARD_SLASH:
            break;
        // unpacking
        case T_ASTERISK:
            break;
        // negative
        case T_MINUS:
            break;

        default:
            break;
        }
        return false;
    }

    /// parse something that looks like an array literal. This might actually
    /// end up parsing a inline type declaration if the first thing in the
    /// array literal is a type declararion, making this an array type
    /// declaration.
    bool parser::parse_array_literal(const token &open_bracket, const char *) throw() {

        // empty array
        /*if(stream.check(T_CLOSE_BRACKET)) {
            // TODO
            stream.accept();
            stack.push_back(new array_expr);
            return true;

            assert(false && "Array literals?");
        }*/

        if(!parse(expression_parsers, 0)) {
            return false;
        }

        ast *first(pop(stack));

        // make the array type declaration, and try to make its location extend
        // for the entire declaration
        if(first->is_instance<type_declaration>()) {
            array_type_declaration *decl(new array_type_declaration(first->reinterpret<type_declaration>()));
            decl->location = open_bracket;
            decl->location.extend(decl->inner_type->location);
            stack.push_back(decl);
            token next;
            stream.accept(next);
            stream.undo();
            if(!consume(T_CLOSE_BRACKET)) {
                return false;
            }
            decl->location.extend(next);

            return true;

        } else if(first->is_instance<expression>()) {
            /*array_expr *arr(new array_expr);
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
            */
            assert(false); // TODO
            return false;
        } else {
            return false;
        }
    }

    bool parser::parse_record_literal(const token &, const char *) throw() {
        return false;
    }

    /// parse some sort of literal (e.g. integer, string, float, variable)
    bool parser::parse_literal(const token &tok, const char *data) throw() {
        expression *ee(nullptr);
        switch(tok.type()) {
        case T_INTEGER_LITERAL:
            ee = new integer_literal_expression(cstring::copy(data));
            break;
        case T_STRING_LITERAL:
            ee = new string_literal_expression(cstring::copy(data), cstring::byte_length(data));
            break;
        case T_RATIONAL_LITERAL:
            ee = new rational_literal_expression(cstring::copy(data));
            break;
        case T_NAME:
            ee = new name_expression(stab[data]);
            break;
        default:
            return false;
        }

        // record the location of the expression
        ee->location = tok;
        stack.push_back(ee);

        return true;
    }

    /// concatenate adjacent string literals
    bool parser::parse_string_concat(unsigned, const token &, const char *data) throw() {
        string_literal_expression *prev_str(stack.back()->reinterpret<string_literal_expression>());
        if(nullptr == prev_str) {
            return false;
        }

        const size_t str_len(cstring::byte_length(data));
        char *new_literal(new char[prev_str->byte_length + str_len + 1U]);

        memcpy(new_literal, prev_str->data, prev_str->byte_length);
        memcpy(&(new_literal[prev_str->byte_length]), data, str_len + 1U);

        cstring::free(prev_str->data);
        prev_str->data = new_literal;
        prev_str->byte_length += str_len;

        return true;
    }

    bool parser::parse_type_name(const token &tok, const char *name) throw() {
        named_type_declaration *decl(new named_type_declaration(stab[name]));
        decl->location = tok;
        stack.push_back(decl);
        return true;
    }

    bool parser::parse_type_type_declaration(const token &tok, const char *) throw() {
        type_type_declaration *decl(new type_type_declaration);
        printf("pushed Type %p\n", reinterpret_cast<void *>(decl));
        decl->location = tok;
        stack.push_back(decl);
        return true;
    }
    bool parser::parse_unit_type_declaration(const token &tok, const char *) throw() {
        unit_type_declaration *decl(new unit_type_declaration);
        decl->location = tok;
        stack.push_back(decl);
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
    bool parser::parse_type_group(const token &open_paren, const char *) throw() {
        if(!parse(type_parsers, 0)) {
            return false;
        }

        type_declaration *decl(stack.back()->reinterpret<type_declaration>());
        token decl_loc(decl->location);
        decl->location = open_paren;
        decl->location.extend(decl_loc);
        stream.accept(decl_loc);
        stream.undo();

        if(!consume(T_CLOSE_PAREN)) {
            return false;
        }

        decl->location.extend(decl_loc);

        // so that we don't extend something like (A -> B) -> C
        // into A -> B -> C
        binary_type_declaration *bin_decl(decl->reinterpret<binary_type_declaration>());
        if(nullptr != bin_decl) {
            bin_decl->is_wrapped = true;
        }

        std::ostringstream ss;
        decl->print(ss, stab);
        //printf("parsed group %s\n", ss.str().c_str());

        return true;
    }

    bool parser::parse_ref_type(const token &, const char *) throw() {
        return true;
    }
    bool parser::parse_array_type(const token &, const char *) throw() {
        return true;
    }

    bool parser::parse_fail_p(const token &, const char *) throw() {
        return false;
    }

    bool parser::parse_fail_s(unsigned, const token &, const char *) throw() {
        return false;
    }

    /// go get a comma-separated list of names, e.g. variable names or type
    /// names
    bool parser::parse_name_list(token_type expected, name_list &names) throw() {
        token prev_token, got;
        const char *name_buff;

        for(bool check_comma(false); ; check_comma = true) {

            // make sure names are comma-separated
            if(check_comma) {
                if(stream.check(T_ASSIGN) || stream.check(T_DECLARE)) {
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

                // don't allow us to re-assign a type name
                stream.accept(got);
                if(T_TYPE_UNIT == got.type()
                || T_TYPE_TYPE == got.type()) {
                    ctx.report_here(got, io::e_rebind_reserved_type, got.name());
                } else {
                    report_unexpected_follow_symbol(prev_token, got, expected);
                }

                return false;
            }

            // collect the name
            stream.accept(prev_token, name_buff);
            names.push_back(std::make_pair(prev_token, stab[name_buff]));
        }

        return true;
    }

    /// parse the argument names list to a function, record, or union.
    bool parser::parse_func_args(
        arrow_type_declaration *template_arg_types,
        arrow_type_declaration *arg_types,
        name_list *template_arg_names,
        name_list *arg_names,
        bool is_func
    ) throw() {

        const char *name_buff;
        token got;
        std::ostringstream ss;

        bool can_have_semicolon(is_func);

        if(nullptr != template_arg_types) {

            const size_t num(template_arg_types->types.size() - (!is_func ? 1U : 0U));
            bool expect_comma(false);

            //printf("tpl num=%lu\n", num);

            for(size_t i(0); i < num; ++i, expect_comma = true) {
                type_declaration *part(template_arg_types->types[i]);

                if(expect_comma) {
                    if(stream.check(T_SEMICOLON)) {
                        break;
                    }

                    if(!consume(T_COMMA)) {
                        return false;
                    }
                }

                stream.accept(got, name_buff);

                // we should expect a type name
                if(part->is_instance<type_type_declaration>()) {

                    // don't allow us to re-assign a type name
                    if(T_TYPE_NAME != got.type()) {

                        if(T_TYPE_UNIT == got.type()
                        || T_TYPE_TYPE == got.type()) {
                            ctx.report_here(got, io::e_rebind_reserved_type, got.name());
                        } else {
                            ctx.report_here(got, io::e_missing_arg_for_type, token::name(T_TYPE_NAME), "Type", got.name());
                        }

                        ctx.report_here(part->location, io::n_tpl_arg_need_type_name_source);

                        return false;
                    }

                // we should expect a variable name
                } else {
                    if(T_NAME != got.type()) {
                        part->print(ss, stab);
                        ctx.report_here(got, io::e_missing_arg_for_type, token::name(T_NAME), ss.str().c_str(), got.name());
                        ctx.report_here(part->location, io::n_tpl_arg_need_type_name_source);
                        return false;
                    }
                }

                template_arg_names->push_back(std::make_pair(
                    got,
                    stab[name_buff]
                ));
            }

            // must we get a semicolon?
            if(nullptr != arg_types && 1U < arg_types->types.size()) {
                if(!consume(T_SEMICOLON)) {
                    return false;
                }
                can_have_semicolon = false;
            }
        }

        // optionally get a semicolon
        if(can_have_semicolon) {
            if(stream.check(T_SEMICOLON)) {
                stream.accept();
            }
        }

        if(nullptr != arg_types) {

            const size_t num(arg_types->types.size() - (is_func ? 1U : 0U));
            bool expect_comma(false);

            //printf("func num=%lu\n", num);

            for(size_t i(0); i < num; ++i, expect_comma = true) {

                if(expect_comma && !consume(T_COMMA)) {
                    return false;
                }

                type_declaration *part(arg_types->types[i]);
                stream.accept(got, name_buff);

                if(T_NAME != got.type()) {
                    part->print(ss, stab);
                    ctx.report_here(got, io::e_missing_arg_for_type, token::name(T_NAME), ss.str().c_str());
                    ctx.report_here(part->location, io::n_func_arg_need_type_name_source);
                    return false;
                }

                arg_names->push_back(std::make_pair(
                    got,
                    stab[name_buff]
                ));
            }
        }

        return true;
    }

    /// parse a function declaration. This could be a type generating function,
    /// i.e. a template type, a function template, or a normal function.
    bool parser::parse_function(function_definition *def, bool is_func) throw() {

        if(!(consume(T_FUNCTION) && consume(T_OPEN_PAREN))) {
            return false;
        }

        if(!parse_func_args(
            def->template_arg_types,
            def->arg_types,
            &(def->template_arg_names),
            &(def->arg_names),
            is_func
        )) {
            return false;
        }

        if(!(consume(T_CLOSE_PAREN) && consume(T_OPEN_BRACE))) {
            return false;
        }

        // parse statements in this function
        if(!parse(def->statements, T_CLOSE_BRACE, def)) {
            return false;
        }

        return consume(T_CLOSE_BRACE);
    }

    /// parse the function type declarations
    bool parser::parse_func_decl_type(
        arrow_type_declaration **tpl_types,
        arrow_type_declaration **arg_types
    ) throw() {
        token next;
        stream.accept(next);
        stream.undo();
        const bool is_func(nullptr != arg_types);

        // go get the (first) set of types
        if(!parse(type_parsers, 0)) {
            ctx.report_here(next,
                (nullptr != arg_types ? io::e_func_decl_bad_arrow : io::e_type_declaration_bad_arrow)
            );
            return false;
        }

        type_declaration *first_type(pop(stack)->reinterpret<type_declaration>());
        type_declaration *second_type(first_type);
        arrow_type_declaration *tpl_types_(nullptr);

        // parsing a function and came across the separator for template
        // args and normal args
        if(is_func && stream.check(T_DOUBLE_ARROW)) {

            if(!consume(T_DOUBLE_ARROW)) {
                delete first_type;
                return false;
            }

            // add in the template types, given that we know we've gotten
            // argument types
            tpl_types_ = first_type->reinterpret<arrow_type_declaration>();
            if(nullptr == tpl_types_
            || tpl_types_->is_wrapped) {
                //printf("re-wrapping template types\n");

                tpl_types_ = new arrow_type_declaration;
                tpl_types_->types.push_back(first_type);
            }
            *tpl_types = tpl_types_;

            stream.accept(next);
            stream.undo();

            // couldn't parse the function type arguments (following the
            // template type arguments)
            if(!parse(type_parsers, 0)) {
                ctx.report_here(next, io::e_func_no_arg_type);
                delete tpl_types_;
                return false;
            }

            second_type = pop(stack)->reinterpret<type_declaration>();

        // not allowed the => for type templates
        } else if(!is_func && stream.check(T_DOUBLE_ARROW)) {
            delete first_type;
            stream.accept(next);
            ctx.report_here(next, io::e_double_arrow_tpl_type);
            return false;
        }

        // add in the argument types. for a type declaration, these are
        // actually template argument types.
        arrow_type_declaration *arg_types_(second_type->reinterpret<arrow_type_declaration>());
        if(nullptr == arg_types_
        || arg_types_->is_wrapped) {
            printf("re-wrapping args %p %lu '%s'\n", reinterpret_cast<void *>(second_type), second_type->type_id(), second_type->class_name());
            arg_types_ = new arrow_type_declaration;
            arg_types_->types.push_back(second_type);
        }

        // function (with possible template arguments)
        if(is_func) {
            *arg_types = arg_types_;

            // only one return; so make it go from Unit to the return type,
            // i.e. takes in nothing
            if(1U == arg_types_->types.size()) {
                //printf("adding in unit\n");
                arg_types_->types.push_back(arg_types_->types[0]);
                arg_types_->types[0] = new unit_type_declaration;
            }

        // template type
        } else {
            *tpl_types = arg_types_;
        }

        // by this point:
        // arg_types_ contains argument types
        // tpl_types_ contains template types
        // if null then there are none for either
        assert(!(is_func && nullptr != tpl_types_ && nullptr == arg_types_));
        assert(!is_func || (is_func && nullptr != arg_types_));

        /*if(nullptr != tpl_types_) {
            printf("tpl len = %lu\n", tpl_types_->types.size());
        }
        if(nullptr != arg_types_) {
            printf("arg len = %lu\n", arg_types_->types.size());
        }*/

        // validate the argument types
        if(is_func && nullptr != (*arg_types)) {
            type_declaration *return_type((*arg_types)->types.back());

            // a function can't return a type/template; only template types can
            // return types
            if((*arg_types)->returns_type()) {
                ctx.report_here(return_type->location, io::e_func_cant_return_type);
                return false;
            }

            type_declaration *first_unit(nullptr);
            type_declaration *first_non_unit(nullptr);
            std::ostringstream ss;

            // make sure the function arguments (excluding return type) has at
            // most one Unit type, and if it does, then it can only take in a
            // unit type
            const size_t len((*arg_types)->types.size() - 1U);
            for(size_t i(0); i < len; ++i) {

                type_declaration *curr((*arg_types)->types[i]);

                // Unit in arg list
                if(curr->is_instance<unit_type_declaration>()) {

                    // two Unit types!
                    if(nullptr != first_unit) {
                        ctx.report_here(first_unit->location, io::e_func_two_units);
                        return false;
                    }

                    first_unit = curr;

                // Type or type-returning thing in arg list; not a perfect check
                // i.e. only works in some cases.
                } else if(curr->is_instance<type_type_declaration>()
                       || curr->returns_type()) {

                    ctx.report_here(curr->location, io::e_func_cant_take_type);
                    return false;

                // non Unit/Type thing
                } else {
                    first_non_unit = curr;
                }

                // unit and non-unit arg types
                if(nullptr != first_unit && nullptr != first_non_unit) {
                    first_non_unit->print(ss, stab);
                    ctx.report_here(first_non_unit->location, io::e_func_has_unit, ss.str().c_str());
                    return false;
                }
            }
        }

        // validate the template types
        if(nullptr != (*tpl_types)) {

            // make sure our template takes in at least one argument
            if(1U == (*tpl_types)->types.size()
            && (nullptr == arg_types || nullptr == (*arg_types))) {
                ctx.report_here((*tpl_types)->types[0]->location, io::e_template_needs_args);
                return false;
            }

            //type_declaration *return_type((*tpl_types)->types.back());

            // make sure we return either a type or another template if this
            // is purely a template
            /*if(!((*tpl_types)->returns_type())
            && (nullptr == arg_types || nullptr == (*arg_types))) {
                ctx.report_here(return_type->location, io::e_template_must_return_type);

                return false;
            }*/

            // template type decl has unit declaration
            const size_t len((*tpl_types)->types.size());
            for(size_t i(0); i < len; ++i) {
                if(!(*tpl_types)->types[i]->is_instance<unit_type_declaration>()) {
                    continue;
                }

                ctx.report_here((*tpl_types)->types[i]->location, io::e_tpl_decl_has_unit);
                return false;
            }

            // not a function, or has no arguments; we need to have more arguments
            // because this type template only specifies one argument, i.e. what it
            if(!is_func && 1U == (*tpl_types)->types.size()) {
                ctx.report_here((*tpl_types)->types[0]->location, io::e_type_tpl_need_args);
                return false;
            }
        }

        return true;
    }

    /// parse a variable definition
    //          let one := 1
    //          let two = 2
    // or a function definition
    //          let foo :: ...
    // or a type declaration
    //          let Foo
    bool parser::parse_let(void) throw() {
        assert(stream.check(T_LET));
        token prev;
        token curr;
        name_list names;
        bool check_sep(false);
        unsigned last_seen(0);

        stream.accept(prev);

        // assume that top of stack is a statement list
        conjunctive_statement *stmts(stack.back()->reinterpret<conjunctive_statement>());

        assert(nullptr != stmts);

        // make sure we can get what we need
        if(!stream.check(T_NAME) && !stream.check(T_TYPE_NAME)) {
            stream.accept(curr);

            // possible common error; the programming is attempting to
            // re-assign the Type type.
            if(T_TYPE_TYPE == curr.type() || T_TYPE_UNIT == curr.type()) {
                ctx.report_here(curr, io::e_rebind_reserved_type, curr.name());
                return false;
            } else {
                return report_unexpected_follow_symbol_2(prev, curr, T_NAME, T_TYPE_NAME);
            }

        // variable/func declaration
        } else if(stream.check(T_NAME)) {

            if(!parse_name_list(T_NAME, names)) {
                return false;
            }

            // see if this is a function declaration, e.g.
            // let foo :: Bar -> Baz := function(...) { ... }
            if(stream.check(T_DECLARE)) {

                // more than one names are part of this function declaration
                if(1U != names.size()) {
                    ctx.report_here(prev, io::e_func_decl_multiple_names, names.size());
                    return false;
                }

                consume(T_DECLARE);

                function_definition *def(new function_definition);
                if(!parse_func_decl_type(&(def->template_arg_types), &(def->arg_types))) {
                    delete def;
                    return false;
                }

                // now we want to parse the function itself
                if(!consume(T_ASSIGN)) {
                    delete def;
                    return false;
                }

                def->statements = new conjunctive_statement;
                def->statements->parent_scope = stmts;

                if(!parse_function(def, true)) {
                    delete def;
                    return false;
                }

                stmts->statements.push_back(def);
                return true;

            } else if(!consume(T_ASSIGN)) {
                return false;
            }

            // for each variable name
            for(size_t i(0); i < names.size(); ++i) {
                if(check_sep && !consume(T_COMMA)) {
                    break;
                }

                if(!parse(expression_parsers, 0)) {
                    break;
                }

                variable_definition *var_def(new variable_definition(
                    names[i].first,
                    names[i].second,
                    stack.back()->reinterpret<expression>()
                ));

                printf("making var def %p\n", reinterpret_cast<void *>(var_def));

                stmts->statements.push_back(var_def);
                stack.pop_back();

                check_sep = true;
                ++last_seen;
            }

            if(last_seen < names.size()) {
                ctx.report_here(names[last_seen].first, io::e_missing_expr_in_let, stab[names[last_seen].second]);
                return false;
            }

        // type declaration
        } else {
            if(!parse_name_list(T_TYPE_NAME, names)) {
                return false;
            }

            // see if this is a type template declaration, e.g.
            // let Fizz :: Bar -> Type := function(...) { ... }
            if(stream.check(T_DECLARE)) {

                // more than one names are part of this function declaration
                if(1U != names.size()) {
                    ctx.report_here(prev, io::e_tpl_decl_multiple_names, names.size());
                    return false;
                }

                consume(T_DECLARE);

                arrow_type_declaration *template_arg_types(nullptr);

                // get the info
                if(!parse_func_decl_type(&template_arg_types, nullptr)) {
                    if(nullptr != template_arg_types) {
                        delete template_arg_types;
                    }
                    return false;
                }

                if(!consume(T_ASSIGN)) {
                    delete template_arg_types;
                    return false;
                }

                ast *decl(nullptr);

                // as a function
                if(stream.check(T_FUNCTION)) {

                    function_definition *def(new function_definition);
                    def->template_arg_types = template_arg_types;
                    def->statements = new conjunctive_statement;
                    def->statements->parent_scope = stmts;

                    if(!parse_function(def, false)) {
                        delete def;
                        return false;
                    }
                    decl = def;

                // as a record
                } else if(stream.check(T_RECORD)) {

                // as a union
                } else if(stream.check(T_UNION)) {

                }

                stmts->statements.push_back(new structured_type_definition(
                    names[0].first,
                    names[0].second,
                    decl,
                    template_arg_types
                ));

                /*
                function_definition *def(new function_definition);
                if(!parse_func_decl_type(true, &(def->template_arg_types), &(def->arg_types))) {
                    delete def;
                    return false;
                }

                // now we want to parse the function itself


                def->statements = new conjunctive_statement;
                if(!parse_function(def, true)) {
                    delete def;
                    return false;
                }

                stmts->statements.push_back(def);
                return true;
                */
                return true;

            } else if(!consume(T_ASSIGN)) {
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

                stmts->statements.push_back(new type_definition(
                    names[i].first,
                    names[i].second,
                    stack.back()->reinterpret<type_declaration>()
                ));
                stack.pop_back();

                check_sep = true;
                ++last_seen;
            }

            if(last_seen < names.size()) {
                ctx.report_here(names[last_seen].first, io::e_missing_type_declaration_in_let, stab[names[last_seen].second]);
                return false;
            }
        }

        return true;
    }

    /// consume a single token, and report an error if the token cannot be
    /// consumed
    bool parser::consume(token_type expected) throw() {
        repl::wait();
        if(!stream.check(expected)) {
            token got;
            stream.accept(got);

            ctx.report(io::e_expected_different_symbol, got.name(), token::name(expected));
            ctx.report(io::c_file_line_col, ctx.name(), got.line(), got.column());
            ctx.report(io::c_highlight, io::highlight_line(
                ctx.file(), got.line(), got.column(), got.end_column()
            ));

            repl::accept();
            return false;
        }

        repl::accept();
        return stream.accept();
    }

    /// constructor
    parser::parser(diagnostic_context &ctx_, token_stream &stream_, symbol_table &stab_) throw()
        : ctx(ctx_)
        , stab(stab_)
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

    bool parser::parse(conjunctive_statement *stmts, token_type end, function_definition *def) throw() {
        bool last_parsed(true);
        //printf("def = %p\n", reinterpret_cast<void *>(def));
        for(; last_parsed && stream.check(); ) {

            /*while(stream.check(T_STRING_LITERAL)
               || stream.check(T_INTEGER_LITERAL)
               || stream.check(T_RATIONAL_LITERAL)) {
                stream.accept();
            }*/

            // done parsing this file
            if(stream.check(end)) {
                break;
            }

            // variable definition
            else if(stream.check(T_LET)) {
                repl::wait();
                last_parsed = parse_let();
                if(last_parsed) {
                    last_parsed = consume(T_SEMICOLON);
                }
                repl::accept();

            // possibly unexpected token; try to parse an expression
            } else if(stream.check()) {
                /*token got;
                stream.accept(got);
                stream.undo();
                p.report_simple(io::e_unexpected_symbol, got);
                last_parsed = false;
                */

                repl::wait();
                last_parsed = parse(expression_parsers, 0);
                if(last_parsed) {
                    last_parsed = consume(T_SEMICOLON);
                    expression *top_expr(pop(stack)->reinterpret<expression>());
                    stmts->statements.push_back(new expression_statement(top_expr));
                }
                repl::accept();

            // template or function
            } else if(nullptr != def && stream.check(T_RETURN)) {
                last_parsed = consume(T_RETURN);
                repl::wait();

                //printf("arg types = %p\n", reinterpret_cast<void *>(def->arg_types));

                // template type
                if(nullptr == def->arg_types) {

                    last_parsed = parse(type_parsers, 0);
                    if(last_parsed) {
                        stmts->statements.push_back(new return_type_statement(
                            stack.back()->reinterpret<type_declaration>()
                        ));
                    }

                // funcion, not returning unit type, so get an expression
                } else if(!def->arg_types->types.back()->is_instance<unit_type_declaration>()) {
                    last_parsed = parse(expression_parsers, 0);
                    if(last_parsed) {
                        stmts->statements.push_back(new return_type_statement(
                            stack.back()->reinterpret<type_declaration>()
                        ));
                    }

                // function returning Unit type but actually returning something
                } else if(!stream.check(T_SEMICOLON)) {
                    token got;
                    stream.accept(got);
                    ctx.report_here(got, io::e_func_return_expr_for_unit);
                    last_parsed = false;
                }

                if(last_parsed) {
                    last_parsed = consume(T_SEMICOLON);
                }
                repl::accept();

            // no idea :(
            } else {
                return consume(end); // will fail.
            }
        }

        return last_parsed;
    }

    /// parse a stream of tokens
    ast *parser::parse(diagnostic_context &ctx, symbol_table &stab, token_stream &stream) throw() {

        parser p(ctx, stream, stab);
        stream.ctx = ctx;

        // push the top thing onto the stack :D
        conjunctive_statement *stmts(new conjunctive_statement);
        p.stack.push_back(stmts);
        bool last_parsed(p.parse(stmts, T_EOF, nullptr));

        if(last_parsed) {
            p.consume(T_EOF);
        }

        // failed to parse, and we have some (presumably) useful error
        // messages in the queue
        if(ctx.has_message(io::message_type::error)
        || ctx.has_message(io::message_type::recoverable_error)
        || ctx.has_message(io::message_type::failed_assertion)) {
            return nullptr;
        }

        // failed to parse but no messages in the queue; try to be helpful.
        if(!last_parsed) {
            token last;
            stream.accept(last);
            ctx.report_here(last, io::e_error_parsing);
            return nullptr;
        }
        p.stack.pop_back();

        return stmts;
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
    void parser::parse_open_file(io::file<xy::io::read_tag> &ff, diagnostic_context &ctx, symbol_table &stab, ast *&ret) throw() {
        tokenizer tt;
        token_stream stream(ctx, tt, ff);
        ret = parse(ctx, stab, stream);
    }

    /// parse a file given a file name
    ast *parser::parse_file(diagnostic_context &ctx, symbol_table &stab, const char *file_name) throw() {
        ast *ret(nullptr);

        if(!io::read::open_file(file_name, parser::parse_open_file, ctx, stab, ret)) {
            ctx.report(io::e_open_file, file_name);
        }
        return ret;
    }

    /// -----------------------------------------------------------------------
    /// -----------------------------------------------------------------------
    /// -----------------------------------------------------------------------

    ast *parser::parse_buffer(diagnostic_context &ctx, symbol_table &stab, const char * const buffer) throw() {
        tokenizer tt;
        support::cstring_reader rr(buffer);
        token_stream stream(ctx, tt, rr);
        return parse(ctx, stab, stream);
    }

    ast *parser::parse_reader(diagnostic_context &ctx, symbol_table &stab, support::byte_reader &reader) throw() {
        tokenizer tt;
        token_stream stream(ctx, tt, reader);
        return parse(ctx, stab, stream);
    }
}

