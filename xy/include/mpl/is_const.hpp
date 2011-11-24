/*
 * is_const.hpp
 *
 *  Created on: Nov 24, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_IS_CONST_HPP_
#define XY_IS_CONST_HPP_

namespace xy { namespace mpl {

    template <typename T>
    class is_const {
    public:
        typedef false_tag result;
        enum {
            RESULT = false
        };
    };

    template <typename T>
    class is_const<const T> {
    public:
        typedef true_tag result;
        enum {
            RESULT = true
        };
    };

}}


#endif /* XY_IS_CONST_HPP_ */
