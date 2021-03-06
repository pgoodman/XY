/*
 * cstring.cpp
 *
 *  Created on: Jul 2, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#include <cstring>

#include "xy/include/cstring.hpp"

namespace xy { namespace cstring {

    char *copy(const char *str) throw() {
        const size_t str_len(byte_length(str));
        char *copy(new char[str_len + 1]);
        memcpy(copy, str, str_len * sizeof(char));
        copy[str_len] = '\0';
        return copy;
    }

    char *copy(const uint8_t *str) throw() {
        const size_t str_len(byte_length(str));
        char *copy(new char[str_len + 1]);
        memcpy(copy, str, str_len * sizeof(char));
        copy[str_len] = '\0';
        return copy;
    }

    void free(const char *str) throw() {
        delete [] str;
    }

    void free(const uint8_t *str) throw() {
        delete [] str;
    }

    size_t byte_length(const char *str) throw() {
        size_t i(0);
        for(; '\0' != str[i]; ++i) { }
        return i;
    }

    size_t byte_length(const uint8_t *str) throw() {
        size_t i(0);
        for(; '\0' != str[i]; ++i) { }
        return i;
    }
}}


