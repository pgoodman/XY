/*
 * token_stream.hpp
 *
 *  Created on: Aug 7, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_TOKEN_STREAM_HPP_
#define XY_TOKEN_STREAM_HPP_

#include "xy/include/diagnostic_context.hpp"
#include "xy/include/token.hpp"
#include "xy/include/tokenizer.hpp"

#include "xy/include/support/byte_reader.hpp"

namespace xy {

    class parser;

    class token_stream {
    private:

        friend class parser;

        enum {
            NUM_BACKUP = 2,
            NUM_TOKENS = NUM_BACKUP + 1
        };

        diagnostic_context &ctx;
        tokenizer &tt;
        support::byte_reader &f;

        char scratch[NUM_TOKENS][token::MAX_LENGTH];
        token tokens[NUM_TOKENS];
        size_t num_seen;
        unsigned backed_up;
        unsigned curr;

    public:

        token_stream(
            diagnostic_context &ctx_,
            tokenizer &tt_,
            support::byte_reader &f_
        ) throw();

        ~token_stream(void) throw();

        bool check(void) throw();
        bool check(token_type) throw();
        bool accept(void) throw();
        bool accept(token &) throw();
        bool accept(token &, const char *&) throw();
        void undo(void) throw();
    };
}

#endif /* XY_TOKEN_STREAM_HPP_ */
