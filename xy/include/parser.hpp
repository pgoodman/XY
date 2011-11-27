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

namespace xy {

    ast *pop(std::vector<ast *> &stack) throw();

    class parser {
    private:

        diagnostic_context &ctx;
        symbol_table stab;
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

        bool parse_name_list(
            token_type expected,
            std::vector<std::pair<token, support::mapped_name> > &names
        ) throw();

        // precedence table
        struct precedence_parser {
        public:
            uint8_t precedence;
            token_type pivot;
            bool (parser::*prefix)(const token &, const char *);
            bool (parser::*postfix)(uint8_t, const token &, const char *);
        };

        static const precedence_parser expression_parsers[];
        static const precedence_parser type_parsers[];

        typedef const parser::precedence_parser *(parser_category)(const token &);

        static const precedence_parser *get_precedence_parser(const parser::precedence_parser *, const token &) throw();

        bool consume(token_type) throw();

        /// precendence parsers
        bool parse(const parser::precedence_parser *, uint8_t prec) throw();
        bool parse_fail_p(const token &, const char *) throw();
        bool parse_fail_s(uint8_t, const token &, const char *) throw();

        // expression parsers
        bool parse_paren_expression(const token &, const char *) throw();
        bool parse_application(uint8_t, const token &, const char *) throw();
        bool parse_type_instantiation(uint8_t, const token &, const char *) throw();
        bool parse_array_access(uint8_t, const token &, const char *) throw();
        bool parse_array_literal(const token &, const char *) throw();
        bool parse_record_literal(const token &, const char *) throw();
        bool parse_infix(uint8_t, const token &, const char *) throw();
        bool parse_prefix(const token &, const char *) throw();
        bool parse_literal(const token &, const char *) throw();

        // type parsers
        bool parse_type_name(const token &, const char *) throw();
        bool parse_union(const token &, const char *) throw();
        bool parse_record(const token &, const char *) throw();
        bool parse_type_function(const token &, const char *) throw();
        bool parse_type_group(const token &, const char *) throw();

        template <typename param_type>
        bool parse_params(io::message_id, token_type, std::vector<param_type *> &) throw();

        template <typename>
        bool parse_infix_type_operator(uint8_t, const token &, const char *) throw();

        //bool parse_arrow_type(uint8_t, const token &, const char *) throw();
        //bool parse_sum_type(uint8_t, const token &, const char *) throw();
        //bool parse_product_type(uint8_t, const token &, const char *) throw();
        bool parse_ref_type(const token &, const char *) throw();
        bool parse_array_type(const token &, const char *) throw();

        /// parse a let statement. the let statement is used to bind the values
        /// of expressions to variables, or type declarations to type names.
        bool parse_let(void) throw();

        parser(diagnostic_context &ctx_, token_stream &stream_) throw();
        ~parser(void) throw();

        static void parse_open_file(io::file<xy::io::read_tag> &ff, diagnostic_context &, bool &) throw();

    public:

        static bool parse(diagnostic_context &ctx, token_stream &stream_) throw();
        static bool parse_file(diagnostic_context &ctx, const char * const file_name) throw();
        static bool parse_buffer(diagnostic_context &ctx, const char * const buffer) throw();
    };

    template <typename param_type>
    bool parser::parse_params(io::message_id bad_param_error, token_type end_type, std::vector<param_type *> &params) throw() {
        bool consume_comma(false);
        token expr_begin;
        ast *val(nullptr);

        for(; !stream.check(T_CLOSE_PAREN) | !stream.check(end_type); consume_comma = true) {

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

    template <typename type_operator>
    bool parser::parse_infix_type_operator(uint8_t prec, const token &op, const char *) throw() {

        ast *left_(pop(stack));

        if(!left_->is_instance<type_decl>()) {
            delete left_;
            return report_simple(io::e_type_decl_expected_before_type_op, op);
        }

        type_decl *left(left_->reinterpret<type_decl>());

        // get a "pivot" token just in case we run into an error.
        token decl_tail;
        stream.accept(decl_tail);
        stream.undo();

        // parse the right-hand operand
        if(!parse(type_parsers, prec)) {
            delete left;
            if(T_TYPE_NAME == decl_tail.type()) {
                ctx.report_here(decl_tail, io::e_incomplete_tpl_inst);
            } else {
                ctx.report_here(decl_tail, io::e_bad_suffix_type_decl, decl_tail.name());
            }
            return false;
        }

        type_decl *right(pop(stack)->reinterpret<type_decl>());
        type_operator *op_decl(left->reinterpret<type_operator>());

        if(nullptr == op_decl) {
            op_decl = new type_operator;
            op_decl->types.push_back(left);
        }

        op_decl->types.push_back(right);

        stack.push_back(op_decl);
        return true;
    }
}


#endif /* PARSER_HPP_ */
