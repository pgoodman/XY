/*
 * hash_array_set.cpp
 *
 *  Created on: Dec 21, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include "xy/include/support/hash_array_set.hpp"

namespace xy { namespace support {

    hash_array_set_handle::hash_array_set_handle(uint64_t handle_) throw()
        : handle(handle_)
    { }

    hash_array_set_handle::hash_array_set_handle(void) throw()
        : handle(0U)
    { }

    hash_array_set_handle::~hash_array_set_handle(void) throw() { }

    bool hash_array_set_handle::operator==(const hash_array_set_handle &other) const throw() {
        if(handle == other.handle) {
            return true;
        }

        enum : uint64_t {
            MASK_OFF_ID = 0xFFFFFFFFFFFFU
        };

        return (handle & MASK_OFF_ID) == (other.handle & MASK_OFF_ID);
    }

    bool hash_array_set_handle::operator!=(const hash_array_set_handle &other) const throw() {
        return !(*this == other);
    }

}}
