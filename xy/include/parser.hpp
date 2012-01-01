/*
 * parser.hpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <vector>
#include <string>
#include <utility>

#include "xy/include/token_stream.hpp"
#include "xy/include/diagnostic_context.hpp"
#include "xy/include/ast.hpp"

#include "xy/include/symbol_table.hpp"

#include "xy/include/io/file.hpp"
#include "xy/include/support/byte_reader.hpp"

namespace xy {

    ast *pop(std::vector<ast *> &stack) throw();

    // add something to the front of a vector
    template <typename T>
    void unshift(std::vector<T> &vec, T elm) throw() {
        std::vector<T> new_vec;
        new_vec.reserve(vec.size() + 1U);
        new_vec.push_back(elm);
        new_vec.insert(new_vec.end(), vec.begin(), vec.end());
        vec.clear();
        vec.swap(new_vec);
    }

    // concatenate two vectors
    template <typename T>
    void extend(std::vector<T> &to, std::vector<T> &from) throw() {
        to.insert(to.end(), from.begin(), from.end());
    }

    class parser {
    private:

        diagnostic_context &ctx;
        symbol_table &stab;
        token_stream &stream;
        std::vector<ast *> stack;

        //bool accept_expected_newline(void) throw();

        bool report_simple(io::message_id, const token &got) throw();

        bool report_unexpected_follow_symbol(
            const token &first, const token &got, token_type expected
        ) throw();

        bool report_unexpected_follow_symbol_2(
            const token &first, const token &got, token_type expected1, token_type expected2
        ) throw();

        // precedence table
        struct precedence_parser {
        public:
            unsigned precedence;
            token_type pivot;
            bool (parser::*prefix)(const token &, const char *);
            bool (parser::*postfix)(unsigned, const token &, const char *);
        };

        static const precedence_parser expression_parsers[];
        static const precedence_parser type_parsers[];

        typedef const parser::precedence_parser *(parser_category)(const token &);

        static const precedence_parser *get_precedence_parser(const parser::precedence_parser *, const token &) throw();

        bool consume(token_type) throw();

        /// precendence parsers
        bool parse(const parser::precedence_parser *, unsigned prec) throw();
        bool parse_fail_p(const token &, const char *) throw();
        bool parse_fail_s(unsigned, const token &, const char *) throw();

        // expression parsers
        bool parse_paren_expression(const token &, const char *) throw();
        bool parse_application(unsigned, const token &, const char *) throw();
        bool parse_type_instantiation(unsigned, const token &, const char *) throw();
        bool parse_array_access(unsigned, const token &, const char *) throw();
        bool parse_array_literal(const token &, const char *) throw();
        bool parse_record_literal(const token &, const char *) throw();
        bool parse_infix(unsigned, const token &, const char *) throw();
        bool parse_prefix(const token &, const char *) throw();
        bool parse_literal(const token &, const char *) throw();
        bool parse_string_concat(unsigned, const token &, const char *) throw();

        // type parsers
        bool parse_type_name(const token &, const char *) throw();
        bool parse_type_type_decl(const token &, const char *) throw();
        bool parse_unit_type_decl(const token &, const char *) throw();
        bool parse_union(const token &, const char *) throw();
        bool parse_record(const token &, const char *) throw();
        bool parse_type_function(const token &, const char *) throw();
        bool parse_type_group(const token &, const char *) throw();
        bool parse_type_operands(unsigned, const token &, type_decl **, type_decl **) throw();

        template <typename param_type>
        bool parse_params(io::message_id, token_type, std::vector<param_type *> &) throw();

        template <typename>
        bool parse_infix_type_operator(unsigned, const token &, const char *) throw();

        bool parse_arrow_type_operator(unsigned, const token &, const char *) throw();

        // function parsers
        bool parse_func_args(
            arrow_type_decl *template_arg_types,
            arrow_type_decl *arg_types,
            name_list *template_arg_names,
            name_list *arg_names,
            bool is_func
        ) throw();
        bool parse_function(func_def *, bool) throw();

        bool parse_ref_type(const token &, const char *) throw();
        bool parse_array_type(const token &, const char *) throw();

        /// parse a let statement. the let statement is used to bind the values
        /// of expressions to variables, or type declarations to type names.
        bool parse_func_decl_type(arrow_type_decl **, arrow_type_decl**) throw();
        bool parse_name_list(token_type, name_list &) throw();
        bool parse_let(void) throw();

        parser(diagnostic_context &, token_stream &, symbol_table &) throw();
        ~parser(void) throw();

        static void parse_open_file(io::file<xy::io::read_tag> &ff, diagnostic_context &, symbol_table &, ast *&) throw();

        bool parse(statement_list *, token_type, func_def *) throw();

    public:

        static ast *parse(diagnostic_context &ctx, symbol_table &, token_stream &stream_) throw();
        static ast *parse_file(diagnostic_context &ctx, symbol_table &, const char * const file_name) throw();
        static ast *parse_buffer(diagnostic_context &ctx, symbol_table &, const char * const buffer) throw();
        static ast *parse_reader(diagnostic_context &ctx, symbol_table &, support::byte_reader &reader) throw();
    };

    template <typename param_type>
    bool parser::parse_params(io::message_id bad_param_error, token_type end_type, std::vector<param_type *> &params) throw() {
        bool consume_comma(false);
        token expr_begin;
        ast *val(nullptr);

        for(; !stream.check(T_CLOSE_PAREN) && !stream.check(end_type); consume_comma = true) {

            if(consume_comma && !consume(T_COMMA)) {
                return false;
            }

            // just so that we know the position of where the expression was
            // meant to begin
            stream.accept(expr_begin);
            stream.undo();

            if(!parse(expression_parsers, 0)) {
                return false;
            }

            val = pop(stack);

            if(!val->is_instance<param_type>()) {
                delete val;
                ctx.report_here(expr_begin, bad_param_error);
                return false;
            }

            params.push_back(val->reinterpret<param_type>());
        }

        return true;
    }

    /// parse a left associative infix type operator
    template <typename type_operator>
    bool parser::parse_infix_type_operator(unsigned prec, const token &op, const char *) throw() {

        type_decl *left(nullptr);
        type_decl *right(nullptr);

        if(!parse_type_operands(prec, op, &left, &right)) {
            return false;
        }

        type_operator *op_decl(left->reinterpret<type_operator>());

        if(nullptr == op_decl || op_decl->is_wrapped) {
            op_decl = new type_operator;
            op_decl->types.push_back(left);
        }

        type_operator *right_op(right->reinterpret<type_operator>());

        // non-op type, or op type that's wrapped
        if(nullptr == right_op || right_op->is_wrapped) {
            op_decl->types.push_back(right);

        // non-wrapped op
        } else {
            extend(op_decl->types, right_op->types);
            right_op->types.clear();
            delete right_op;
        }

        token location(left->location);
        location.extend(op);
        location.extend(right->location);
        op_decl->location = location;
        stack.push_back(op_decl);

        return true;
    }
}


#endif /* PARSER_HPP_ */
