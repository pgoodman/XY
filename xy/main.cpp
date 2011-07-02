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

#include "xy/include/lexer.hpp"
#include "xy/include/io/file.hpp"
#include "xy/include/io/message.hpp"
#include "xy/include/utf8/codepoint.hpp"

using namespace xy;

/// lex an open file
static void lex_open_file(io::file<io::read_tag> &f, io::message_queue &mq, lexer &ll) {
    utf8::codepoint cp;
    int chr;
    for(; ll.get_codepoint(f, mq, cp); ) {
        if(cp.is_null()) {
            break;
        }

        if(!cp.is_ascii()) {
            (void) mq;
            //mq.push(io::e_mb_not_in_string, );
        }

        chr = cp.to_cstring()[0];

        printf("%lu %lu %s\n", ll.line(), ll.column(), cp.to_cstring());
    }
}

/// lex a file by its file name
static bool lex_file(const char * const file_name) throw() {
    io::message_queue mq;
    lexer ll;

    mq.push(io::test, 10, 'a', "hello world!!!");
    mq.print_all(stderr);

    return io::with_open_file<io::read_tag>(file_name, lex_open_file, mq, ll);
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
