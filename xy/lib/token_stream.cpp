/*
 * token_stream.cpp
 *
 *  Created on: Aug 7, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include <cstring>

#include "xy/include/token_stream.hpp"
#include "xy/include/array.hpp"

namespace xy {

    token_stream::token_stream(
        diagnostic_context &ctx_,
        tokenizer &tt_,
        support::byte_reader &f_
    ) throw()
        : ctx(ctx_)
        , tt(tt_)
        , f(f_)
        , num_seen(0)
        , backed_up(0)
        , curr(0)
    {
        memset(scratch[0], 0, array::size(scratch[0]));
        memset(scratch[1], 0, array::size(scratch[1]));
    }

    token_stream::~token_stream(void) throw() { }

    /// check if we have a token
    bool token_stream::check(void) throw() {
        if(0 == backed_up) {
            if(!tt.get_token(f, ctx, tokens[curr], scratch[curr])) {
                return false;
            }
            ++backed_up;
            ++num_seen;
        }
        return true;
    }

    /// check if we have a token and if the current token has a specific
    /// type
    bool token_stream::check(token_type type_) throw() {
        if(check()) {
            return type_ == tokens[curr % NUM_TOKENS].type();
        }
        return false;
    }

    /// accept and move past the current token
    bool token_stream::accept(void) throw() {
        if(check()) {
            curr = (curr + 1) % NUM_TOKENS;
            --backed_up;
            return true;
        }
        return false;
    }

    bool token_stream::accept(token &tok) throw() {
        if(check()) {
            tok = tokens[curr];
            curr = (curr + 1) % NUM_TOKENS;
            --backed_up;
            return true;
        }
        return false;
    }

    /// bind the current token, accept it, and move past
    bool token_stream::accept(token &tok, const char *&data) throw() {
        if(check()) {
            tok = tokens[curr];
            data = scratch[curr];

            curr = (curr + 1) % NUM_TOKENS;
            --backed_up;
            return true;
        }
        return false;
    }

    /// undo the acceptance of a token
    void token_stream::undo(void) throw() {
        if(num_seen > 0 && backed_up < NUM_BACKUP) {
            ++backed_up;
            curr = (curr + NUM_TOKENS - 1) % NUM_TOKENS;
        }
    }

}
