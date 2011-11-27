/*
 * equal.hpp
 *
 *  Created on: Jun 28, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef XY_EQUAL_HPP_
#define XY_EQUAL_HPP_

#include "xy/include/mpl/bool.hpp"

namespace xy { namespace mpl {

    template <typename A, typename B>
    class equal {
    public:
        typedef false_tag result;
        operator bool (void) {
            return false;
        }
    };

    template <typename A>
    class equal<A *, A *> {
    public:
        typedef true_tag result;
        operator bool (void) {
            return true;
        }
    };

    template <typename A>
    class equal<A, A> {
    public:
        typedef true_tag result;
        operator bool (void) {
            return true;
        }
    };

    template <typename A, typename B>
    class equal<A *, B *> : public equal<A, B> { };

    template <typename A, typename B>
    class equal<const A, const B> : public equal<A, B> { };

    template <typename A, typename B>
    class equal<const A, B> : public equal<A, B> { };

    template <typename A, typename B>
    class equal<A, const B> : public equal<A, B> { };

    template <typename A, typename B>
    class const_equal {
    public:
        typedef false_tag result;
        operator bool (void) {
            return false;
        }
    };

    template <typename A>
    class const_equal<A *, A *> {
    public:
        typedef true_tag result;
        operator bool (void) {
            return true;
        }
    };

    template <typename A>
    class const_equal<A, A> {
    public:
        typedef true_tag result;
        operator bool (void) {
            return true;
        }
    };

    template <typename A, typename B>
    class const_equal<A *, B *> : public const_equal<A, B> { };

    template <typename A, typename B>
    class const_equal<const A, const B> : public equal<A, B> { };

    template <typename A, typename B>
    class const_equal<const A, B> {
    public:
        typedef false_tag result;
        operator bool (void) {
            return false;
        }
    };

    template <typename A, typename B>
    class const_equal<A, const B> {
    public:
        typedef false_tag result;
        operator bool (void) {
            return false;
        }
    };
    /*
    template <typename A, typename B>
    bool equal_p(void) {
        return equal<A,B>();
    }

    template <typename A, typename B>
    bool const_equal_p(void) {
        return const_equal<A,B>();
    }
    */
}}

#endif /* XY_EQUAL_HPP_ */
