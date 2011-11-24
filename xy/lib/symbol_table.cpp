/*
 * symbol_table.cpp
 *
 *  Created on: Aug 9, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include "xy/include/symbol_table.hpp"

namespace xy {

    void symbol_table::push_context(void) throw() {

    }

    void symbol_table::pop_context(void) throw() {

    }

    support::mapped_name symbol_table::operator[](const char *name) throw() {
        return names.map_name(name);
    }

    const char *symbol_table::operator[](const support::mapped_name name) throw() {
        return names.unmap_name(name);
    }

}
