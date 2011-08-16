/*
 * unsafe_cast.hpp
 *
 *  Created on: Aug 15, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_UNSAFE_CAST_HPP_
#define XY_UNSAFE_CAST_HPP_

#include <stdint.h>

#include "xy/include/support/inline.hpp"

namespace xy { namespace support {

    template <typename ToT, typename FromT>
    XY_INLINE ToT unsafe_cast(const FromT &v) throw()  {
        ToT dest;
        memcpy(&dest, &v, sizeof(ToT));
        return dest;
    }

    template <typename ToT, typename FromT>
    XY_INLINE ToT unsafe_cast(FromT *v) throw() {
        return unsafe_cast<ToT>(
            reinterpret_cast<uintptr_t>(v)
        );
    }

}}


#endif /* XY_UNSAFE_CAST_HPP_ */
