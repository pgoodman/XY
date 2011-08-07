/*
 * remove_const.hpp
 *
 *  Created on: Aug 6, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_REMOVE_CONST_HPP_
#define XY_REMOVE_CONST_HPP_

namespace xy { namespace mpl {

    template <typename T>
    class remove_const {
    public:
        typedef T type;
    };

    template <typename T>
    class remove_const<const T> {
    public:
        typedef T type;
    };
}}

#endif /* XY_REMOVE_CONST_HPP_ */
