/*
 * small_set.hpp
 *
 *  Created on: Aug 16, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_SMALL_SET_HPP_
#define XY_SMALL_SET_HPP_

#include <functional>
#include <stdint.h>

#include "xy/include/mpl/is_pointer.hpp"
#include "xy/include/mpl/is_reference.hpp"
#include "xy/include/mpl/if_.hpp"
#include "xy/include/mpl/or_.hpp"
#include "xy/include/mpl/remove_const.hpp"

namespace xy { namespace support {

    /// represents a small set of objects where insertion and removal are O(N).
    template <typename T>
    class small_set {
    private:

        static const std::less<T> LESS_THAN;

        enum {
            USES_POINTER = mpl::is_pointer<T>::RESULT
        };

        uint16_t num_elems;
        uint16_t capacity;
        T *elems;

        typedef typename mpl::remove_const<T>::type unconst_T;
        typedef const typename mpl::if_<
            mpl::or_<mpl::is_pointer<unconst_T>, mpl::is_reference<unconst_T>>,
            unconst_T,
            unconst_T &
        >::result parameter_type;

        /// increase the capacity of the set
        T *reserve(uint16_t new_capacity) throw() {
            capacity = new_capacity;
            return new T[capacity];
        }

        /// do a binary search to find where to place something
        bool find(parameter_type obj, size_t &next_index) throw() {
            size_t base(0);
            size_t last_checked(capacity + 1);
            next_index = num_elems / 2;

            for(; last_checked != next_index && next_index < num_elems; ) {
                last_checked = next_index;
                if(obj < elems[next_index]) {
                    next_index = next_index - ((next_index - base + 1) / 2);
                } else {
                    next_index = next_index + ((next_index - base + 1) / 2);
                    base = last_checked;
                }
            }

            return next_index < num_elems && obj == elems[next_index];
        }

    public:

        small_set(parameter_type obj) throw()
            : num_elems(0)
            , capacity(0)
            , elems(nullptr)
        {
            elems = reserve(2);
            num_elems = 1;
            elems[0] = obj;
        }

        /// assumed that obj0 < obj1
        small_set(parameter_type obj0, parameter_type obj1) throw()
            : num_elems(0)
            , capacity(0)
            , elems(nullptr)
        {
            elems = reserve(2);
            elems[0] = obj0;
            elems[1] = obj1;
            num_elems = 2;
        }

        /// assumed that obj0 < obj1 < obj2
        small_set(parameter_type obj0, parameter_type obj1, parameter_type obj2) throw()
            : num_elems(0)
            , capacity(0)
            , elems(nullptr)
        {
            elems = reserve(4);
            elems[0] = obj0;
            elems[1] = obj1;
            elems[2] = obj2;
            num_elems = 3;
        }

        /// assumed that obj0 < obj1 < obj2
        small_set(parameter_type obj0, parameter_type obj1, parameter_type obj2, parameter_type obj3) throw()
            : num_elems(0)
            , capacity(0)
            , elems(nullptr)
        {
            elems = reserve(4);
            elems[0] = obj0;
            elems[1] = obj1;
            elems[2] = obj2;
            elems[3] = obj3;
            num_elems = 4;
        }

        small_set(void) throw()
            : num_elems(0)
            , capacity(0)
            , elems(nullptr)
        { }

        small_set(small_set &&that) throw()
            : num_elems(that.num_elems)
            , capacity(that.capacity)
            , elems(that.elems)
        {
            that.num_elems = 0;
            that.capacity = 0;
            that.elems = nullptr;
        }

        ~small_set(void) throw() {
            if(nullptr != elems) {
                delete [] elems;
                elems = nullptr;
                capacity = 0;
                num_elems = 0;
            }
        }

        /// add an item into the set; returns true if the item wasn't in the
        /// set, false otherwise.
        bool add(parameter_type obj) throw() {

            if(nullptr == elems) {
                num_elems = 1;
                elems = reserve(2);
                elems[0] = obj;
                return true;
            }

            size_t insert_at(0);
            if(!find(obj, insert_at)) {

                // allocate new storage, copy in the shifted elements
                if((num_elems + 1) > capacity) {
                    T *new_elems(reserve(capacity * 2U));

                    for(size_t i(0); i < insert_at; ++i) {
                        new_elems[i] = elems[i];
                    }

                    for(size_t i(insert_at); i < num_elems; ++i) {
                        new_elems[i + 1] = elems[i];
                    }

                    delete [] elems;
                    elems = new_elems;

                // shift
                } else {
                    for(size_t i(num_elems); i >= insert_at; --i) {
                        elems[i] = elems[i - 1];
                    }
                }

                elems[insert_at] = obj;
                num_elems += 1;
                return true;
            }

            return false;
        }

        /// remove an item from the set; returns false if the item was never
        /// in the set.
        bool remove(parameter_type obj) throw() {
            if(nullptr == elems) {
                return false;
            }

            size_t insert_at(0);

            if(!find(obj, insert_at)) {
                return false;
            }

            // shift backward
            for(size_t i(insert_at); i < num_elems; ++i) {
                elems[i] = elems[i + 1];
            }

            num_elems -= 1;

            if(0 == num_elems) {
                delete [] elems;
                capacity = 0;
                elems = nullptr;
            }

            return true;
        }

        size_t size(void) const throw() {
            return num_elems;
        }


    };

    // initialize
    template <typename T>
    const std::less<T> small_set<T>::LESS_THAN;
}}

#endif /* XY_SMALL_SET_HPP_ */
