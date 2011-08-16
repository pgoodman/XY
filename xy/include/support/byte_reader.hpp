/*
 * byte_reader.hpp
 *
 *  Created on: Aug 11, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_BYTE_READER_HPP_
#define XY_BYTE_READER_HPP_

#include <cstring>
#include <stdint.h>

namespace xy { namespace support {

    /// forward-only sequential reader of bytes in blocks of some size
    class byte_reader {
    public:
        virtual ~byte_reader(void) throw();
        virtual size_t read_block(uint8_t *block, const size_t GIVEN_SIZE) const throw() = 0;
    };

}}


#endif /* XY_BYTE_READER_HPP_ */
