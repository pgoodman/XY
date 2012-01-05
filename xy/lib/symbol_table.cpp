/*
 * symbol_table.cpp
 *
 *  Created on: Aug 9, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include "xy/include/symbol_table.hpp"
#include "xy/include/ast.hpp"

namespace xy {

    symtab::entry *symbol_table::lookup(conjunctive_statement *context, symtab::symbol name) throw() {
        symtab::entry *found(nullptr);
        for(; nullptr != context; context = context->parent_scope) {
            if(context->bound_names.find(name, found)) {
                break;
            }
        }
        return found;
    }

    symtab::entry *symbol_table::insert(conjunctive_statement *context, symtab::symbol name) throw() {

        symtab::entry *new_entry(nullptr);
        assert(nullptr != context);
        assert(!context->bound_names.find(name, new_entry));

        new_entry = entry_alloc.allocate();
        context->bound_names.force_insert(name, new_entry);

        return new_entry;
    }

    symtab::symbol symbol_table::operator[](const char *name) throw() {
        return names.add(
            name,
            static_cast<int>(cstring::byte_length(name) + 1U)
        );
    }

    const char *symbol_table::operator[](const symtab::symbol name) throw() {
        return names.find(name);
    }

}
