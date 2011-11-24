/*
 * cstring.hpp
 *
 *  Created on: Jul 2, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef XY_CSTRING_HPP_
#define XY_CSTRING_HPP_

#include <stdint.h>
#include <cstring>

#include "xy/include/pp.hpp"

namespace xy { namespace cstring {

    /// return a heap-allocated copy of a C-string
    char *copy(const char *str) throw();
    char *copy(const uint8_t *str) throw();

    /// free a heap-allocated C string
    void free(const char *str) throw();
    void free(const uint8_t *str) throw();

    /// get the number of bytes in a c string, excluding the the null
    /// byte.
    size_t byte_length(const char *str) throw();
    size_t byte_length(const uint8_t *str) throw();

}}

#if XY_HOST_LINUX
size_t   strlcat(char *, const char *, size_t);
size_t   strlcpy(char *, const char *, size_t);
#endif

#endif /* XY_CSTRING_HPP_ */
