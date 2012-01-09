/*
 * rtti.hpp
 *
 *  Created on: Jan 8, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_RTTI_HPP_
#define XY_RTTI_HPP_

#include <stdint.h>
#include <cstdio>

#include "xy/include/mpl/id.hpp"
#include "xy/include/support/unsafe_cast.hpp"

namespace xy { namespace mpl {

    typedef uint64_t rtti_type;

    template <typename Category, typename Derived, typename Base=void>
    struct rtti;

    /// base class for rtti types
    struct rtti_base {
    public:

        virtual rtti_type type_id(void) const throw() = 0;

        template <typename T>
        bool is_instance(void) const throw() {
            return 0UL != (this->type_id() & T::exact_id());
        }

        template <typename T>
        T *reinterpret(void) throw() {
            if(!this->is_instance<T>()) {
                return nullptr;
            }

            return support::unsafe_cast<T *>(this);
        }
    };

    /// derived type
    template <typename Category, typename Derived, typename Base>
    struct rtti : public Base {
    public:

        rtti(void) throw() { }
        virtual ~rtti(void) throw() { }

        static rtti_type static_id(void) throw() {
            static rtti_type id(Base::static_id() | rtti<Category, Derived, Base>::exact_id());
            return id;
        }

        static rtti_type exact_id(void) throw() {
            static rtti_type id(1ULL << mpl::id<Derived, Category>::get());
            return id;
        }

        virtual rtti_type type_id(void) const throw() {
            printf("class %s has id %lu\n",
                            this->class_name(),
                            rtti<Category, Derived, void>::static_id());
            return rtti<Category, Derived, Base>::static_id();
        }
    };

    // base type
    template <typename Category, typename Derived>
    struct rtti<Category, Derived, void> : public rtti_base {
    public:

        rtti(void) throw() { }
        virtual ~rtti(void) throw() { }

        static rtti_type static_id(void) throw() {
            return rtti<Category, Derived, void>::exact_id();
        }

        static rtti_type exact_id(void) throw() {
            static rtti_type id(1ULL << mpl::id<Derived, Category>::get());
            return id;
        }

        virtual rtti_type type_id(void) const throw() {
            return rtti<Category, Derived, void>::static_id();
        }
    };


}}


#endif /* XY_RTTI_HPP_ */
