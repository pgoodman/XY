/*
 * reader.hpp
 *
 *  Created on: Nov 29, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_REPL_READER_HPP_
#define XY_REPL_READER_HPP_

#include "xy/include/support/byte_reader.hpp"

namespace xy { namespace repl {

    /// forward-only sequential reader of bytes in blocks of some size
    class reader : public support::byte_reader {
    private:

        mutable size_t pos;
        mutable bool is_empty;
        mutable bool is_done;
        mutable char buffer[8193];

    public:
        reader(void) throw();
        void reset(void) throw();
        bool got_exit(void) const throw();
        const char *history(void) const throw();

        virtual ~reader(void) throw();
        virtual size_t read_block(uint8_t *block, const size_t GIVEN_SIZE) const throw();
    };

}}



#endif /* XY_REPL_READER_HPP_ */
