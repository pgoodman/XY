/*
 * parser.hpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef PARSER_HPP_
#define PARSER_HPP_

#include "xy/include/token.hpp"
#include "xy/include/diagnostic_context.hpp"

namespace xy {

    class parser {
    private:

    public:

        bool give_token(diagnostic_context &ctx, token &tok, const char (&token_data)[MAX_TOKEN_LENGTH]) throw();
    };

}


#endif /* PARSER_HPP_ */
