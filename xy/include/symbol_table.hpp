/*
 * symbol_table.hpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef SYMBOL_TABLE_HPP_
#define SYMBOL_TABLE_HPP_

#include "xy/include/support/hash_array_set.hpp"
#include "xy/include/support/block_allocator.hpp"
#include "xy/include/front_end.hpp"

namespace xy {

    /// manages symbols
    class symbol_table {
    private:

        support::hash_array_set<char> names;
        support::block_allocator<symtab::entry, 256U> entry_alloc;
        //support::block_allocator<symtab::context> context_alloc;

    public:

        symtab::symbol operator[](const char *name) throw();
        const char *operator[](symtab::symbol name) throw();

        /// lookup a symbol in some context based on the AST
        symtab::entry *lookup(conjunctive_statement *context, symtab::symbol name) throw();

        /// insert a symbol into the symbol table
        symtab::entry *insert(conjunctive_statement *context, symtab::symbol name) throw();
    };

}

#endif /* SYMBOL_TABLE_HPP_ */
