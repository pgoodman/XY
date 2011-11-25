/*
 * is_reference.hpp
 *
 *  Created on: Aug 17, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_IS_REFERENCE_HPP_
#define XY_IS_REFERENCE_HPP_

#include "xy/include/mpl/bool.hpp"

namespace xy { namespace mpl {

    template <typename T>
    class is_reference {
    public:
        typedef false_tag result;
        enum : bool {
            RESULT = false
        };
    };

    template <typename T>
    class is_reference<T &> {
    public:
        typedef true_tag result;
        enum : bool {
            RESULT = true
        };
    };

}}


#endif /* XY_IS_REFERENCE_HPP_ */
