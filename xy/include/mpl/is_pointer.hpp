/*
 * is_pointer.hpp
 *
 *  Created on: Aug 16, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_IS_POINTER_HPP_
#define XY_IS_POINTER_HPP_

#include "xy/include/mpl/bool.hpp"

namespace xy { namespace mpl {

    template <typename T>
    class is_pointer {
    public:
        typedef false_tag result;
        enum {
            RESULT = false
        };
    };

    template <typename T>
    class is_pointer<T *> {
    public:
        typedef true_tag result;
        enum {
            RESULT = true
        };
    };

}}

#endif /* XY_IS_POINTER_HPP_ */
