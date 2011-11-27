/*
 * cstring_reader.cpp
 *
 *  Created on: Nov 27, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include "xy/include/support/cstring_reader.hpp"
#include "xy/include/cstring.hpp"

namespace xy { namespace support {

    static const char *NULL_STRING("");

    cstring_reader::cstring_reader(const char *buffer_) throw()
        : buff_len(nullptr == buffer_ ? 0UL : cstring::byte_length(buffer_))
        , pos(0UL)
        , buffer(nullptr == buffer_ ? NULL_STRING : buffer_)
    { }

    cstring_reader::~cstring_reader(void) throw() {
        buffer = NULL_STRING;
        pos = 0;
        buff_len = 0;
    }

    size_t cstring_reader::read_block(uint8_t *block, const size_t GIVEN_SIZE) const throw() {

        if(pos >= buff_len) {
            return 0UL;
        }

        const size_t read_amount(((pos + GIVEN_SIZE) <= buff_len) ? GIVEN_SIZE : buff_len - pos);
        memcpy(block, &(buffer[pos]), read_amount);

        pos += read_amount;

        return read_amount;
    }

}}

