/*
 * list_map.hpp
 *
 *  Created on: Jan 3, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_LIST_MAP_HPP_
#define XY_LIST_MAP_HPP_

namespace xy { namespace support {

    template <typename K, typename V>
    class list_map {
    private:

        struct entry {
            entry *next;
            K key;
            V val;
        };

        entry *entries;

        /// find a value in the mapping and return it
        V *find(const K &key) throw() {
            entry foo;
            foo.next = entries;

            for(entry *prev(&foo), *curr(entries);
                nullptr != curr;
                prev = curr, curr = curr->next) {

                if(key == curr->key) {

                    // re-order to front
                    prev->next = curr->next;
                    curr->next = entries;
                    entries = curr;

                    return &(curr->val);
                }
            }

            return nullptr;
        }

    public:

        list_map(void) throw()
            : entries(nullptr)
        { }

        ~list_map(void) throw() {
            for(entry *curr(entries), *next(nullptr);
                nullptr != curr;
                curr = next) {

                next = curr->next;
                curr->next = nullptr;

                delete curr;
            }
            entries = nullptr;
        }

        /// find a value in the mapping and return it
        bool find(const K key, V &val) throw() {
            V *existing_val(find(key));
            if(nullptr == existing_val) {
                return false;
            }

            val = *existing_val;

            return true;
        }

        void insert(const K key, const V &val) throw() {
            V *existing_val(find(key));
            if(nullptr == existing_val) {
                force_insert(key, val);
            } else {
                *existing_val = val;
            }
        }

        void force_insert(const K key, const V &val) throw() {
            entry *new_entry(new entry);
            new_entry->key = key;
            new_entry->val = val;
            new_entry->next = entries;
            entries = new_entry;
        }
    };

}}


#endif /* XY_LIST_MAP_HPP_ */
