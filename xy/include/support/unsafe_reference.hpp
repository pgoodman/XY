/*
 * unsafe_reference.hpp
 *
 *  Created on: Nov 10, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_UNSAFE_REFERENCE_HPP_
#define XY_UNSAFE_REFERENCE_HPP_

#include "xy/include/support/unsafe_cast.hpp"

namespace xy { namespace support {

    template <typename T>
    T &unsafe_reference(void) throw() {
        char dummy;
        return *support::unsafe_cast<T *>(&dummy);
    }

}}

#endif /* XY_UNSAFE_REFERENCE_HPP_ */
