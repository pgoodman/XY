/*
 * id.hpp
 *
 *  Created on: Nov 22, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_ID_HPP_
#define XY_ID_HPP_

namespace xy { namespace mpl {

    typedef unsigned type_id;

    namespace {
        template<typename T>
        class id_impl {
        public:
            static unsigned get(void) throw() {
                static unsigned id_(0);
                return id_++;
            }
        };
    }

    template <typename T, typename Category=void>
    class id {
    public:
        static type_id get(void) throw() {
            static type_id id_(id_impl<Category>::get());
            return id_;
        }
    };
}}

#endif /* XY_ID_HPP_ */
