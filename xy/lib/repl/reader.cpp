/*
 * reader.cpp
 *
 *  Created on: Nov 29, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */
#include <cstdio>
#include "xy/include/repl/reader.hpp"
#include "xy/include/repl/repl.hpp"

#define D(x)

namespace xy { namespace repl {

    static char NULL_STRING[] = {'\0','\0','\0'};

    reader::reader(char *buffer_) throw()
        : pos(0UL)
        , is_empty(true)
        , buffer(nullptr == buffer_ ? NULL_STRING : buffer_)
    { }

    reader::~reader(void) throw() {
        buffer = NULL_STRING;
        pos = 0;
        is_empty = true;
    }

    void reader::reset(void) throw() {
        pos = 0;
        is_empty = true;
        buffer[0] = '\0';
        buffer[1] = '\0';
    }

    /// read up to the end of a string or the end of a line
    size_t reader::read_block(uint8_t *block, const size_t GIVEN_SIZE) const throw() {
        bool just_read(false);

        // the buffer is empty, do the first read
        if(is_empty && repl::check()) {
            D( printf("READER: begin fill buffer\n"); )
            repl::read::yield();
            is_empty = false;
            just_read = true;

        // don't do anything; REPL isn't on anymore
        } else if(!repl::check()) {
            D( printf("READER: repl is done.\n"); )
            return 0U;
        }

        // we haven't just requested new info from the REPL but we should.
        if(!just_read && '\n' == buffer[pos]) {
            repl::read::yield();
        }


        /*
        D( printf("READER: trying to read block! buffer is '%s', tail is '%s'\n", buffer, &(buffer[pos])); )

        for(; repl::check() && repl::should_wait(); ) {
            // we've just started evaluating a new block; yield to the reader
            if(0 == pos && 0 == buffer[pos]) {
                D( printf("READER: reader wants initial\n"); )
                repl::read::yield();
                continue;
            }

            // we're at the new line boundary; yield back to the reader so the
            if('\n' == buffer[pos]) {

                D( printf("READER: reader wants more\n"); )
                repl::read::yield();
                ++pos;
                continue;
            } else if(repl::should_wait()) {
                D( printf("READER: buffer='%s' pos=%lu  tail='%s'\n", buffer, pos, &(buffer[pos])); )
            }

            break;
        }
        */

        size_t given_size(GIVEN_SIZE - 1U); // -1 to allow for trailing '\0'
        size_t read_amount(0);
        for(; read_amount <= given_size; ++read_amount) {
            if('\0' == buffer[pos + read_amount]) {
                break;
            } else if(0 < read_amount && '\n' == buffer[pos + read_amount]) {
                D( printf("READER: new line at offset %lu+%lu = %lu, returning read amound %lu\n", pos, read_amount, pos + read_amount, read_amount); )
                break;
            }
        }

        memcpy(block, &(buffer[pos]), read_amount);
        block[read_amount] = '\0';
        pos += read_amount;

        D( printf("READER: read block of size %lu\n", read_amount); )

        return read_amount;
    }

}}


