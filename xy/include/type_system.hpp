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

        //support::block_allocator<name_type> name_alloc;
        //support::block_allocator<alias_type> alias_alloc;
        //support::block_allocator<cover_type> cover_alloc;
        support::block_allocator<product_type> product_alloc;
        support::block_allocator<record_type> record_alloc;
        support::block_allocator<sum_type> sum_alloc;
        support::block_allocator<arrow_type> func_alloc;
        support::block_allocator<reference_type> ref_alloc;
        support::block_allocator<array_type> arr_alloc;
        support::block_allocator<type_variable_type> type_var_alloc;

        //support::block_allocator<type_handle> handle_alloc;

        //type_id next_type_id;

        /// mapping of names to name types; these are for "unit" types that
        /// contain nothing, e.g. an Empty type for a BinaryTree.
        // std::map<type_name, name_type *> names;

        /// the dictionary of all types; maps type id -> type *
        //std::vector<type *> types;

        /// hash table representing all product types
        /*
        std::tr1::unordered_map<
            std::vector<type *> *,
            product_type *
        > product_types;*/

        /// a list of types that we are adding into some tuple/record/
        /// function type.
        //std::vector<type *> params;
        //std::vector<type_name> param_names;

        template <typename T>
        inline T *make(support::block_allocator<T> &allocator) throw() {
            T *tt(allocator.allocate());
            return tt;
        }

    public:

        name_type *make_name(void) throw();
        alias_type *make_alias(void) throw();
        cover_type *make_cover(void) throw();
        product_type *make_product(void) throw();
        record_type *make_record(void) throw();
        sum_type *make_sum(void) throw();
        arrow_type *make_arrow(void) throw();
        reference_type *make_reference(void) throw();
        array_type *make_array(void) throw();
        type_variable_type *make_type_var(void) throw();
    };

}

#endif /* XY_TYPE_SYSTEM_HPP_ */
