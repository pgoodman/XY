/*
 * codepoint.hpp
 *
 *  Created on: Jun 14, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef XY_CODEPOINT_HPP_
#define XY_CODEPOINT_HPP_

#include <stdint.h>

namespace xy { namespace utf8 {

    class decoder;

    /// represents a single UTF-8 codepoint. Stores the codepoints as null-
    /// terminated C strings.
    class codepoint {
        friend class decoder;
    private:

        typedef void private_tag;

        uint8_t data[5];

        void init_from_byte_array(uint8_t *);

    public:

        codepoint(void) throw();
        codepoint(const char) throw();
        codepoint(const codepoint &) throw();

        friend bool operator==(const char, const codepoint &) throw();

        bool operator==(const codepoint &) const throw();

        bool is_null(void) const throw();
        bool is_ascii(void) const throw();

        const char *to_cstring(void) const throw();

        codepoint &operator=(const char) throw();
        codepoint &operator=(const codepoint &) throw();

        size_t byte_length(void) const throw();
    };

    bool operator==(const char, const codepoint &) throw();
}}

#endif /* XY_CODEPOINT_HPP_ */
