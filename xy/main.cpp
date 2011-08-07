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

using namespace xy;

/// lex an open file
static void tokenize_open_file(io::file<io::read_tag> &f, diagnostic_context &ctx, tokenizer &tt) {
    token tok;
    char file_name[io::REAL_PATH_MAX_LEN];
    char scratch[MAX_TOKEN_LENGTH];

    printf("token size=%lu\n", sizeof(token));
    printf("cwd='%s'\n", io::get_cwd());
    printf("file='%s'\n", io::get_real_path(ctx.top_file(), file_name));

    for(; tt.get_token(f, ctx, tok, scratch); ) {
        printf("%u:%u %s %s\n", tok.line(), tok.column(), tok.name(), scratch);
    }
}

/// lex a file by its file name
static bool tokenize_file(diagnostic_context &ctx, const char * const file_name) throw() {
    tokenizer tt;

    ctx.push_file(file_name);

    if(!io::read::open_file(file_name, tokenize_open_file, ctx, tt)) {
        ctx.diag.push(io::e_open_file, file_name);
    }

    if(ctx.diag.has_message()) {
        ctx.print_diagnostics(stderr);
        return false;
    }

    return true;
}

int main(int argc, const char **argv) throw() {
    diagnostic_context c;
    if(argc > 1) {
        tokenize_file(c, argv[1]);
        printf("done parsing.\n");
    } else if(argc > 1) {
        printf("unable to open file '%s' for parsing.\n", argv[1]);
    } else {
        printf("please specify a file to parse.\n");
    }

    return 0;
}
