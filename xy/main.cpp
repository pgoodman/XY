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
#include "xy/include/parser.hpp"
#include "xy/include/cstring.hpp"

#include "xy/include/repl/repl.hpp"
#include "xy/include/repl/reader.hpp"

#define D(x)

using namespace xy;

static const char *REPL_HISTORY_FILE_NAME(".xy.repl");

static void write_repl_file(const char *buffer) throw() {
    FILE *fp(fopen(REPL_HISTORY_FILE_NAME, "w"));
    if(nullptr == fp) {
        printf("failed to open repl history file.\n");
        return;
    }

    fwrite(buffer, 1U, cstring::byte_length(buffer) + 1U, fp);
    fclose(fp);
}

static void delete_repl_file(void) throw() {
    remove(REPL_HISTORY_FILE_NAME);
}

int main(int argc, char *argv[]) {
    if(argc > 1) {
        diagnostic_context ctx(argv[1], argv[1]);
        if(!parser::parse_file(ctx, argv[1])) {
            fprintf(stderr, "Error parsing file '%s'\n", argv[1]);
        }
        if(ctx.has_message()) {
            ctx.print_diagnostics(stderr);
            return false;
        }
    } else {

        repl::reader byte_reader;
        bool parse_was_good(false);
        diagnostic_context ctx("stdin", REPL_HISTORY_FILE_NAME);

        for(;; ctx.reset(), byte_reader.reset()) {

            D( printf("MAIN: about to parse\n"); )
            parse_was_good = parser::parse_reader(ctx, byte_reader);

            if(ctx.has_message()) {
                write_repl_file(byte_reader.history());
                ctx.print_diagnostics(stderr);
                delete_repl_file();
            }

            if(byte_reader.got_exit()) {
                break;
            }

            if(parse_was_good) {
                // TODO
            }
        }
    }

    return 0;
}
