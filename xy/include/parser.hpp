/*
 * parser.hpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef PARSER_HPP_
#define PARSER_HPP_

#include "xy/include/token_stream.hpp"
#include "xy/include/diagnostic_context.hpp"

namespace xy {

    bool parse_let(diagnostic_context &ctx, token_stream &stream) throw();

    bool parse_defun(diagnostic_context &ctx, token_stream &stream) throw();

    bool parse_deftype(diagnostic_context &ctx, token_stream &stream) throw();

    bool parse_type_assign(diagnostic_context &ctx, token_stream &stream) throw();

    bool parse_func_assign(diagnostic_context &ctx, token_stream &stream) throw();

    bool parse(diagnostic_context &ctx, token_stream &stream) throw();
    bool parse_file(diagnostic_context &ctx, const char * const file_name) throw();
    bool parse_buffer(diagnostic_context &ctx, const char * const buffer) throw();
}


#endif /* PARSER_HPP_ */
