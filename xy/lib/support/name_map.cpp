/*
 * name_map.cpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include <cstring>

#include "xy/include/support/name_map.hpp"
#include "xy/include/array.hpp"

#include "xy/deps/libdatrie/alpha-map.h"
#include "xy/deps/libdatrie/trie.h"

namespace xy { namespace support {

    mapped_name::mapped_name(const char *name_) throw()
        : name(name_)
    { }

    mapped_name::mapped_name(void) throw()
        : name(nullptr)
    { }

    mapped_name::mapped_name(const mapped_name &that) throw()
        : name(that.name)
    { }

    mapped_name::~mapped_name(void) throw() {
        name = nullptr;
    }

    mapped_name &mapped_name::operator=(const mapped_name &that) throw() {
        name = that.name;
        return *this;
    }

    name_map::name_map(void) throw()
        : alpha_map(nullptr)
        , trie(nullptr)
        , blocks(nullptr)
    {
        alpha_map = alpha_map_new();

        alpha_map_add_range(alpha_map, 33U, 122U);

        trie = trie_new(alpha_map);
    }

    name_map::~name_map(void) throw() {
        trie_free(trie);
        alpha_map_free(alpha_map);
    }

    /// add the cstring name to the end of the block, starting from the end
    /// of the most recently added block
    char *name_map::add_to_block(const char *name) throw() {
        const size_t len(strlen(name) + 1U);

        // make a new block
        if(nullptr == blocks || len > blocks->capacity) {
            name_block *new_block(new name_block);
            new_block->next = blocks;
            new_block->capacity = array::size(new_block->block);
            blocks = new_block;
        }

        blocks->capacity -= len;

        char *write_loc(&(blocks->block[blocks->capacity]));
        memcpy(write_loc, name, len);

        return write_loc;
    }

    mapped_name name_map::map_name(const char *name) throw() {
        char *data(nullptr);
        if(trie_retrieve(trie, name, &data)) {
            return mapped_name(data);
        } else {
            char *name_(add_to_block(name));
            trie_store(trie, name_, name_);
            return mapped_name(name_);
        }
    }

    const char *name_map::unmap_name(const mapped_name name) const throw() {
        return name.name;
    }
}}
