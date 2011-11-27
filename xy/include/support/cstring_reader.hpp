/*
 * cstring_reader.hpp
 *
 *  Created on: Nov 27, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_CSTRING_READER_HPP_
#define XY_CSTRING_READER_HPP_

#include "xy/include/support/byte_reader.hpp"

namespace xy { namespace support {

    /// forward-only sequential reader of bytes in blocks of some size
    class cstring_reader : public byte_reader {
    private:

        mutable size_t buff_len;
        mutable size_t pos;
        const char *buffer;

    public:
        cstring_reader(const char *) throw();

        virtual ~cstring_reader(void) throw();
        virtual size_t read_block(uint8_t *block, const size_t GIVEN_SIZE) const throw();
    };

}}


#endif /* XY_CSTRING_READER_HPP_ */
