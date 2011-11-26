/*
 * name_map.cpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include <cstring>

#include "xy/include/support/name_map.hpp"
#include "xy/include/support/unsafe_cast.hpp"

#include "xy/include/array.hpp"
#include "xy/include/cstring.hpp"

#include "xy/deps/libdatrie/alpha-map.h"
#include "xy/deps/libdatrie/trie.h"

namespace xy { namespace support {

    static const unsigned DATA_FILL[] = {~0U, ~0U, ~0U};

    mapped_name::mapped_name(const mapped_name_impl &data_) throw()
        : data(data_)
    { }

    mapped_name::mapped_name(void) throw() {
        memcpy(&data, &(DATA_FILL[0]), sizeof data);
    }

    mapped_name::mapped_name(const mapped_name &that) throw()
        : data(that.data)
    { }

    mapped_name::~mapped_name(void) throw() {
        memcpy(&data, &(DATA_FILL[0]), sizeof data);
    }

    mapped_name &mapped_name::operator=(const mapped_name &that) throw() {
        data = that.data;
        return *this;
    }

    bool mapped_name::operator==(const mapped_name &that) const throw() {
        return 0 == memcmp(this, &that, sizeof *this);
    }
    bool mapped_name::operator!=(const mapped_name &that) const throw() {
        return 0 != memcmp(this, &that, sizeof *this);
    }
    bool mapped_name::operator<(const mapped_name &that) const throw() {
        return *reinterpret_cast<const uint32_t *>(this) < *reinterpret_cast<const uint32_t *>(&that);
    }

    name_map::name_map(void) throw()
        : alpha_map(nullptr)
        , trie(nullptr)
        , blocks(nullptr)
        , block_id(-1)
        , capacity(0)
    {
        alpha_map = alpha_map_new();

        alpha_map_add_range(alpha_map, 33U, 122U);

        trie = trie_new(alpha_map);
    }

    name_map::~name_map(void) throw() {
        trie_free(trie);
        alpha_map_free(alpha_map);
        if(nullptr != blocks) {
            for(unsigned i(0); i < capacity; ++i) {
                if(nullptr != blocks[i]) {
                    delete blocks[i];
                    blocks[i] = nullptr;
                }
            }

            delete [] blocks;
            blocks = nullptr;
        }
    }

    /// add the cstring name to the end of the block, starting from the end
    /// of the most recently added block
    mapped_name::mapped_name_impl name_map::add_to_block(const char *name) throw() {
        const size_t len(cstring::byte_length(name) + 1U);
        name_block *block(nullptr);

        // check if we have no blocks or if the last block doesn't have the
        // capacity to hold this word
        if(nullptr == blocks || len > blocks[block_id]->capacity) {
            ++block_id;

            if(nullptr == blocks || block_id >= static_cast<int>(capacity)) {
                unsigned new_capacity = capacity * 2;
                if(new_capacity <= capacity) {
                    new_capacity = ((capacity / 2) + 2) * 2;
                }

                // like a realloc
                name_block **new_blocks(new name_block *[new_capacity]);
                memset(new_blocks, 0, sizeof(name_block *) * new_capacity);
                memcpy(new_blocks, blocks, capacity);
                memset(blocks, 0, sizeof(name_block *) * capacity);
                capacity = new_capacity;
                delete [] blocks;
                blocks = new_blocks;
            }

            // make our new block
            name_block *new_block(new name_block);
            new_block->capacity = array::length(new_block->slots);
            new_block->id = block_id;
            new_block->next_offset = 0;
            memset(&(new_block->slots[0]), 0, array::size(new_block->slots));

            // add it in
            blocks[block_id] = new_block;
        }

        block = blocks[block_id];

        char *write_loc(&(block->slots[block->next_offset]));
        memcpy(write_loc, name, len);

        mapped_name::mapped_name_impl name_loc;
        name_loc.block_id = block_id;
        name_loc.length = len - 1U;
        name_loc.slot = block->next_offset;

        block->next_offset += len;
        block->capacity -= len;

        return name_loc;
    }

    /// map a name
    mapped_name name_map::map_name(const char *name) throw() {
        mapped_name data;
        if(trie_retrieve(trie, name, support::unsafe_cast<uint32_t *>(&data))) {
            return mapped_name(data);
        } else {
            data = add_to_block(name);
            trie_store(trie, name, *support::unsafe_cast<uint32_t *>(&data));
            return mapped_name(data);
        }
    }

    /// unmap a mapped name
    const char *name_map::unmap_name(const mapped_name &name) const throw() {
        if(nullptr == blocks || name.data.block_id > block_id) {
            return nullptr;
        }

        const name_block *block(blocks[name.data.block_id]);

        // bad slot, each name should be null-terminated
        if(0 < name.data.slot && '\0' != block->slots[name.data.slot - 1]) {
            return nullptr;

        // bad slot, each name must be of non-zero positive length
        } else if('\0' == block->slots[name.data.slot]) {
            return nullptr;

        // bad slot, name length doesn't match byte length of name
        } else if(name.data.length
               != cstring::byte_length(&(block->slots[name.data.slot]))) {
            return nullptr;
        }

        return &(block->slots[name.data.slot]);
    }
}}
