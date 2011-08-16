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

using namespace xy;


int main(int argc, const char **argv) throw() {
    if(argc > 1) {
        diagnostic_context ctx(argv[1]);
        if(!parse_file(ctx, argv[1])) {
            printf("Error parsing file '%s'\n", argv[1]);
        }
        if(ctx.diag.has_message()) {
            ctx.print_diagnostics(stderr);
            return false;
        }
    } else {
        printf("please specify a file to parse.\n");
    }

    return 0;
}
