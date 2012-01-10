/*
 * reader.cpp
 *
 *  Created on: Nov 29, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */
#include <cstdio>
#include <cstdlib>

#include "xy/include/repl/reader.hpp"
#include "xy/include/repl/repl.hpp"
#include "xy/include/cstring.hpp"
#include "xy/include/array.hpp"

#include "xy/deps/linenoise/linenoise.h"

#define D(x)

namespace xy { namespace repl {

    /// set auto-completion things for linenoise
    static void completion(const char *buff, linenoiseCompletions *lc) {
        switch(buff[0]) {
        case 'l': linenoiseAddCompletion(lc,"let"); break;
        case 'f': linenoiseAddCompletion(lc,"function"); break;
        case 'r':
            linenoiseAddCompletion(lc,"record");
            linenoiseAddCompletion(lc,"return");
            break;
        case 'u': linenoiseAddCompletion(lc,"union"); break;
        case '-': linenoiseAddCompletion(lc,"->"); break;
        case ':': linenoiseAddCompletion(lc,":="); break;
        case 'y': linenoiseAddCompletion(lc,"yield"); break;
        case '=': linenoiseAddCompletion(lc,"=>"); break;
        case 'T': linenoiseAddCompletion(lc,"Type"); break;
        case 'U': linenoiseAddCompletion(lc,"Unit"); break;
            break;
        default: break;
        }
    }

    static void append_line(char *cursor, const char *end, char *line, size_t line_len) throw() {
        if((cursor + line_len + 1) > end) {
            free(line);
            fprintf(stderr, "Error: REPL buffer full.\n");
            ::exit(EXIT_FAILURE);
        }

        memcpy(cursor, line, line_len);
        cursor[line_len] = '\n';
        free(line);
    }

    reader::reader(void) throw()
        : pos(0UL)
        , is_empty(true)
        , is_done(false)
    {
        memset(&(buffer[0]), 0, array::size(buffer));
        linenoiseSetCompletionCallback(completion);
    }

    reader::~reader(void) throw() {
        pos = 0;
        is_empty = true;
        is_done = true;
    }

    void reader::reset(void) throw() {
        pos = 0;
        is_empty = true;
        memset(&(buffer[0]), 0, array::size(buffer));
    }

    bool reader::got_exit(void) const throw() {
        return is_done;
    }

    const char *reader::history(void) const throw() {
        return &(buffer[0]);
    }

    /// read up to the end of a string or the end of a line
    size_t reader::read_block(uint8_t *block, const size_t GIVEN_SIZE) const throw() {
        bool just_read(false);
        char *line(nullptr);
        size_t pos_offset(0U);

        if(is_done) {
            return 0U;
        }

        // the buffer is empty, do the first read
        if(is_empty) {
            is_empty = false;
            just_read = true;

            line = linenoise(">>> ");
            if(nullptr == line) {
                fprintf(stderr, "Error: unable to read initial line from REPL.\n");
                ::exit(EXIT_FAILURE);
            }

            if(0 == strcmp("exit", line)) {
                free(line);
                buffer[0] = '\0';
                is_done = true;
                return 0U;
            }
        }

        // we haven't just requested new info from the REPL but we should.
        if(!just_read && '\n' == buffer[pos]) {

            if(can_accept()) {
               return 0U;
            }

            just_read = true;
            pos_offset = 1U;
            line = linenoise("... ");
            if(nullptr == line) {
                fprintf(stderr, "Error: unable to read following line from REPL.\n");
                ::exit(EXIT_FAILURE);
            }
        }

        if(just_read) {
            append_line(
                const_cast<char *>(&(buffer[pos + pos_offset])),
                &array::last(buffer) - 1,
                line,
                cstring::byte_length(line)
            );
        }

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


