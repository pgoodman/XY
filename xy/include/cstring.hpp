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

namespace xy { namespace cstring {

    char *copy(const char *str) throw();
    void free(const char *str) throw();

}}

#endif /* XY_CSTRING_HPP_ */
