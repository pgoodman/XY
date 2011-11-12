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

#include "xy/include/token_stream.hpp"
#include "xy/include/diagnostic_context.hpp"

#include "xy/include/symbol_table.hpp"

#include "xy/include/io/file.hpp"

namespace xy {

    struct parse_tree;

    class parser {
    private:

        diagnostic_context &ctx;
        symbol_table stab;
        token_stream &stream;
        std::vector<parse_tree *> scratch;

        //bool accept_expected_newline(void) throw();

        bool unexpected_follow_symbol(
            token &first, token &got, token_type expected
        ) throw();

        bool unexpected_follow_symbol_2(
            token &first, token &got, token_type expected1, token_type expected2
        ) throw();

        bool parse_name_list(
            token_type expected,
            std::vector<std::string> &names
        ) throw();

        // precedence table
        struct expression_parser {
        public:
            uint8_t precedence;
            token_type pivot;
            bool (parser::*prefix)(const token &, const char *);
            bool (parser::*postfix)(uint8_t, const token &, const char *);
        };
        static const expression_parser expression_parsers[];

        const expression_parser *get_sub_parser(const token &) throw();

        bool consume(token_type) throw();

        /// parse an arbitrary expression
        bool parse_expression(uint8_t prec=0) throw();
        bool parse_paren_expression(const token &, const char *) throw();
        bool parse_application(uint8_t, const token &, const char *) throw();
        bool parse_array_access(uint8_t, const token &, const char *) throw();
        bool parse_array_literal(const token &, const char *) throw();
        bool parse_record_literal(const token &, const char *) throw();

        bool parse_infix(uint8_t, const token &, const char *) throw();
        bool parse_prefix(const token &, const char *) throw();

        bool parse_fail_p(const token &, const char *) throw();
        bool parse_fail_s(uint8_t, const token &, const char *) throw();

        bool parse_literal(const token &, const char *) throw();

        /// parse a let statement. the let statement is used to bind the values
        /// of expressions to variables, or type declarations to type names.
        bool parse_let(void) throw();

        parser(diagnostic_context &ctx_, token_stream &stream_) throw();

        static void parse_open_file(io::file<xy::io::read_tag> &ff, diagnostic_context &, bool &) throw();

    public:

        static bool parse(diagnostic_context &ctx, token_stream &stream_) throw();
        static bool parse_file(diagnostic_context &ctx, const char * const file_name) throw();
        static bool parse_buffer(diagnostic_context &ctx, const char * const buffer) throw();
    };
}


#endif /* PARSER_HPP_ */
