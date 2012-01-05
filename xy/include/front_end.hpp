/*
 * front_end.hpp
 *
 *  Created on: Jan 4, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_FRONT_END_HPP_
#define XY_FRONT_END_HPP_

#include "xy/include/support/hash_array_set.hpp"

namespace xy {

    // ast forward declarations
    struct ast;
    struct ast_visitor;
    struct statement;
    struct conjunctive_statement;

    // type forward declarations
    struct type;

    // diagnostics
    class diagnostic_context;

    // symbol table stuff
    class symbol_table;

    namespace symtab {

        /// a name in a symbol table
        typedef support::hash_array_set_handle symbol;

        /// type of a symbol table entry
        enum entry_type : uint8_t {
            TEMPLATE_TYPE_FUNCTION,
            TEMPLATE_TYPE_RECORD,
            TEMPLATE_TYPE_UNION,
            TEMPLATE_FUNCTION,
            FUNCTION,

            ARROW_TYPE,
            SUM_TYPE,
            PRODUCT_TYPE,
            ARRAY_TYPE,
            REF_TYPE,

            VARIABLE,

            NONE
        };

        /// an entry in the symbol table
        struct entry {
        public:
            entry_type name_type;
            type *type_;
            ast *origin;
        };
    }

}


#endif /* XY_FRONT_END_HPP_ */
