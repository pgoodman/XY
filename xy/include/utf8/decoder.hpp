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

    class decoder {
    private:
        uint8_t state;

    public:
        decoder(void) throw();
        void reset(void) throw();
        bool byte_is_valid(uint8_t) throw();
    };

}}

#endif /* XY_DECODE_HPP_ */
