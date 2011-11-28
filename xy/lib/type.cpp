/*
 * type.cpp
 *
 *  Created on: Aug 13, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include "xy/include/type.hpp"

namespace xy {

    type::~type(void) throw() { }
    name_type::~name_type(void) throw() { }
    alias_type::~alias_type(void) throw() { }
    cover_type::~cover_type(void) throw() { }
    product_type::~product_type(void) throw() { }
    tuple_type::~tuple_type(void) throw() { }
    record_type::~record_type(void) throw() { }
    sum_type::~sum_type(void) throw() { }
    function_type::~function_type(void) throw() { }
    reference_type::~reference_type(void) throw() { }
    integer_type::~integer_type(void) throw() { }
    array_type::~array_type(void) throw() { }
    token_list_type::~token_list_type(void) throw() { }
    token_expression_type::~token_expression_type(void) throw() { }

    //--------------------------------------------------------------------------

    type::type(void) throw()
        : line(0)
        , column(0)
        , file_id(0)
        , is_new(true)
        , rep()
    { }

#if 0
    type::type(void) throw()
        : line(0)
        , column(0)
        , file_id(0)
        , handle(nullptr)
    { }

    type::type(type_handle *handle_) throw()
        : line(0)
        , column(0)
        , file_id(0)
        , handle(handle_)
    { }

    type::type(uint32_t line_, uint32_t column_, uint16_t file_id_,
               type_handle *handle_) throw()
        : line(line_)
        , column(column_)
        , file_id(file_id_)
        , handle(handle_)
    { }
#endif

    type_kind type::get_kind(void) const throw() {
        return kind::TYPE;
    }

#if 0
    /// type equivalence
    bool type::is_equivalent(const type *that) const throw() {
        if(this == that) {
            return true;

        }

        const type_kind this_kind(this->get_kind());

        if(nullptr == that || this_kind != that->get_kind()) {
            return false;

        } else if(nullptr != this->handle && nullptr != that->handle) {
            return this->handle == that->handle;
        }

        //

        switch(this_kind) {
        case kind::TYPE: return true;

        case kind::COVER:
        case kind::ALIAS:
            return reinterpret_cast<const alias_type *>(this)->aliased_type->is_equivalent(
                reinterpret_cast<const alias_type *>(that)->aliased_type
            );

        case kind::ARRAY:
            return reinterpret_cast<const array_type *>(this)->of_type->is_equivalent(
                reinterpret_cast<const array_type *>(that)->of_type
            );

        case kind::FUNCTION: {
            const function_type *this_p(reinterpret_cast<const function_type *>(this));
            const function_type *that_p(reinterpret_cast<const function_type *>(that));
            return this_p->from_type->is_equivalent(that_p->from_type)
                && this_p->to_type->is_equivalent(that_p->to_type);
        }
        case kind::INTEGER: {
            const integer_type *this_p(reinterpret_cast<const integer_type *>(this));
            const integer_type *that_p(reinterpret_cast<const integer_type *>(that));
            return this_p->align == that_p->align
                && this_p->width == that_p->width
                && this_p->is_signed == that_p->is_signed;
        }

        case kind::NAMED:
            return reinterpret_cast<const name_type *>(this)->name
                == reinterpret_cast<const name_type *>(that)->name;

        case kind::REFERENCE:
            return reinterpret_cast<const reference_type *>(this)->to_type->is_equivalent(
                reinterpret_cast<const reference_type *>(that)->to_type
            );

        case kind::TOKEN_EXPRESSION:
            return reinterpret_cast<const token_expression_type *>(this)->expected_type->is_equivalent(
                reinterpret_cast<const token_expression_type *>(that)->expected_type
            );

        case kind::META_TYPE:
            return true;

        default:
            break;
        }

        return false;
    }


    bool type::is_subtype(const type *that) const throw() {
        if(this == that) {
            return true;
        } else if(nullptr == that || this->get_kind() != that->get_kind()) {
            return false;
        }
        return false;
    }

    //--------------------------------------------------------------------------
    kind_equivalent_meta_type::kind_equivalent_meta_type(type_kind kind_) throw()
        : type()
        , kind(kind_)
    { }
#endif

    kind_equivalent_meta_type::~kind_equivalent_meta_type(void) throw() { }
#if 0
    bool kind_equivalent_meta_type::is_subtype(const type *) const throw() {
        return false;
    }

    //--------------------------------------------------------------------------

    name_type::name_type(void) throw()
        : type()
        , name()
    { }

    name_type::name_type(type_handle *handle_, type_name name_) throw()
        : type(handle_)
        , name(name_)
    { }

    name_type::name_type(uint32_t line_, uint32_t col_, uint16_t file_id_,
                           type_handle *handle_, type_name name_) throw()
        : type(line_, col_, file_id_, handle_)
        , name(name_)
    { }
#endif
    type_kind name_type::get_kind(void) const throw() {
        return kind::NAMED;
    }

    //--------------------------------------------------------------------------
#if 0
    alias_type::alias_type(void) throw()
        : name_type()
        , aliased_type(nullptr)
    { }

    alias_type::alias_type(type_handle *handle_, type_name name_, type *aliased_type_) throw()
        : name_type(handle_, name_)
        , aliased_type(aliased_type_)
    { }

    alias_type::alias_type(uint32_t line_, uint32_t col_, uint16_t file_id_,
                           type_handle *handle_, type_name name_, type *aliased_type_) throw()
        : name_type(line_, col_, file_id_, handle_, name_)
        , aliased_type(aliased_type_)
    { }
#endif
    //--------------------------------------------------------------------------
#if 0
    /// type equivalence for product types (tuples, records)
    bool product_type::is_equivalent(const type *that) const throw() {
        if(this == that) {
            return true;
        }

        const type_kind that_kind(that->get_kind());

        if(nullptr == that
        || (kind::PRODUCT != that_kind && kind::PRODUCT != that_kind)) {
            return false;

        } else if(nullptr != this->handle && nullptr != that->handle) {
            return this->handle == that->handle;
        }

        const product_type *that_p(reinterpret_cast<const product_type *>(that));
        const size_t num_types(that_p->types.size());

        if(num_types != this->types.size()) {
            return false;
        }

        for(size_t i(0); i < num_types; ++i) {
            if(!this->types[i]->is_equivalent(that_p->types[i])) {
                return false;
            }
        }

        return true;
    }
#endif
    //--------------------------------------------------------------------------

    type_kind alias_type::get_kind(void) const throw() {
        return kind::ALIAS;
    }

    //--------------------------------------------------------------------------

    type_kind cover_type::get_kind(void) const throw() {
        return kind::COVER;
    }

    //--------------------------------------------------------------------------

    type_kind tuple_type::get_kind(void) const throw() {
        return kind::PRODUCT;
    }

    //--------------------------------------------------------------------------

    type_kind record_type::get_kind(void) const throw() {
        return kind::PRODUCT;
    }

    //--------------------------------------------------------------------------

    type_kind sum_type::get_kind(void) const throw() {
        return kind::SUM;
    }

#if 0
    /// type equivalence for sum types (e.g. a C union)
    bool sum_type::is_equivalent(const type *that) const throw() {
        if(this == that) {
            return true;
        }

        const type_kind that_kind(that->get_kind());

        if(nullptr == that || kind::SUM != that_kind) {
            return false;

        } else if(nullptr != this->handle && nullptr != that->handle) {
            return this->handle == that->handle;
        }

        const sum_type *that_p(reinterpret_cast<const sum_type *>(that));
        const size_t num_types(that_p->types.size());

        if(num_types != this->types.size()) {
            return false;
        }

        // assumes that all types in the sum are unique
        for(size_t i(0); i < num_types; ++i) {
            for(size_t j(0); j < num_types; ++j) {
                if(!this->types[i]->is_equivalent(that_p->types[j])) {
                    return false;
                }
            }
        }

        return true;
    }
#endif

    //--------------------------------------------------------------------------

    type_kind function_type::get_kind(void) const throw() {
        return kind::FUNCTION;
    }

    //--------------------------------------------------------------------------

    type_kind reference_type::get_kind(void) const throw() {
        return kind::REFERENCE;
    }

    //--------------------------------------------------------------------------
#if 0
    integer_type::integer_type(type_handle *handle_, bool is_signed_,
                               uint8_t align_, uint8_t width_) throw()
        : type(handle_)
        , is_signed(is_signed_)
        , align(align_)
        , width(width_)
    { }
#endif

    type_kind integer_type::get_kind(void) const throw() {
        return kind::INTEGER;
    }

    //--------------------------------------------------------------------------

    type_kind array_type::get_kind(void) const throw() {
        return kind::ARRAY;
    }

    //--------------------------------------------------------------------------
#if 0
    token_list_type::token_list_type(type_handle *handle_, bool eval_list_) throw()
        : type(handle_)
        , eval_list(eval_list_)
    { }
#endif
    type_kind token_list_type::get_kind(void) const throw() {
        return kind::TOKEN_LIST;
    }

    //--------------------------------------------------------------------------

    type_kind token_expression_type::get_kind(void) const throw() {
        return kind::TOKEN_EXPRESSION;
    }

    //--------------------------------------------------------------------------
#if 0
    namespace builtin {
#if 0
        namespace {
            static type_handle ROOT{1, support::small_set<type *>(&TYPE), support::small_set<type *>(&TYPE, &TOKEN_LIST, &EVALD_TOKEN_LIST, &INTEGRAL), &TYPE, type_name()};
            static type_handle TOKEN_LIST_HANDLE{2, support::small_set<type *>(&TYPE), support::small_set<type *>(), &TOKEN_LIST, type_name()};
            static type_handle EVALD_TOKEN_LIST_HANDLE{3, support::small_set<type *>(&TYPE), support::small_set<type *>(), &EVALD_TOKEN_LIST, type_name()};
            static type_handle INTEGRAL_HANDLE{4, support::small_set<type *>(&TYPE), support::small_set<type *>(&INT_64, &UINT_64), &INTEGRAL, type_name()};

            static type_handle INT_64_HANDLE{5, support::small_set<type *>(&INTEGRAL), support::small_set<type *>(&INT_32, &UINT_32), &INT_64, type_name()};
            static type_handle UINT_64_HANDLE{6, support::small_set<type *>(&INTEGRAL), support::small_set<type *>(&UINT_32), &UINT_64, type_name()};

            static type_handle INT_32_HANDLE{7, support::small_set<type *>(&INT_64), support::small_set<type *>(&INT_16, &UINT_16), &INT_32, type_name()};
            static type_handle UINT_32_HANDLE{8, support::small_set<type *>(&INT_64, &UINT_64), support::small_set<type *>(&UINT_16), &UINT_32, type_name()};

            static type_handle INT_16_HANDLE{9, support::small_set<type *>(&INT_32), support::small_set<type *>(&INT_8, &UINT_8), &INT_16, type_name()};
            static type_handle UINT_16_HANDLE{10, support::small_set<type *>(&INT_32, &UINT_32), support::small_set<type *>(&UINT_8), &UINT_16, type_name()};

            static type_handle INT_8_HANDLE{11, support::small_set<type *>(&INT_16), support::small_set<type *>(), &INT_8, type_name()};
            static type_handle UINT_8_HANDLE{12, support::small_set<type *>(&INT_16, &UINT_16), support::small_set<type *>(), &UINT_8, type_name()};
        }
#endif
#if 0
        type TYPE(&ROOT);

        token_list_type TOKEN_LIST(&TOKEN_LIST_HANDLE, false);
        token_list_type EVALD_TOKEN_LIST(&EVALD_TOKEN_LIST_HANDLE, true);

        type INTEGRAL(&INTEGRAL_HANDLE);

        integer_type INT_8(&INT_8_HANDLE, true, 1, 1);
        integer_type INT_16(&INT_16_HANDLE, true, 2, 2);
        integer_type INT_32(&INT_32_HANDLE, true, 4, 4);
        integer_type INT_64(&INT_64_HANDLE, true, 8, 4);

        integer_type UINT_8(&UINT_8_HANDLE, false, 1, 1);
        integer_type UINT_16(&UINT_16_HANDLE, false, 2, 2);
        integer_type UINT_32(&UINT_32_HANDLE, false, 4, 4);
        integer_type UINT_64(&UINT_64_HANDLE, false, 8, 4);
#endif
    }
#endif
}
