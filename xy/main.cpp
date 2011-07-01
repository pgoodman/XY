/*
 * main.cpp
 *
 *  Created on: Jun 14, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

// idea: "give" and "take" type qualifiers for specifying who is responsible
//       for freeing memory.
//
//      e.g. unit free(take [ptr]);
//           [int] ptr = new int;
//           free(give ptr)

#include <cstdio>
#include <cctype>

#include "xy/include/array.hpp"
#include "xy/include/io/file.hpp"
#include "xy/include/utf8/codepoint.hpp"
#include "xy/include/utf8/decoder.hpp"

using namespace xy;

/// lex an open file
static void lex_open_file(io::file<io::read_tag> &f) {

    enum {
        BLOCK_MARGIN = 5,
        BLOCK_SIZE = 4096
    };

    uint8_t scratch[BLOCK_SIZE]{0};

    utf8::codepoint chr;
    utf8::decoder decoder;

    size_t line[2]{1, 0};
    size_t column[2]{1, 0};
    unsigned curr{0}, next{1};

    for(size_t read_size{BLOCK_SIZE};
        read_size == BLOCK_SIZE; ) {

        read_size = f.read_block<BLOCK_SIZE>(scratch);
        if(0U == read_size) {
            break;
        }

        for(size_t i(0); i < read_size; ++i) {
            if(!decoder.next_state(scratch[i], chr)) {
                continue;
            }

            line[next] = line[curr];
            column[next] = column[curr] + 1;

            if(chr.is_ascii()) {
                if('\n' == chr) {
                    line[next] += 1;
                    column[next] = 1;
                } else if('\t' == chr) {
                    column[next] += 3;
                } else if(!isgraph(chr.to_cstring()[0])) {
                    column[next] -= 1;
                }
            }

            printf("%lu %lu %s\n", line[curr], column[curr], chr.to_cstring());

            curr = next;
            next = 1 - curr;
        }
    }
}

/// lex a file by its file name
static bool lex_file(const char * const file_name) throw() {
    return io::with_open_file<io::read_tag>(file_name, lex_open_file);
}

int main(int argc, const char **argv) throw() {
    if(argc > 1 && lex_file(argv[1])) {
        printf("done parsing.\n");
    } else if(argc > 1) {
        printf("unable to open file '%s' for parsing.\n", argv[1]);
    } else {
        printf("please specify a file to parse.\n");
    }

    return 0;
}
