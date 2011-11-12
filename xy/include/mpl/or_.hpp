/*
 * or_.hpp
 *
 *  Created on: Aug 17, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_OR__HPP_
#define XY_OR__HPP_

namespace xy { namespace mpl {


    namespace detail {
        template <typename result_tag_a, typename result_tag_b>
        class or_impl;

        template <>
        class or_impl<true_tag, true_tag> {
        public:
            typedef true_tag result;
        };

        template <>
        class or_impl<true_tag, false_tag> {
        public:
            typedef true_tag result;
        };

        template <>
        class or_impl<false_tag, true_tag> {
        public:
            typedef true_tag result;
        };

        template <>
        class or_impl<false_tag, false_tag> {
        public:
            typedef false_tag result;
        };
    }

    template <typename result_tag_a, typename result_tag_b>
    class or_ : public detail::or_impl<typename result_tag_a::result, typename result_tag_b::result> { };

    template <>
    class or_<true_tag, true_tag> {
    public:
        typedef true_tag result;
    };

    template <>
    class or_<true_tag, false_tag> {
    public:
        typedef true_tag result;
    };

    template <>
    class or_<false_tag, true_tag> {
    public:
        typedef true_tag result;
    };

    template <>
    class or_<false_tag, false_tag> {
    public:
        typedef false_tag result;
    };
}}



#endif /* XY_OR__HPP_ */
