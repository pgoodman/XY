/*
 * symbol_table.hpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef SYMBOL_TABLE_HPP_
#define SYMBOL_TABLE_HPP_

#include "xy/include/support/name_map.hpp"
#include "xy/include/support/block_allocator.hpp"

namespace xy {

    struct type;
    struct parse_tree;
    class symbol_table;

    namespace stab {

        typedef enum {
            ST_VAR_NAME,
            ST_TYPE_NAME
        } entry_type;

        // forward declarations
        class entry;
        class context;

        // an entry in the symbol table
        class entry {
        private:

            friend class symbol_table;

            support::mapped_name name;
            entry_type name_type;
            type *type;
        };

        // a set of entries in the symbol table.
        class context {
        private:

            friend class symbol_table;
        };
    }

    class symbol_table {
    private:

        support::name_map names;
        support::block_allocator<stab::entry, 1024U> entry_alloc;
        support::block_allocator<stab::context> context_alloc;

    public:

        void push_context(void) throw();

        support::mapped_name operator[](const char *name) throw();
        const char *operator[](support::mapped_name name) throw();

        void pop_context(void) throw();

    };

}

#endif /* SYMBOL_TABLE_HPP_ */
