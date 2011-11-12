/*
 * if_.hpp
 *
 *  Created on: Aug 17, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_IF__HPP_
#define XY_IF__HPP_

#include "xy/include/mpl/bool.hpp"

namespace xy { namespace mpl {

    namespace detail {
        template <typename result_tag, typename true_type, typename false_type>
        class if_impl;

        template <typename true_type, typename false_type>
        class if_impl<true_tag, true_type, false_type> {
        public:
            typedef true_type result;
        };

        template <typename true_type, typename false_type>
        class if_impl<false_tag, true_type, false_type> {
        public:
            typedef false_type result;
        };
    }

    template <typename result_tag, typename true_type, typename false_type>
    class if_ : public detail::if_impl<typename result_tag::result, true_type, false_type> { };

    template <typename true_type, typename false_type>
    class if_<true_tag, true_type, false_type> {
    public:
        typedef true_type result;
    };

    template <typename true_type, typename false_type>
    class if_<false_tag, true_type, false_type> {
    public:
        typedef false_type result;
    };

}}

#endif /* XY_IF__HPP_ */
