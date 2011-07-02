/*
 * lexer.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#include <cstring>
#include <cctype>

#include "xy/include/lexer.hpp"
#include "xy/include/array.hpp"

namespace xy {

    lexer::lexer(void) throw()
        : chr()
    {
        line_tracker[0] = 1;
        column_tracker[0] = 1;
        curr = 0;
        next = 1;
        read_size = BLOCK_SIZE;
        state = READ_BLOCK;
        memset(scratch, 0, array::size(scratch));
    }

    bool lexer::get_codepoint(
        io::file<io::read_tag> &f,
        io::message_queue &mq,
        utf8::codepoint &cp
    )  throw() {
        switch(state) {
        case READ_BLOCK:

            for(; read_size == BLOCK_SIZE; ) {

                read_size = f.read_block<BLOCK_SIZE>(scratch);
                if(0U == read_size) {
                    state = DONE_READING;
                    return false;
                }

                for(i = 0; i < read_size; ++i) {

                    // continue until we read a full codepoint
                    if(!decoder.next_state(scratch[i], chr)) {
                        continue;
                    }

                    if(decoder.found_error()) {
                        mq.push(io::w_invalid_utf8_cp);
                    }

                    // we've read a codepoint
                    line_tracker[next] = line_tracker[curr];
                    column_tracker[next] = column_tracker[curr] + 1;

                    if(chr.is_ascii()) {
                        if('\n' == chr) {
                            line_tracker[next] += 1;
                            column_tracker[next] = 1;
                        } else if('\t' == chr) {
                            column_tracker[next] += 3;
                        } else if(!isgraph(chr.to_cstring()[0])) {
                            column_tracker[next] -= 1;
                        }
                    }

                    state = READ_NEXT_CODEPOINT;
                    cp = chr;
                    return true;

        case READ_NEXT_CODEPOINT:
                    curr = next;
                    next = 1 - curr;
                }
            }
            break;

        case DONE_READING:
            return false;
        }

        // todo: this is an error condition
        if(decoder.is_in_use()) {
            mq.push(io::e_invalid_trailing_utf8_cp);
            return false;
        }

        return false;
    }

    size_t lexer::line(void) const throw() {
        return line_tracker[curr];
    }

    size_t lexer::lexer::column(void) const throw() {
        return column_tracker[curr];
    }
}
