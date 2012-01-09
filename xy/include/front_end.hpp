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

    // tokenizer
    class token;

    // ast forward declarations
    struct ast;
    struct ast_visitor;
    struct statement;
    struct conjunctive_statement;
    struct expression;
    struct type_declaration;
    struct definition_statement;

    // type forward declarations
    class type_system;
    struct type;
    struct name_type;
    struct alias_type;
    struct cover_type;
    struct product_type;
    //struct tuple_type;
    //struct record_type;
    struct sum_type;
    struct arrow_type;
    struct reference_type;
    struct integer_type;
    struct array_type;
    struct token_list_type;
    struct token_expression_type;

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
            TEMPLATE_TYPE_VARIABLE,
            TEMPLATE_FUNCTION,
            FUNCTION,

            ARROW_TYPE,
            SUM_TYPE,
            PRODUCT_TYPE,
            ARRAY_TYPE,
            REF_TYPE,

            VARIABLE,
            TEMPLATE_VARIABLE,

            NONE
        };

        /// an entry in the symbol table
        struct entry {
        public:
            entry_type name_type;
            //type *type_;
            union {
                //name_type *name;
                product_type *product;
                sum_type *sum;
                arrow_type *arrow;
                reference_type *ref;
                array_type *array;
                type *base;
            } type;

            union {
                definition_statement *def;
                expression *expr;
                statement *stmt;
                type_declaration *type_decl;
                token *location; // mostly unsafe
            } origin;
            conjunctive_statement *scope;
        };
    }

}


#endif /* XY_FRONT_END_HPP_ */
