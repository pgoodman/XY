/*
 * ast.hpp
 *
 *  Created on: Nov 20, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_AST_HPP_
#define XY_AST_HPP_

#include <vector>
#include <iostream>

#include "xy/include/pp.hpp"
#include "xy/include/token.hpp"
#include "xy/include/front_end.hpp"
#include "xy/include/mpl/id.hpp"

#include "xy/include/support/list_map.hpp"
#include "xy/include/support/unsafe_cast.hpp"

#include "xy/include/cstring.hpp"

#define XY_AST_CONST_ARG(__, a) decltype(a) XY_CAT(a, _)
#define XY_AST_CONST_ARG_INIT(__, a) this->a = XY_CAT(a, _);
#define XY_AST_PARENT_CONST_ARG(parent_class, a) decltype(static_cast<parent_class *>(nullptr)->a) XY_CAT(a, _)
#define XY_AST_PARENT_CONST_ARG_INIT(__, a) (this->a = XY_CAT(a, _))

#define XY_AST_CONST_ARG_COMMA_UNROLL_1(A, B, a) A(B, a)
#define XY_AST_CONST_ARG_COMMA_UNROLL_2(A, B, a, b) A(B, a), A(B, b)
#define XY_AST_CONST_ARG_COMMA_UNROLL_3(A, B, a, b, c) A(B, a), A(B, b), A(B, c)
#define XY_AST_CONST_ARG_COMMA_UNROLL_4(A, B, a, b, c, d) A(B, a), A(B, b), A(B, c), A(B, d)
#define XY_AST_CONST_ARG_COMMA_UNROLL_5(A, B, a, b, c, d, e) A(B, a), A(B, b), A(B, c), A(B, d), A(B, e)

#define XY_AST_CONST_ARG_UNROLL_1(A, B, a) A(B, a)
#define XY_AST_CONST_ARG_UNROLL_2(A, B, a, b) A(B, a) A(B, b)
#define XY_AST_CONST_ARG_UNROLL_3(A, B, a, b, c) A(B, a) A(B, b) A(B, c)
#define XY_AST_CONST_ARG_UNROLL_4(A, B, a, b, c, d) A(B, a) A(B, b) A(B, c) A(B, d)
#define XY_AST_CONST_ARG_UNROLL_5(A, B, a, b, c, d, e) A(B, a) A(B, b) A(B, c) A(B, d) A(B, e)

#define XY_AST_CONST_DEFAULT_ARG_INIT(a, v) a(v)
#define XY_AST_CONST_DEFAUL_ARG_UNROLL_1(A, a) A a
#define XY_AST_CONST_DEFAUL_ARG_UNROLL_2(A, a, b) A a, A b
#define XY_AST_CONST_DEFAUL_ARG_UNROLL_3(A, a, b, c) A a, A b, A c
#define XY_AST_CONST_DEFAUL_ARG_UNROLL_4(A, a, b, c, d) A a, A b, A c, A d
#define XY_AST_CONST_DEFAUL_ARG_UNROLL_5(A, a, b, c, d, e) A a, A b, A c, A d, A e

#define XY_AST_MEMBER_UNROLL_1(A, a) A a
#define XY_AST_MEMBER_UNROLL_2(A, a, b) A a A b
#define XY_AST_MEMBER_UNROLL_3(A, a, b, c) A a A b A c
#define XY_AST_MEMBER_UNROLL_4(A, a, b, c, d) A a A b A c A d
#define XY_AST_MEMBER_UNROLL_5(A, a, b, c, d, e) A a A b A c A d A e

#define XY_AST_DESTROY_FIELD(__, a) \
    support::delete_ast<decltype(this->a)>::rec_delete(this->a);

/// used to forward declare an AST type
#define XY_AST_FORWARD_DECLARE(type) \
    struct type;

#define XY_AST_DECLARE_VISITORS(type) \
    virtual void visit(type *) throw();

/// apply some macro M to all AST types
#define XY_AST_TYPES(M) \
    M(expression) \
    M(name_expression) \
    M(function_call_expression) \
    M(type_instance_expression) \
    M(array_access_expression) \
    M(infix_expression) \
    M(prefix_expression) \
    M(literal_expression) \
    M(integer_literal_expression) \
    M(rational_literal_expression) \
    M(string_literal_expression) \
    M(conjunctive_statement) \
    M(statement) \
    M(expression_statement) \
    M(definition_statement) \
    M(type_definition) \
    M(structured_type_definition) \
    M(variable_definition) \
    M(function_definition) \
    M(return_expression_statement) \
    M(return_type_statement) \
    M(type_declaration) \
    M(template_instance_type_declaration) \
    M(named_type_declaration) \
    M(type_type_declaration) \
    M(unit_type_declaration) \
    M(array_type_declaration) \
    M(reference_type_declaration) \
    M(binary_type_declaration) \
    M(sum_type_declaration) \
    M(product_type_declaration) \
    M(arrow_type_declaration)

namespace xy {

    namespace {

        /// a "hidden" category for type ids
        class ast_type_category { };
    }

    typedef uint64_t ast_type;

    typedef std::vector<std::pair<token, symtab::symbol> > \
            name_list;

    typedef support::list_map<symtab::symbol, symtab::entry *> \
            name_binding;

    XY_AST_TYPES(XY_AST_FORWARD_DECLARE)

    /// AST visitor class
    struct ast_visitor {
    public:
        virtual ~ast_visitor(void) throw() { }
        virtual void visit(ast *) throw();
        XY_AST_TYPES(XY_AST_DECLARE_VISITORS)
    };

    namespace support {

        /// implements the subtype relationship for AST class types and the
        /// needed RTTI.
        template <typename T, typename O=void>
        struct ast_impl;

        /// non-virtual; represent a subtype relationship among two AST classes
        /// i.e. T <: O, where we can check the subtype relation at runtime.
        template <typename T, typename O>
        struct ast_impl : public O {
        public:

            ast_impl(void) throw() { }

            static ast_type static_id(void) throw() {
                static bool has_id(false);
                static ast_type id(0);

                if(!has_id) {
                    has_id = true;
                    id = O::static_id() | (1 << mpl::id<T, ast_type_category>::get());
                }

                return id;
            }

            static ast_type exact_id(void) throw() {
                return 1 << mpl::id<T, ast_type_category>::get();
            }

            virtual ~ast_impl(void) throw() { }

            virtual ast_type type_id(void) const throw() {
                return static_id();
            }

            virtual void visit(ast_visitor &visitor) throw() {
                T *self(support::unsafe_cast<T *>(this));
                visitor.visit(self);
            }
        };

        /// top-level, virtual; base class of the root AST class.
        template <typename T>
        struct ast_impl<T, void> {
        public:

            ast_impl(void) throw() { }

            static ast_type static_id(void) throw() {
                static bool has_id(false);
                static ast_type id(0);

                if(!has_id) {
                    has_id = true;
                    id = (1 << mpl::id<T, ast_type_category>::get());
                }

                return id;
            }

            static ast_type exact_id(void) throw() {
                return 1 << mpl::id<T, ast_type_category>::get();
            }

            virtual ~ast_impl(void) throw() { }

            virtual ast_type type_id(void) const throw() {
                return static_id();
            }

            virtual void visit(ast_visitor &visitor) throw() {
                T *self(support::unsafe_cast<T *>(this));
                visitor.visit(self);
            }
        };

        template <typename T>
        class delete_ast;

        /// delete a vector of AST nodes stored in an AST node
        template <typename T>
        class delete_ast<std::vector<T *> > {
        public:
            static void rec_delete(std::vector<T *> &vec) throw() {
                //for(T *type : vec) {
                for(size_t i(0); i < vec.size(); ++i) {
                    if(nullptr != vec[i]) {
                        delete vec[i];
                        vec[i] = nullptr;
                    }
                }
                vec.clear();
            }
        };

        /// delete a cstring stored in an AST node
        template <>
        class delete_ast<const char *> {
        public:
            static void rec_delete(const char *ptr) throw() {
                if(nullptr != ptr) {
                    cstring::free(ptr);
                }
            }
        };

        /// delete an AST stored in an AST node
        template <typename T>
        class delete_ast {
        public:
            static void rec_delete(T &ptr) throw() {
                if(nullptr != ptr) {
                    delete ptr;
                    ptr = nullptr;
                }
            }
        };

        template <typename T>
        void print_separated_list(std::vector<T> &ls, const char *sep, std::ostream &os, symbol_table &stab) throw() {
            for(size_t i(0); i < ls.size(); ++i) {
                if(0 < i) {
                    os << sep;
                }
                ls[i]->print(os, stab);
            }
        }
    }

    /// top-level AST node type
    struct ast : public support::ast_impl<ast> {
    public:

        ast(void) throw();

        virtual ~ast(void) throw();

        template <typename T>
        bool is_instance(void) const throw() {
            return 0UL != (this->type_id() & T::exact_id());
        }

        template <typename T>
        T *reinterpret(void) throw() {
            if(!this->is_instance<T>()) {
                return nullptr;
            }

            return support::unsafe_cast<T *>(this);
        }

        virtual void print(std::ostream &, symbol_table &) throw();
    };

#   include "xy/include/ast.inc"
}

#endif /* XY_AST_HPP_ */
