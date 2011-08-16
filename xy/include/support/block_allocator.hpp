/*
 * block_allocator.hpp
 *
 *  Created on: Aug 15, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_BLOCK_ALLOCATOR_HPP_
#define XY_BLOCK_ALLOCATOR_HPP_

#include <cassert>
#include <new>

#include "xy/include/support/unsafe_cast.hpp"

#define XY_USE_BLOCK_ALLOCATOR 1

namespace xy { namespace support {

    namespace detail {

        template <typename T>
        struct block_allocator_slot {
        public:

            typedef block_allocator_slot<T> self_type;

            T obj;
            self_type *next;

            block_allocator_slot(void)
                : obj()
                , next(0)
            { }

            ~block_allocator_slot() { }
        };

        template <typename T, const unsigned NUM_SLOTS>
        struct block_allocator_block {
        public:

            typedef block_allocator_block<T,NUM_SLOTS> self_type;
            typedef block_allocator_slot<T> slot_type;

            self_type *next;
            slot_type slots[NUM_SLOTS];

            block_allocator_block(self_type *_next) throw()
                : next(_next)
                , slots()
            {
                slot_type *last(&(slots[NUM_SLOTS - 1]));
                for(slot_type *curr(&(slots[0])); curr <= last; ++curr) {
                    curr->next = curr + 1;
                }
                last->next = 0;
            }

            block_allocator_block(const self_type &) throw()
                : next(0)
                , slots()
            {
                assert(false);
            }

            ~block_allocator_block(void) throw() { }

            self_type &operator=(const self_type &) throw() {
                assert(false);
                return *this;
            }
        };
    }

    /// note: - destructors of parameterized type are only called if all
    ///         allocated objects are deallocated!
    template <typename T, const unsigned BLOCK_SIZE=256U>
    class block_allocator {
    private:

        typedef detail::block_allocator_slot<T> slot_type;
        typedef detail::block_allocator_block<T, BLOCK_SIZE> block_type;
        typedef block_allocator<T,BLOCK_SIZE> self_type;

        slot_type *free_list;
        block_type *block_list;

    public:

        block_allocator(void) throw()
            : free_list(0)
            , block_list(0)
        { }

        block_allocator(const self_type &) throw()
            : free_list(0)
            , block_list(0)
        {
            assert(false);
        }

        ~block_allocator(void) throw() {
            for(block_type *curr(block_list), *next(0); 0 != curr; curr = next) {
                next = curr->next;
                delete curr;
            }

            free_list = 0;
            block_list = 0;
        }

        self_type &operator=(const self_type &) throw() {
            assert(false);
            return *this;
        }

        inline T *allocate(void) throw() {
#if XY_USE_BLOCK_ALLOCATOR
            if(0 == free_list) {
                block_list = new block_type(block_list);
                free_list = &(block_list->slots[0]);
            }

            slot_type *obj(free_list);
            free_list = obj->next;

            return &(obj->obj);
#else
            return new T;
#endif
        }

        inline void deallocate(T *ptr) throw() {
#if XY_USE_BLOCK_ALLOCATOR
            // destroy and re-instantiate
            ptr->~T();
            new (ptr) T;

            const ptrdiff_t diff(
                reinterpret_cast<char *>(&(block_list->slots[0].obj)) -
                reinterpret_cast<char *>(&(block_list->slots[0]))
            );

            slot_type *new_head(0);

            if(diff > 0) {
                new_head = unsafe_cast<slot_type *>(
                    reinterpret_cast<char *>(ptr) - diff
                );
            } else {
                new_head = unsafe_cast<slot_type *>(ptr);
            }

            new_head->next = free_list;
            free_list = new_head;
#else
            delete ptr;
#endif
        }
    };
}}


#endif /* XY_BLOCK_ALLOCATOR_HPP_ */
