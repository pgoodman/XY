/*
 * token_stream.hpp
 *
 *  Created on: Aug 7, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_TOKEN_STREAM_HPP_
#define XY_TOKEN_STREAM_HPP_

#include "xy/include/token.hpp"

namespace xy {

    class token_stream {
    public:

        virtual ~token_stream(void) throw();

        virtual bool check(void) throw() = 0;
        virtual bool check(token_type) throw() = 0;
        virtual bool accept(void) throw() = 0;
        virtual bool accept(token &, const char *&) throw() = 0;
        virtual void undo(void) throw() = 0;
    };
}

#endif /* XY_TOKEN_STREAM_HPP_ */
