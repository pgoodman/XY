/*
 * type_system.hpp
 *
 *  Created on: Aug 14, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_TYPE_SYSTEM_HPP_
#define XY_TYPE_SYSTEM_HPP_

#include <map>
#include <vector>
//#include <tr1/unordered_map>

#include "xy/include/type.hpp"
#include "xy/include/support/block_allocator.hpp"

namespace xy {

    class type_system {
    private:

        support::block_allocator<name_type> name_alloc;
        support::block_allocator<alias_type> alias_alloc;
        support::block_allocator<cover_type> cover_alloc;
        support::block_allocator<tuple_type> tuple_alloc;
        support::block_allocator<record_type> record_alloc;
        support::block_allocator<sum_type> sum_alloc;
        support::block_allocator<function_type> func_alloc;
        support::block_allocator<reference_type> ref_alloc;
        support::block_allocator<array_type> arr_alloc;
        support::block_allocator<token_expression_type> token_expression_alloc;

        //support::block_allocator<type_handle> handle_alloc;

        type_id next_type_id;

        /// mapping of names to name types; these are for "unit" types that
        /// contain nothing, e.g. an Empty type for a BinaryTree.
        std::map<type_name, name_type *> names;

        /// the dictionary of all types; maps type id -> type *
        std::vector<type *> types;

        /// hash table representing all product types
        /*
        std::tr1::unordered_map<
            std::vector<type *> *,
            product_type *
        > product_types;*/

        /// a list of types that we are adding into some tuple/record/
        /// function type.
        std::vector<type *> params;
        std::vector<type_name> param_names;

        template <typename T>
        T *make(support::block_allocator<T> &allocator, uint16_t file, uint32_t line, uint32_t col) throw() {
            T *type(allocator.allocate());
            type->file_id = file;
            type->line = line;
            type->column = col;
            return type;
        }

    public:

        name_type *make_name(uint16_t file, uint32_t line, uint32_t col) throw();
        alias_type *make_alias(uint16_t file, uint32_t line, uint32_t col) throw();
        cover_type *make_cover(uint16_t file, uint32_t line, uint32_t col) throw();
        tuple_type *make_tuple(uint16_t file, uint32_t line, uint32_t col) throw();
        record_type *make_record(uint16_t file, uint32_t line, uint32_t col) throw();
        sum_type *make_sum(uint16_t file, uint32_t line, uint32_t col) throw();
        function_type *make_function(uint16_t file, uint32_t line, uint32_t col) throw();
        reference_type *make_reference(uint16_t file, uint32_t line, uint32_t col) throw();
        array_type *make_array(uint16_t file, uint32_t line, uint32_t col) throw();
        token_expression_type *make_expression(uint16_t file, uint32_t line, uint32_t col) throw();
    };

}

#endif /* XY_TYPE_SYSTEM_HPP_ */
