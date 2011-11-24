/*
 * codepoint.cpp
 *
 *  Created on: Jun 14, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#include <cstring>

#include "xy/include/utf8/codepoint.hpp"

namespace xy { namespace utf8 {

    codepoint::codepoint(void) throw() { memset(data, '\0', 5UL); }
    codepoint::codepoint(const codepoint &o) throw() { memcpy(data, o.data, 5UL); }
    codepoint::codepoint(const char c) throw() {
        memset(data + 1, '\0', 4UL);
        data[0] = c;
    }

    bool operator==(const char c, const codepoint &cp) throw() {
        return reinterpret_cast<uint8_t>(c) == cp.data[0] && '\0' == cp.data[1];
    }

    bool codepoint::operator==(const codepoint &o) const throw() {
        return 0 == memcmp(data, o.data, 5UL);
    }

    codepoint &codepoint::operator=(const char c) throw() {
        data[0] = c;
        data[1] = data[2] = data[3] = data[4] = '\0';
        return *this;
    }

    codepoint &codepoint::operator=(const codepoint &cp) throw() {
        memcpy(data, cp.data, 5UL);
        return *this;
    }

    void codepoint::init_from_byte_array(uint8_t *a) {
        memcpy(data, a, 5U);
    }

    bool codepoint::is_null(void) const throw() {
        return 0 == data[0];
    }

    bool codepoint::is_ascii(void) const throw() {
        return data[0] < 128 && 0 == data[1];
    }

    const char *codepoint::to_cstring(void) const throw() {
        return reinterpret_cast<const char *>(data);
    }

    size_t codepoint::byte_length(void) const throw() {
        return strlen(reinterpret_cast<const char *>(data));
    }
}}
