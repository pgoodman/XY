/*
 * decode.hpp
 *
 *  Created on: Jun 14, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef XY_DECODE_HPP_
#define XY_DECODE_HPP_

#include <stdint.h>

namespace xy { namespace utf8 {

    class codepoint;

    /// represents a UTF-8 decoder state machine that takes as input a single
    /// byte and performs transitions base on the byte.
    class decoder {
    private:
        uint8_t state;
        uint8_t seen;
        uint8_t chars[5];
        bool found_error_;

    public:
        decoder(void) throw();
        bool next_state(uint8_t, codepoint &) throw();
        bool is_in_use(void) const throw();
        bool found_error(void) const throw();
        void reset(void) throw();

        static size_t length(const char *) throw();
    };

}}

#endif /* XY_DECODE_HPP_ */
