/*
 * name_map.hpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef NAME_MAP_HPP_
#define NAME_MAP_HPP_

#include <stdint.h>

extern "C" {
    struct _Trie;
    struct _AlphaMap;
}

namespace xy { namespace support {

    class name_map;

    /// unique representation for a name (e.g. identifier, type name).
    class mapped_name {
    private:
        friend class name_map;

        struct mapped_name_impl {
            unsigned slot:12; // offset into the block
            unsigned length:5; // length of the name
            unsigned block_id:15; // id of the block
        } data;

        static_assert(sizeof(uint32_t) >= sizeof(mapped_name_impl),
            "A mapped name must fit into 32 bits."
        );

        mapped_name(const mapped_name_impl &data_) throw();

    public:

        mapped_name(void) throw();
        mapped_name(const mapped_name &) throw();
        ~mapped_name(void) throw();

        mapped_name &operator=(const mapped_name &) throw();
        bool operator==(const mapped_name &) const throw();
        bool operator!=(const mapped_name &) const throw();
    };

    /// a mapping of names to mapped_name.
    /// names in this map can be no longer than 32 bytes.
    class name_map {
    private:

        struct name_block {
        public:
            char slots[4096U];
            unsigned next_offset;
            unsigned id;
            unsigned capacity;
        };

        // trie data structures
        struct _AlphaMap *alpha_map;
        struct _Trie *trie;

        // array of pointers to blocks
        name_block **blocks;

        // most current block id, used as an index into the blocks array
        int block_id;

        // how many block ids the blocks array can hold
        uint16_t capacity;

        /// add a name into a block (where a block is a large array).
        mapped_name::mapped_name_impl add_to_block(const char *) throw();

    public:

        name_map(void) throw();

        ~name_map(void) throw();

        /// add a name to the map and return that name's associated mapped_name
        /// multiple invocations of map_name with equivalent names will yield
        /// the same mapped_name.
        mapped_name map_name(const char *name) throw();

        /// return a C string representation of a given mapped name.
        /// this has the property the following property:
        ///
        ///    map_name("foo") == map_name(unmap_name(map_name("foo")))
        ///
        const char *unmap_name(const mapped_name &name) const throw();
    };

}}

#endif /* NAME_MAP_HPP_ */
