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
    /*
    class decoder {
    public:
        typedef uint8_t (next_char_func)(bool &);
    private:
        uint8_t state;
    public:
        decoder(next_char_func *) throw();
        size_t decode_next_into(uint8_t ) const throw();
    };*/

    class decoder {
    private:
        uint8_t state;
        uint8_t seen;
        uint8_t chars[5];
    public:
        decoder(void) throw();
        bool next_state(uint8_t, codepoint &) throw();
    };

}}

#endif /* XY_DECODE_HPP_ */
