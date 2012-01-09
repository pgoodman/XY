/*
 * type_system.cpp
 *
 *  Created on: Aug 16, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include "xy/include/type_system.hpp"

namespace xy {
#if 0
    name_type *type_system::make_name(void) throw() {
        return make(name_alloc);
    }

    alias_type *type_system::make_alias(void) throw() {
        return make(alias_alloc);
    }

    cover_type *type_system::make_cover(void) throw() {
        return make(cover_alloc);
    }
#endif
    product_type *type_system::make_product(void) throw() {
        return make(product_alloc);
    }

    record_type *type_system::make_record(void) throw() {
        return make(record_alloc);
    }

    sum_type *type_system::make_sum(void) throw() {
        return make(sum_alloc);
    }

    arrow_type *type_system::make_arrow(void) throw() {
        return make(func_alloc);
    }

    reference_type *type_system::make_reference(void) throw() {
        return make(ref_alloc);
    }

    array_type *type_system::make_array(void) throw() {
        return make(arr_alloc);
    }

    type_variable_type *type_system::make_type_var(void) throw() {
        return make(type_var_alloc);
    }
}


