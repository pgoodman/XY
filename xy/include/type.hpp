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
#include <functional>
#include <utility>

#include "xy/include/front_end.hpp"
#include "xy/include/support/small_set.hpp"
#include "xy/include/support/disjoint_set.hpp"

namespace xy {

    class type_system;

    struct type;
    struct name_type;
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
    typedef uint16_t type_id;

    /// represents a placeholder for a type that is equivalent with any type of
    /// the same kind
    struct kind_equivalent_meta_type;

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
            PRODUCT,
            SUM,
            FUNCTION,
            REFERENCE,
            INTEGER,
            ARRAY,
            TOKEN_LIST,
            TOKEN_EXPRESSION,
            META_TYPE
        };
    }

    typedef enum kind::type_kind_ type_kind;
    typedef symtab::symbol type_name;
    typedef uint16_t type_id;

#if 0
    // serves as a representative for multiple equivalent types.
    struct type_handle {
    public:

        /*
        // the id for this type
        type_id id;

        support::small_set<type *> super_types;
        support::small_set<type *> sub_types;
        */

        support::disjoint_set<type> rep;

        type *original_type;

        // name of the original version of the type, if the type had a name.
        type_name original_name;
    };
#endif

    /// root type structure
    struct type {
    protected:

        friend class type_system;

        friend struct name_type;
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
        friend struct kind_equivalent_meta_type;

        // where this came from
        uint32_t line;
        uint32_t column;
        uint16_t file_id;
        bool is_new;

        // this type's handle
        //type_handle *handle;

        support::disjoint_set<type> rep;

    public:
        type(void) throw();

        //type(uint32_t line_=0, uint32_t col_=0, uint16_t file_id_=0) throw();

        //type(type_handle *handle_) throw();
        //type(uint32_t line_, uint32_t col_, uint16_t file_id_,
        //     type_handle *handle_) throw();

        virtual ~type(void) throw();
        virtual type_kind get_kind(void) const throw();

        //virtual bool is_equivalent(const type *that) const throw();
        //virtual bool is_subtype(const type *that) const throw();
    };

    struct kind_equivalent_meta_type : public type {
    public:
        const type_kind kind;

        kind_equivalent_meta_type(type_kind kind_) throw();
        virtual ~kind_equivalent_meta_type(void) throw();

        //virtual bool is_subtype(const type *that) const throw();
    };

    /// a type with a name
    struct name_type : public type {
    public:
        type_name name;

        //name_type(uint32_t line_=0, uint32_t col_=0, uint16_t file_id_=0) throw();
        //name_type(type_name name_, uint32_t line_=0, uint32_t col_=0, uint16_t file_id_=0) throw();

        //name_type(void) throw();
        //name_type(type_handle *handle_, type_name name_) throw();
        //name_type(uint32_t line_, uint32_t col_, uint16_t file_id_,
        //           type_handle *handle_, type_name name_) throw();

        virtual ~name_type(void) throw();
        virtual type_kind get_kind(void) const throw();
    };

    /// an equivalent name for an anonymous type.
    struct alias_type : public name_type {
    public:
        type *aliased_type;

        //alias_type(type *aliased_type_=nullptr, uint32_t line_=0, uint32_t col_=0, uint16_t file_id_=0) throw();
        //alias_type(type_name name_, type *aliased_type_=nullptr, uint32_t line_=0, uint32_t col_=0, uint16_t file_id_=0) throw();

        //alias_type(void) throw();
        //alias_type(type_handle *handle_, type_name name_, type *aliased_type_) throw();
        //alias_type(uint32_t line_, uint32_t col_, uint16_t file_id_,
        //           type_handle *handle_, type_name name_, type *aliased_type_) throw();

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

        //virtual bool is_equivalent(const type *that) const throw();
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
        //virtual bool is_equivalent(const type *that) const throw();

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

        //integer_type(type_handle *handle_, bool is_signed_, uint8_t align_,
        //             uint8_t width_) throw();

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
        //const bool eval_list;

        //token_list_type(type_handle *handle_, bool eval_list_) throw();

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
#if 0
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

        /*
         * TYPE,
            NAMED,
            ALIAS,
            COVER,
            PRODUCT,
            SUM,
            FUNCTION,
            REFERENCE,
            INTEGER,
            ARRAY,
            TOKEN_LIST,
            TOKEN_EXPRESSION,
            META_TYPE
         */

        extern kind_equivalent_meta_type META_KIND_TYPE;
        extern kind_equivalent_meta_type META_KIND_NAMED;
        extern kind_equivalent_meta_type META_KIND_ALIAS;
        extern kind_equivalent_meta_type META_KIND_COVER;
    }
#endif
}
#if 0
namespace std {
    template <>
    struct less<xy::type_handle *> : binary_function <xy::type_handle *,xy::type_handle *,bool> {
    public:
        bool operator() (const xy::type_handle *x, const xy::type_handle *y) const {
            return x->id < y->id;
        }
    };

    /*
    namespace tr1 {
        template <>
        struct hash<std::vector<xy::type *> *>
            : public std::unary_function<std::vector<xy::type *> *, std::size_t>
        {
        public:
            std::size_t operator()(const std::vector<xy::type *> *types) const {
                enum {
                    NUM_STATIC_IDS = 16
                };
                xy::type_id ids_static[NUM_STATIC_IDS];
                xy::type_id *ids(nullptr);
                const size_t num_ids(types->size());
                if(num_ids > num_ids) {
                    ids = new xy::type_id[num_ids];
                } else {
                    ids = &(ids_static[0]);
                }

                for(size_t i(0); i < num_ids; ++i) {
                    ids[i] = ((*types)[i])->handle->id;
                }

                if(num_ids > num_ids) {
                    delete [] ids;
                }
                ids = nullptr;
            }
        };
    }
    */
}
#endif

#endif /* TYPE_HPP_ */
