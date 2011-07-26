/*
 * name_map.hpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef NAME_MAP_HPP_
#define NAME_MAP_HPP_

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

        const char *name;

        mapped_name(const char *name_) throw();

    public:

        mapped_name(void) throw();
        mapped_name(const mapped_name &) throw();
        ~mapped_name(void) throw();

        mapped_name &operator=(const mapped_name &) throw();
    };

    /// a mapping of names to mapped_name.
    class name_map {
    private:

        struct name_block {
        public:
            char block[4096U];
            size_t capacity;
            name_block *next;
        };

        struct _AlphaMap *alpha_map;
        struct _Trie *trie;
        name_block *blocks;

        /// add a name into a block (where a block is a large array).
        char *add_to_block(const char *) throw();

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
        const char *unmap_name(mapped_name name) const throw();
    };

}}

#endif /* NAME_MAP_HPP_ */
