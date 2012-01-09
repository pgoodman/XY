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

#include "xy/include/color.hpp"
#include "xy/include/diagnostic_context.hpp"
#include "xy/include/parser.hpp"
#include "xy/include/ast.hpp"
#include "xy/include/symbol_table.hpp"
#include "xy/include/type_system.hpp"
#include "xy/include/cstring.hpp"

#ifndef XY_DISABLE_REPL
#   include "xy/include/repl/repl.hpp"
#   include "xy/include/repl/reader.hpp"
#endif

#include "xy/include/pass/resolve_names.hpp"

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

static void init_symbol_table(symbol_table &stab) throw() {

    /// initialize some entries
    stab["Int8"];   stab["Int16"];  stab["Int32"];  stab["Int64"];
    stab["UInt8"];  stab["UInt16"]; stab["UInt32"]; stab["UInt64"];
    stab["Bool"];   stab["Byte"];


}

#define XY_AST_INIT_ID(type) \
    printf(#type " id is %lu\n", type::static_id());

int main(int argc, char *argv[]) {

    // initialize the AST type ids
    printf("ast id is %lu\n", ast::static_id());
    XY_AST_TYPES(XY_AST_INIT_ID)

    symbol_table stab;
    type_system ts;
    init_symbol_table(stab);
    ast *tree(nullptr);

    if(argc > 1) {
        diagnostic_context ctx(argv[1], argv[1]);
        tree = parser::parse_file(ctx, stab, argv[1]);
        if(!tree || ctx.has_message()) {
            ctx.print_diagnostics(stderr);
            return false;
        }

        //resolve(tree);
        delete tree;
    } else {

#ifndef XY_DISABLE_REPL
        repl::reader byte_reader;
        diagnostic_context ctx("stdin", REPL_HISTORY_FILE_NAME);

        for(;; ctx.reset(), byte_reader.reset()) {

            D( printf("MAIN: about to parse\n"); )
            tree = parser::parse_reader(ctx, stab, byte_reader);

            if(!tree || ctx.has_message()) {
                write_repl_file(byte_reader.history());
                ctx.print_diagnostics(stderr);
                ctx.reset();
                delete_repl_file();
            }

            if(byte_reader.got_exit()) {
                if(tree) delete tree;
                break;
            }

            pass::resolve_names(ctx, stab, ts, tree);

            if(ctx.has_message()) {
                write_repl_file(byte_reader.history());
                ctx.print_diagnostics(stderr);
                delete_repl_file();
            }

            //resolve(tree);
            delete tree;
        }
#else

        fprintf(stderr, XY_F_RED "Expected a file name." XY_F_DEF "\n\n");

#endif
    }

    return 0;
}
