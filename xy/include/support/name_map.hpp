/*
 * name_map.hpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef NAME_MAP_HPP_
#define NAME_MAP_HPP_

/*
#include <stdint.h>

extern "C" {
    struct _Trie;
    struct _AlphaMap;
}*/

#include "xy/include/support/hash_array_set.hpp"

namespace xy { namespace support {

    class name_map;

    typedef support::hash_set_array_handle mapped_name;

    /// a mapping of names to mapped_name.
    /// names in this map can be no longer than 32 bytes.
    class name_map {
    private:
        support::hash_array_set<char> hash_set;

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
