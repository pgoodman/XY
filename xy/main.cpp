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

#include "xy/deps/linenoise/linenoise.h"

using namespace xy;

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
    default: break;
    }
}

int main(int argc, const char **argv) throw() {
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
        char *line(nullptr);
        linenoiseSetCompletionCallback(completion);
        while(nullptr != (line = linenoise(">>> "))) {
            diagnostic_context ctx("stdin");
            if(!parser::parse_buffer(ctx, line)) {
                fprintf(stderr, "Error parsing.\n");
            }

            if(ctx.has_message()) {
                ctx.print_diagnostics(stderr);
            }
        }
    }

    return 0;
}
