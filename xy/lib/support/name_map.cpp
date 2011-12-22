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

namespace xy { namespace support {

    name_map::name_map(void) throw()
        : hash_set()
    { }

    name_map::~name_map(void) throw() { }

    /// map a name
    mapped_name name_map::map_name(const char *name) throw() {
        return hash_set.add(
            name,
            static_cast<int>(cstring::byte_length(name) + 1U)
        );
    }

    /// unmap a mapped name
    const char *name_map::unmap_name(const mapped_name &name) const throw() {
        return hash_set.find(name);
    }
}}
