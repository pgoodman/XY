/*
 * hash_set.hpp
 *
 *  Created on: Dec 21, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_HASH_ARRAY_SET_HPP_
#define XY_HASH_ARRAY_SET_HPP_

#include <cassert>
#include <cstring>

#include "xy/include/array.hpp"
#include "xy/deps/murmurhash/MurmurHash3.h"

namespace xy { namespace support {

    namespace detail {
        static const size_t primes[] = {
            53, 97, 193, 389,
            769, 1543, 3079, 6151,
            12289, 24593, 49157, 98317,
            196613, 393241, 786433, 1572869,
            3145739, 6291469, 12582917, 25165843,
            50331653, 100663319, 201326611, 402653189,
            805306457, 1610612741
        };
    }

    template <typename T>
    class hash_array_set;

    class hash_array_set_handle {
    private:

        template <typename> friend class hash_array_set;

        uint64_t handle;

        hash_array_set_handle(uint64_t handle_) throw();

    public:
        hash_array_set_handle(void) throw();
        ~hash_array_set_handle(void) throw();
        bool operator==(const hash_array_set_handle &other) const throw();
        bool operator!=(const hash_array_set_handle &other) const throw();
    };

    template <typename T>
    class hash_array_set {
    private:

        enum : int {
            NUM_SLOTS = 4096
        };

        enum : uint32_t {
            SEED = 0x5f3759dfU
        };

        /// a list of blocks for storing the arrays
        struct block_list {
        public:
            T slots[NUM_SLOTS];
            int num_used_slots;
            block_list *next_block;

            block_list(void) throw()
                : num_used_slots(0)
                , next_block(nullptr)
            {
                memset(&(slots[0]), 0, array::size(slots));
            }
        };

        /// slot type
        struct slot_type {
        public:
            T *array;
            uint32_t hash;
            int32_t len;
        };

        block_list *block;
        slot_type *slots;
        int slot_index;
        int num_used_slots;
        uint16_t table_id;

        static uint16_t next_table_id;

        /// return a pointer to where the array will be copied and update
        /// the number of used slots in the head block.
        T *copy_area(int size) throw() {
            assert(size < NUM_SLOTS);

            if(nullptr == block) {
                block = new block_list;
            }

            const int num_used_slots(block->num_used_slots);

            if((size + num_used_slots) >= NUM_SLOTS) {
                block_list *new_block(new block_list);
                new_block->next_block = block;
                block = new_block;
            }

            block->num_used_slots += size;

            return &(block->slots[num_used_slots]);
        }

        /// find the thing in the hash table, with high probability
        slot_type *find(slot_type *table, size_t table_len, uint32_t hash, int len) const throw() {
            size_t loc(hash % table_len);
            for(;;) {

                // linear probing
                for(; loc < table_len; ++loc) {
                    if(-1 == table[loc].len) {
                        return &(table[loc]);

                    // possible duplicate; don't insert
                    } else if(hash == table[loc].hash
                           && len == table[loc].len) {

                        return &(table[loc]);
                    }
                }
                loc = 0U;
            }

            return nullptr;
        }

        /// add an array into the hash table
        slot_type *insert(slot_type *table, size_t table_len, uint32_t hash, int len, const T *arr) throw() {
            slot_type *new_slot(find(table, table_len, hash, len));

            if(-1 != new_slot->len
            && 0 == memcmp(new_slot->array, arr, len * sizeof(T))) {
                return nullptr;
            }

            new_slot->len = len;
            new_slot->hash = hash;
            return new_slot;
        }

        /// resize the hash table
        void resize(void) throw() {

            const size_t new_length(detail::primes[slot_index + 1]);
            const size_t old_length(detail::primes[slot_index]);

            slot_type *new_slots(new slot_type[new_length]);
            memset(new_slots, -1, sizeof(slot_type) * new_length);

            // copy from the old to the new
            for(size_t i(0); i < old_length; ++i) {
                if(-1 == slots[i].len) {
                    continue;
                }

                // copy in
                slot_type *new_slot(insert(new_slots, new_length, slots[i].hash, slots[i].len, slots[i].array));
                if(nullptr != new_slot) {
                    new_slots->array = slots[i].array;
                }
            }

            // replace old slots
            delete [] slots;
            slots = new_slots;
            ++slot_index;
        }

    public:

        static_assert(-1 == 0xFFFFFFFF, "Must be using two's complement.");

        hash_array_set(void) throw()
            : block(nullptr)
            , slots(nullptr)
            , slot_index(-1)
            , num_used_slots(0)
            , table_id(next_table_id++)
        { }

        /// clear out the hash array set
        ~hash_array_set(void) throw() {

            // clear out the arrays
            for(block_list *curr(block), *next(nullptr); nullptr != curr; curr = next) {
                next = curr->next_block;
                delete curr;
            }
            block = nullptr;

            // clear out the hash table
            if(nullptr != slots) {
                delete [] slots;
                slots = nullptr;
            }
        }

        /// add an array into the hash array set; this returns a handle
        /// back to that thing in the hash array set.
        hash_array_set_handle add(const T *arr, int len) throw() {
            assert(nullptr != arr);
            assert(0 < len);

            uint32_t hash(0);
            uint64_t handle(0);
            MurmurHash3_x86_32(arr, len, SEED, &hash);

            // initialize the hash table
            if(nullptr == slots) {
                slot_index = 0;
                slots = new slot_type[detail::primes[slot_index]];
                memset(slots, -1, detail::primes[slot_index] * sizeof(slot_type));

            // possibly resize the hash table
            } else {
                const float num_slots_(static_cast<float>(detail::primes[slot_index]));
                const float num_used_slots_(static_cast<float>(num_used_slots));
                if((num_used_slots_ / num_slots_) > 0.65) {
                    resize();
                }
            }

            // insert into the hash table
            slot_type *new_slot(insert(slots, detail::primes[slot_index], hash, len, arr));

            // copy the array in
            if(nullptr != new_slot) {
                T *new_arr(copy_area(len));
                memcpy(new_arr, arr, len * sizeof(T));
                new_slot->array = new_arr;
            }

            // build up the handle
            handle |= static_cast<uint64_t>(len & 0xFFFF) << 16U << 16U;
            handle |= static_cast<uint64_t>(table_id) << 16U << 16U << 16U;
            handle |= hash;

            return hash_array_set_handle(handle);
        }

        enum : uint64_t {
            MASK_HASH = 0xFFFFFFFFU
        };

        /// does this hash array set contain a specific thing?
        bool contains(hash_array_set_handle h) throw() {
            static int id(static_cast<int>(h.handle >> 16U >> 16U >> 16U));
            if(id == table_id) {
                return true;
            }

            const uint32_t hash(static_cast<uint32_t>(h.handle & MASK_HASH));
            const int len(static_cast<int>((h.handle >> 16U >> 16U) & 0xFFFF));

            return -1 != find(slots, detail::primes[slot_index], hash, len);
        }

        /// check if the hash array contains some array
        bool contains(const T *arr, int len) const throw() {
            uint32_t hash(0);
            MurmurHash3_x86_32(arr, len, SEED, &hash);
            return -1 != find(slots, detail::primes[slot_index], hash, len);
        }

        /// try to extract something from the hash set.
        const T *find(hash_array_set_handle h) const throw() {
            const uint32_t hash(static_cast<uint32_t>(h.handle & MASK_HASH));
            const int len(static_cast<int>((h.handle >> 16U >> 16U) & 0xFFFF));
            slot_type *slot(find(slots, detail::primes[slot_index], hash, len));
            if(-1 == slot->len) {
                return nullptr;
            }
            return slot->array;
        }
    };

    template <typename T>
    uint16_t hash_array_set<T>::next_table_id(0U);

}}

#endif /* XY_HASH_ARRAY_SET_HPP_ */
