/*
 * type.hpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef TYPE_HPP_
#define TYPE_HPP_

#include <stdint.h>
#include <vector>

#include "xy/include/support/name_map.hpp"

namespace xy {

    struct type;
    struct named_type;
    struct alias_type;
    struct cover_type;
    struct product_type;
    struct tuple_type;
    struct record_type;
    struct sum_type;
    struct function_type;
    struct reference_type;
    struct integer_type;
    struct array_type;
    struct token_list_type;
    struct token_expression_type;

    namespace builtin {
        enum {
            NUM_BUILTIN_TYPES = 12
        };
    }

    namespace kind {
        enum type_kind_ {
            TYPE,
            NAMED,
            ALIAS,
            COVER,
            TUPLE,
            RECORD,
            SUM,
            FUNCTION,
            REFERENCE,
            INTEGER,
            ARRAY,
            TOKEN_LIST,
            TOKEN_EXPRESSION
        };
    }

    typedef enum kind::type_kind_ type_kind;
    typedef support::mapped_name type_name;
    typedef uint16_t type_id;

    // serves as a representative for multiple equivalent types.
    struct type_handle {
    public:
        uint16_t num_parents;

        // the id for this type
        type_id id;

        type **parent;
        type *original_type;

        // name of the original version of the type, if the type had a name.
        type_name original_name;
    };

    /// root type structure
    struct type {
    protected:

        friend struct named_type;
        friend struct alias_type;
        friend struct cover_type;
        friend struct product_type;
        friend struct tuple_type;
        friend struct record_type;
        friend struct sum_type;
        friend struct function_type;
        friend struct reference_type;
        friend struct integer_type;
        friend struct array_type;
        friend struct token_list_type;
        friend struct token_expression_type;

        // where this came from
        uint32_t line;
        uint32_t column;
        const char *file_name;

        // this type's handle
        type_handle *handle;

    public:
        type(void) throw();
        type(type_handle *handle_) throw();
        type(uint32_t line_, uint32_t col_, const char *file_name_,
             type_handle *handle_) throw();

        virtual ~type(void) throw();
        virtual type_kind get_kind(void) const throw();

        virtual bool is_equivalent(const type *that) const throw();
        virtual bool is_subtype(const type *that) const throw();
    };

    /// a type with a name
    struct named_type : public type {
    public:
        type_name name;

        named_type(void) throw();
        named_type(type_handle *handle_, type_name name_) throw();
        named_type(uint32_t line_, uint32_t col_, const char *file_name_,
                   type_handle *handle_, type_name name_) throw();

        virtual ~named_type(void) throw();
        virtual type_kind get_kind(void) const throw();
    };

    /// an equivalent name for an anonymous type.
    struct alias_type : public named_type {
    public:
        type *aliased_type;

        alias_type(void) throw();
        alias_type(type_handle *handle_, type_name name_, type *aliased_type_) throw();
        alias_type(uint32_t line_, uint32_t col_, const char *file_name_,
                   type_handle *handle_, type_name name_, type *aliased_type_) throw();

        virtual ~alias_type(void) throw();
        virtual type_kind get_kind(void) const throw();
    };

    /// an alias for an existing type where the alias is not equivalent to the
    /// original type
    struct cover_type : public alias_type {
    public:

        virtual ~cover_type(void) throw();
        virtual type_kind get_kind(void) const throw();
    };

    /// a product of multiple types
    struct product_type : public type {
    public:

        std::vector<type *> types;

        virtual bool is_equivalent(const type *that) const throw();
        virtual ~product_type(void) throw();

    };

    /// a structure containing values of different types.
    struct tuple_type : public product_type {
    public:

        virtual ~tuple_type(void) throw();
        virtual type_kind get_kind(void) const throw();
    };

    /// a structure containting values of different types where each value
    /// can be referenced by a unique name.
    struct record_type : public product_type {
    public:

        virtual ~record_type(void) throw();
        virtual type_kind get_kind(void) const throw();

        bool add_field(type *, type_name) throw();
        type *get_field(type_name) throw();
    };

    /// represents one of multiple types
    struct sum_type : public type {
    public:

        std::vector<type *> types;

        virtual ~sum_type(void) throw();
        virtual type_kind get_kind(void) const throw();
        virtual bool is_equivalent(const type *that) const throw();

        bool add(type *) throw();
    };

    /// Function type from_type -> to_type
    struct function_type : public type {
    public:

        virtual ~function_type(void) throw();
        virtual type_kind get_kind(void) const throw();

        type *from_type;
        type *to_type;
    };

    /// represents a reference to another type
    struct reference_type : public type {
    public:

        virtual ~reference_type(void) throw();
        virtual type_kind get_kind(void) const throw();

        type *to_type;
    };

    /// represents a fixed-width integral type
    struct integer_type : public type {
    private:
        integer_type(void) throw();
    public:

        const bool is_signed;
        const uint8_t align;
        const uint8_t width;

        integer_type(type_handle *handle_, bool is_signed_, uint8_t align_,
                     uint8_t width_) throw();
        virtual ~integer_type(void) throw();
        virtual type_kind get_kind(void) const throw();
    };

    /// represents an array of values of some type
    struct array_type : public type {
    public:

        virtual ~array_type(void) throw();
        virtual type_kind get_kind(void) const throw();

        type *of_type;
    };

    /// type representing a list of tokens
    struct token_list_type : public type {
    public:
        const bool eval_list;

        token_list_type(type_handle *handle_, bool eval_list_) throw();

        virtual ~token_list_type(void) throw();
        virtual type_kind get_kind(void) const throw();
    };

    /// type representing a list of tokens that should be able to be evaluated
    /// to some expected type
    struct token_expression_type : public type {
    public:

        type *expected_type;

        virtual ~token_expression_type(void) throw();
        virtual type_kind get_kind(void) const throw();
    };

    namespace builtin {
        extern token_list_type TOKEN_LIST;
        extern token_list_type EVALD_TOKEN_LIST;

        extern type INTEGRAL;

        extern integer_type INT_8;
        extern integer_type INT_16;
        extern integer_type INT_32;
        extern integer_type INT_64;

        extern integer_type UINT_8;
        extern integer_type UINT_16;
        extern integer_type UINT_32;
        extern integer_type UINT_64;

        extern type TYPE;
    }
}

#endif /* TYPE_HPP_ */
