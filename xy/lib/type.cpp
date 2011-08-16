/*
 * type.cpp
 *
 *  Created on: Aug 13, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include "xy/include/type.hpp"

namespace xy {

    type::~type(void) { }
    named_type::~named_type(void) { }
    alias_type::~alias_type(void) { }
    cover_type::~cover_type(void) { }
    product_type::~product_type(void) { }
    tuple_type::~tuple_type(void) { }
    record_type::~record_type(void) { }
    sum_type::~sum_type(void) { }
    function_type::~function_type(void) { }
    reference_type::~reference_type(void) { }
    integer_type::~integer_type(void) { }
    array_type::~array_type(void) { }
    token_list_type::~token_list_type(void) { }
    token_expression_type::~token_expression_type(void) { }

    //--------------------------------------------------------------------------

    type::type(void) throw()
        : line(0)
        , column(0)
        , file_name(nullptr)
        , handle(nullptr)
    { }

    type::type(type_handle *handle_) throw()
        : line(0)
        , column(0)
        , file_name(nullptr)
        , handle(handle_)
    { }

    type::type(uint32_t line_, uint32_t column_, const char *file_name_,
               type_handle *handle_) throw()
        : line(line_)
        , column(column_)
        , file_name(file_name_)
        , handle(handle_)
    { }

    type_kind type::get_kind(void) const throw() {
        return kind::TYPE;
    }

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
            return reinterpret_cast<const named_type *>(this)->name
                == reinterpret_cast<const named_type *>(that)->name;

        case kind::REFERENCE:
            return reinterpret_cast<const reference_type *>(this)->to_type->is_equivalent(
                reinterpret_cast<const reference_type *>(that)->to_type
            );

        case kind::TOKEN_EXPRESSION:
            return reinterpret_cast<const token_expression_type *>(this)->expected_type->is_equivalent(
                reinterpret_cast<const token_expression_type *>(that)->expected_type
            );

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

    named_type::named_type(void) throw()
        : type()
        , name()
    { }

    named_type::named_type(type_handle *handle_, type_name name_) throw()
        : type(handle_)
        , name(name_)
    { }

    named_type::named_type(uint32_t line_, uint32_t col_, const char *file_name_,
                           type_handle *handle_, type_name name_) throw()
        : type(line_, col_, file_name_, handle_)
        , name(name_)
    { }

    type_kind named_type::get_kind(void) const throw() {
        return kind::NAMED;
    }

    //--------------------------------------------------------------------------

    alias_type::alias_type(void) throw()
        : named_type()
        , aliased_type(nullptr)
    { }

    alias_type::alias_type(type_handle *handle_, type_name name_, type *aliased_type_) throw()
        : named_type(handle_, name_)
        , aliased_type(aliased_type_)
    { }

    alias_type::alias_type(uint32_t line_, uint32_t col_, const char *file_name_,
                           type_handle *handle_, type_name name_, type *aliased_type_) throw()
        : named_type(line_, col_, file_name_, handle_, name_)
        , aliased_type(aliased_type_)
    { }

    //--------------------------------------------------------------------------

    /// type equivalence for product types (tuples, records)
    bool product_type::is_equivalent(const type *that) const throw() {
        if(this == that) {
            return true;
        }

        const type_kind that_kind(that->get_kind());

        if(nullptr == that
        || (kind::TUPLE != that_kind && kind::RECORD != that_kind)) {
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
        return kind::TUPLE;
    }

    //--------------------------------------------------------------------------

    type_kind record_type::get_kind(void) const throw() {
        return kind::RECORD;
    }

    //--------------------------------------------------------------------------

    type_kind sum_type::get_kind(void) const throw() {
        return kind::SUM;
    }

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

    //--------------------------------------------------------------------------

    type_kind function_type::get_kind(void) const throw() {
        return kind::FUNCTION;
    }

    //--------------------------------------------------------------------------

    type_kind reference_type::get_kind(void) const throw() {
        return kind::REFERENCE;
    }

    //--------------------------------------------------------------------------

    integer_type::integer_type(type_handle *handle_, bool is_signed_,
                               uint8_t align_, uint8_t width_) throw()
        : type(handle_)
        , is_signed(is_signed_)
        , align(align_)
        , width(width_)
    { }

    type_kind integer_type::get_kind(void) const throw() {
        return kind::INTEGER;
    }

    //--------------------------------------------------------------------------

    type_kind array_type::get_kind(void) const throw() {
        return kind::ARRAY;
    }

    //--------------------------------------------------------------------------

    token_list_type::token_list_type(type_handle *handle_, bool eval_list_) throw()
        : type(handle_)
        , eval_list(eval_list_)
    { }

    type_kind token_list_type::get_kind(void) const throw() {
        return kind::TOKEN_LIST;
    }

    //--------------------------------------------------------------------------

    type_kind token_expression_type::get_kind(void) const throw() {
        return kind::TOKEN_EXPRESSION;
    }

    //--------------------------------------------------------------------------

    namespace builtin {
        namespace {
            static type_handle ROOT{1, 1, reinterpret_cast<type **>(&TYPE), &TYPE, type_name()};
            static type_handle TOKEN_LIST_HANDLE{1, 2, reinterpret_cast<type **>(&TYPE), &TOKEN_LIST, type_name()};
            static type_handle EVALD_TOKEN_LIST_HANDLE{1, 3, reinterpret_cast<type **>(&TYPE), &EVALD_TOKEN_LIST, type_name()};
            static type_handle INTEGRAL_HANDLE{1, 4, reinterpret_cast<type **>(&TYPE), &INTEGRAL, type_name()};

            static type_handle INT_64_HANDLE{1, 5, reinterpret_cast<type **>(&INTEGRAL), &INT_64, type_name()};
            static type_handle UINT_64_HANDLE{1, 6, reinterpret_cast<type **>(&INTEGRAL), &UINT_64, type_name()};

            static type_handle INT_32_HANDLE{1, 7, reinterpret_cast<type **>(&INT_64), &INT_32, type_name()};
            static type *uint32_parents[]{&UINT_64, &INT_64};
            static type_handle UINT_32_HANDLE{1, 8, uint32_parents, &UINT_32, type_name()};

            static type_handle INT_16_HANDLE{1, 9, reinterpret_cast<type **>(&INT_32), &INT_16, type_name()};
            static type *uint16_parents[]{&UINT_32, &INT_32};
            static type_handle UINT_16_HANDLE{1, 10, uint16_parents, &UINT_16, type_name()};

            static type_handle INT_8_HANDLE{1, 11, reinterpret_cast<type **>(&INT_16), &INT_8, type_name()};
            static type *uint8_parents[]{&UINT_16, &INT_16};
            static type_handle UINT_8_HANDLE{1, 12, uint8_parents, &UINT_8, type_name()};
        }

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
    }

}
