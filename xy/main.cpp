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
#include <vector>
#include <cassert>

#include "xy/include/diagnostic_context.hpp"
#include "xy/include/token.hpp"
#include "xy/include/tokenizer.hpp"
#include "xy/include/io/file.hpp"
#include "xy/include/io/message.hpp"
#include "xy/include/io/cwd.hpp"
#include "xy/include/io/real_path.hpp"
#include "xy/include/utf8/codepoint.hpp"
#include "xy/include/parser.hpp"

#include "xy/include/repl/repl.hpp"
#include "xy/include/repl/reader.hpp"

using namespace xy;

int main(int argc, char *argv[]) {
    if(argc > 1) {
        diagnostic_context ctx(argv[1]);
        if(!parser::parse_file(ctx, argv[1])) {
            fprintf(stderr, "Error parsing file '%s'\n", argv[1]);
        }
        if(ctx.has_message()) {
            ctx.print_diagnostics(stderr);
            return false;
        }
    } else {

        char *buffer = repl::init();
        repl::reader byte_reader(buffer);
        diagnostic_context ctx("stdin");

        for(; repl::check(); ) {
            ctx.reset();

            if(!parser::parse_reader(ctx, byte_reader)) {
                fprintf(stderr, "Error parsing.\n");
            }

            if(ctx.has_message()) {
                ctx.print_diagnostics(stderr);
            }

            //printf("main loop yielding\n");
            byte_reader.reset();
            repl::read::yield();
        }



            /*
            while(repl::READ_MORE == repl::eval::yield()) {

                line = linenoise("... ");
                if(nullptr == line) {
                    break;
                }

                line_len = cstring::byte_length(line) + 1;
                if((cursor + line_len) > end_of_buffer) {
                    free(line);
                    fprintf(stderr, "Error: REPL buffer full.\n");
                    break;
                }

                // extend the buffer in place
                memcpy(cursor, line, line_len);
                cursor[line_len - 1] = '\n';
                free(line);
                line = nullptr;
            }
            */

            /*
            if(!parser::parse_buffer(ctx, line)) {
                fprintf(stderr, "Error parsing.\n");
            }

            if(ctx.has_message()) {
                ctx.print_diagnostics(stderr);
            }
            */

        //}

        repl::exit();
    }

    return 0;
}
