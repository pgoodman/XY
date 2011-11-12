/*
 * type_system.cpp
 *
 *  Created on: Aug 16, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include "xy/include/type_system.hpp"

namespace xy {

    name_type *type_system::make_name(uint16_t file, uint32_t line, uint32_t col) throw() {
        return make(name_alloc, file, line, col);
    }

    alias_type *type_system::make_alias(uint16_t file, uint32_t line, uint32_t col) throw() {
        return make(alias_alloc, file, line, col);
    }

    cover_type *type_system::make_cover(uint16_t file, uint32_t line, uint32_t col) throw() {
        return make(cover_alloc, file, line, col);
    }

    tuple_type *type_system::make_tuple(uint16_t file, uint32_t line, uint32_t col) throw() {
        return make(tuple_alloc, file, line, col);
    }

    record_type *type_system::make_record(uint16_t file, uint32_t line, uint32_t col) throw() {
        return make(record_alloc, file, line, col);
    }

    sum_type *type_system::make_sum(uint16_t file, uint32_t line, uint32_t col) throw() {
        return make(sum_alloc, file, line, col);
    }

    function_type *type_system::make_function(uint16_t file, uint32_t line, uint32_t col) throw() {
        return make(func_alloc, file, line, col);
    }

    reference_type *type_system::make_reference(uint16_t file, uint32_t line, uint32_t col) throw() {
        return make(ref_alloc, file, line, col);
    }

    array_type *type_system::make_array(uint16_t file, uint32_t line, uint32_t col) throw() {
        return make(arr_alloc, file, line, col);
    }

    token_expression_type *type_system::make_expression(uint16_t file, uint32_t line, uint32_t col) throw() {
        return make(token_expression_alloc, file, line, col);
    }
}


