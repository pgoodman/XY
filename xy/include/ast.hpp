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
#include "xy/include/symbol_table.hpp"
#include "xy/include/mpl/id.hpp"
#include "xy/include/support/name_map.hpp"
#include "xy/include/support/unsafe_cast.hpp"

#define XY_AST_CONST_ARG(__, a) decltype(a) XY_CAT(a, _)
#define XY_AST_CONST_ARG_INIT(__, a) a(XY_CAT(a, _))
#define XY_AST_PARENT_CONST_ARG(parent_class, a) decltype(static_cast<parent_class *>(nullptr)->a) XY_CAT(a, _)
#define XY_AST_PARENT_CONST_ARG_INIT(__, a) (this->a = XY_CAT(a, _))

#define XY_AST_CONST_ARG_UNROLL_1(A, B, a) A(B, a)
#define XY_AST_CONST_ARG_UNROLL_2(A, B, a, b) A(B, a), A(B, b)
#define XY_AST_CONST_ARG_UNROLL_3(A, B, a, b, c) A(B, a), A(B, b), A(B, c)
#define XY_AST_CONST_ARG_UNROLL_4(A, B, a, b, c, d) A(B, a), A(B, b), A(B, c), A(B, d)
#define XY_AST_CONST_ARG_UNROLL_5(A, B, a, b, c, d, e) A(B, a), A(B, b), A(B, c), A(B, d), A(B, e)

#define XY_AST_CONST_DEFAULT_ARG_INIT(a, v) a(v)
#define XY_AST_CONST_DEFAUL_ARG_UNROLL_1(A, a) A a
#define XY_AST_CONST_DEFAUL_ARG_UNROLL_2(A, a, b) A a, A b
#define XY_AST_CONST_DEFAUL_ARG_UNROLL_3(A, a, b, c) A a, A b, A c
#define XY_AST_CONST_DEFAUL_ARG_UNROLL_4(A, a, b, c, d) A a, A b, A c, A d
#define XY_AST_CONST_DEFAUL_ARG_UNROLL_5(A, a, b, c, d, e) A a, A b, A c, A d, A e

#define XY_AST_DESTROY_FIELD(__, a) \
    support::delete_ast<decltype(this->a)>::rec_delete(this->a)

/// specify a constructor and which fields it should construct
#define XY_AST_CONSTRUCTOR(class_name, ...) \
    class_name (XY_CAT(XY_AST_CONST_ARG_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_CONST_ARG, void, __VA_ARGS__)) throw() \
        : XY_CAT(XY_AST_CONST_ARG_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_CONST_ARG_INIT, void, __VA_ARGS__) \
    { }

/// specify a destructor and which fields should be recursively destroyed
#define XY_AST_DESTRUCTOR(class_name, ...) \
    virtual ~class_name(void) throw() { \
        XY_CAT(XY_AST_CONST_ARG_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_DESTROY_FIELD, void, __VA_ARGS__); \
    }

#define XY_AST_DEFAULT_DESTRUCTOR(class_name) \
    virtual ~class_name(void) throw() { }

/// define a default constructor, with the initialization specialized with tuples,
/// e.g. (var, val).
#define XY_AST_DEFAULT_CONSTRUCTOR(class_name, ...) \
    class_name(void) throw() \
        : XY_CAT(XY_AST_CONST_DEFAUL_ARG_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_CONST_DEFAULT_ARG_INIT, __VA_ARGS__) \
    { }

/// inherit a base class constructor
#define XY_AST_INHERIT_CONSTRUCTOR(class_name, parent_class) \
    using ast_impl<class_name,parent_class>::ast_impl;

/// make a constructor that forwords all of its arguments to a base class
/// constructor
#define XY_AST_FORWARD_CONSTRUCTOR(class_name, parent_class, ...) \
    class_name (XY_CAT(XY_AST_CONST_ARG_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_PARENT_CONST_ARG, parent_class, __VA_ARGS__)) throw() { \
        XY_CAT(XY_AST_CONST_ARG_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_PARENT_CONST_ARG_INIT, parent_class, __VA_ARGS__) ; \
    }

namespace xy {

    // forward declaration
    struct type;

    namespace {

        /// a "hidden" category for type ids
        class ast_type_category { };
    }

    typedef size_t ast_type;

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

        ast(void) throw() { }

        virtual ~ast(void) throw() { }

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

        virtual void print(std::ostream &, symbol_table &) throw() { }
    };

    struct expression;
    struct statement;
    struct type_decl;
    struct arrow_type_decl;

    /// top-level expression AST type
    struct expression : public support::ast_impl<expression, ast> {
    public:
        type *type_;

        XY_AST_DEFAULT_CONSTRUCTOR(expression, (type_, nullptr))
        XY_AST_CONSTRUCTOR(expression, type_)
        XY_AST_DEFAULT_DESTRUCTOR(expression)
    };

        struct name_expr : public support::ast_impl<name_expr, expression> {
        public:

            support::mapped_name name;

            XY_AST_CONSTRUCTOR(name_expr, name)
            XY_AST_DEFAULT_DESTRUCTOR(name_expr)
        };

        struct function_call_expr : public support::ast_impl<function_call_expr, expression> {
        public:
            expression *function;
            std::vector<ast *> template_parameters;
            std::vector<expression *> arguments;

            // it is possible that we will get some arguments, categorized as
            // template arguments, that are all expressions, but are not known
            // if they are template args or function args
            bool might_be_ambiguous;

            XY_AST_CONSTRUCTOR(function_call_expr, function)
            XY_AST_DESTRUCTOR(function_call_expr, function, template_parameters, arguments)
        };

        struct type_instance_expr : public support::ast_impl<type_instance_expr, expression> {
        public:
            type_decl *declaration;
            std::vector<expression *> values;

            XY_AST_CONSTRUCTOR(type_instance_expr, declaration)
            XY_AST_DESTRUCTOR(type_instance_expr, declaration, values)
        };

        struct array_access_expr : public support::ast_impl<array_access_expr, expression> {
        public:
            expression *array;
            expression *index;

            XY_AST_CONSTRUCTOR(array_access_expr, array, index)
            XY_AST_DESTRUCTOR(array_access_expr, array, index)
        };

        struct infix_expr : public support::ast_impl<infix_expr, expression> {
        public:
            expression *left;
            expression *right;
            token_type op;

            XY_AST_CONSTRUCTOR(infix_expr, left, right, op)
            XY_AST_DESTRUCTOR(infix_expr, left, right)
        };

        struct prefix_expr : public support::ast_impl<prefix_expr, expression> {
        public:
            expression *right;
            token_type op;

            XY_AST_CONSTRUCTOR(prefix_expr, right, op)
            XY_AST_DESTRUCTOR(prefix_expr, right)
        };

        struct array_expr : public support::ast_impl<array_expr, expression> {
        public:
            std::vector<expression *> elements;

            XY_AST_DEFAULT_CONSTRUCTOR(array_expr, (elements, XY_NOTHING))
            XY_AST_DESTRUCTOR(array_expr, elements)
        };

        struct literal_expr : public support::ast_impl<literal_expr, expression> {
        public:
            const char *data;

            XY_AST_DEFAULT_CONSTRUCTOR(literal_expr, (data, nullptr))
            XY_AST_CONSTRUCTOR(literal_expr, data)
            XY_AST_DESTRUCTOR(literal_expr, data)
        };

            struct integer_literal_expr : public support::ast_impl<integer_literal_expr, literal_expr> {
            public:

                XY_AST_FORWARD_CONSTRUCTOR(integer_literal_expr, literal_expr, data)
                XY_AST_DEFAULT_DESTRUCTOR(integer_literal_expr)
            };

            struct rational_literal_expr : public support::ast_impl<rational_literal_expr, literal_expr> {
            public:

                XY_AST_FORWARD_CONSTRUCTOR(rational_literal_expr, literal_expr, data)
                XY_AST_DEFAULT_DESTRUCTOR(rational_literal_expr)
            };

            struct string_literal_expr : public support::ast_impl<string_literal_expr, literal_expr> {
            public:

                unsigned byte_length;

                string_literal_expr(const char *data_, unsigned len_) throw()
                    : byte_length(len_)
                {
                    this->data = data_;
                }
                XY_AST_DEFAULT_DESTRUCTOR(string_literal_expr)
            };

    struct statement_list : public support::ast_impl<statement_list, ast> {
    public:
        std::vector<statement *> statements;

        XY_AST_DEFAULT_CONSTRUCTOR(statement_list,
            (statements, XY_NOTHING))
        XY_AST_DESTRUCTOR(statement_list, statements)
    };

    struct statement : public support::ast_impl<statement, ast> {
    public:
        statement(void) throw() { }
        virtual ~statement(void) throw() { }
    };

        struct expression_stmt : public support::ast_impl<expression_stmt, statement> {
        public:
            expression *expression;

            XY_AST_CONSTRUCTOR(expression_stmt, expression)
            XY_AST_DESTRUCTOR(expression_stmt, expression)
        };

        /// simple type definition
        struct type_def : public support::ast_impl<type_def, statement> {
        public:
            token location;
            support::mapped_name name;
            type_decl *declaration;

            XY_AST_CONSTRUCTOR(type_def, location, name, declaration)
            XY_AST_DESTRUCTOR(type_def, declaration)
        };

        /// simple variable definition
        struct var_def : public support::ast_impl<var_def, statement> {
        public:
            token location;
            support::mapped_name name;
            expression *value;

            XY_AST_CONSTRUCTOR(var_def, location, name, value)
            XY_AST_DESTRUCTOR(var_def, value)
        };

        /// function definition/declaration
        struct func_def : public support::ast_impl<var_def, statement> {
        public:
            token location;
            support::mapped_name name;
            arrow_type_decl *template_arg_types;
            arrow_type_decl *arg_types;
            std::vector<support::mapped_name> template_arg_names;
            std::vector<support::mapped_name> arg_names;
            statement_list *statements;

            XY_AST_DEFAULT_CONSTRUCTOR(func_def,
                (template_arg_types,    nullptr),
                (arg_types,             nullptr),
                (template_arg_names,    XY_NOTHING),
                (arg_names,             XY_NOTHING),
                (statements,            nullptr))

            XY_AST_DESTRUCTOR(func_def,
                template_arg_types, arg_types, statements)
        };

        struct return_expr_stmt : public support::ast_impl<return_expr_stmt, statement> {
            expression *value;

            XY_AST_CONSTRUCTOR(return_expr_stmt, value)
            XY_AST_DESTRUCTOR(return_expr_stmt, value)
        };

        struct return_type_stmt : public support::ast_impl<return_type_stmt, statement> {
            type_decl *value;

            XY_AST_CONSTRUCTOR(return_type_stmt, value)
            XY_AST_DESTRUCTOR(return_type_stmt, value)
        };

    struct type_decl : public support::ast_impl<type_decl, ast> {
        token location;
        type_decl(void) throw() { }
        virtual ~type_decl(void) throw() { }
        virtual bool returns_type(void) throw() { return false; }
    };

        struct template_instance_type_decl : public support::ast_impl<template_instance_type_decl, type_decl> {

            type_decl *template_type;
            std::vector<ast *> parameters;

            XY_AST_CONSTRUCTOR(template_instance_type_decl, template_type)
            XY_AST_DESTRUCTOR(template_instance_type_decl, template_type, parameters)

            virtual void print(std::ostream &os, symbol_table &stab) throw() {
                template_type->print(os, stab);
                os << "(";
                support::print_separated_list(parameters, ", ", os, stab);
                os << ")";
            }
        };

        struct named_type_decl : public support::ast_impl<named_type_decl, type_decl> {
        public:
            support::mapped_name name;

            XY_AST_CONSTRUCTOR(named_type_decl, name)
            XY_AST_DEFAULT_DESTRUCTOR(named_type_decl)

            virtual void print(std::ostream &os, symbol_table &stab) throw() {
                os << stab[name];
            }
        };

        struct type_type_decl : public support::ast_impl<type_type_decl, type_decl> {
        public:
            virtual void print(std::ostream &os, symbol_table &) throw() {
                os << "Type";
            }
        };

        struct type_unit_decl : public support::ast_impl<type_unit_decl, type_decl> {
        public:
            virtual void print(std::ostream &os, symbol_table &) throw() {
                os << "Unit";
            }
        };

        struct array_type_decl : public support::ast_impl<array_type_decl, type_decl> {
        public:
            type_decl *inner_type;

            XY_AST_CONSTRUCTOR(array_type_decl, inner_type)
            XY_AST_DESTRUCTOR(array_type_decl, inner_type)

            virtual void print(std::ostream &os, symbol_table &stab) throw() {
                os << "[";
                inner_type->print(os, stab);
                os << "]";
            }
        };

        struct reference_type_decl : public support::ast_impl<reference_type_decl, type_decl> {
        public:
            type_decl *inner_type;

            XY_AST_CONSTRUCTOR(reference_type_decl, inner_type)
            XY_AST_DESTRUCTOR(reference_type_decl, inner_type)

            virtual void print(std::ostream &os, symbol_table &stab) throw() {
                os << "&";
                if(inner_type->is_instance<named_type_decl>()
                || inner_type->is_instance<reference_type_decl>()
                || inner_type->is_instance<array_type_decl>()) {
                    inner_type->print(os, stab);
                } else {
                    os << "(";
                    inner_type->print(os, stab);
                    os << ")";
                }
            }
        };

        struct binary_type_decl : public support::ast_impl<binary_type_decl, type_decl> {
        public:
            std::vector<type_decl *> types;
            bool is_wrapped;

            XY_AST_DEFAULT_CONSTRUCTOR(binary_type_decl, (types, XY_NOTHING), (is_wrapped, false))
            XY_AST_DESTRUCTOR(binary_type_decl, types)
        };

            struct sum_type_decl : public support::ast_impl<sum_type_decl, binary_type_decl> {
            public:
                std::vector<support::mapped_name> params;
                std::vector<support::mapped_name> fields;

                XY_AST_DEFAULT_CONSTRUCTOR(sum_type_decl, (params, XY_NOTHING), (fields, XY_NOTHING))
                XY_AST_DEFAULT_DESTRUCTOR(sum_type_decl)

                virtual void print(std::ostream &os, symbol_table &stab) throw() {
                    os << "(";
                    support::print_separated_list(this->types, " + ", os, stab);
                    os << ")";
                }
            };

            struct product_type_decl : public support::ast_impl<product_type_decl, binary_type_decl> {
            public:
                std::vector<support::mapped_name> params;
                std::vector<support::mapped_name> fields;

                XY_AST_DEFAULT_CONSTRUCTOR(product_type_decl, (params, XY_NOTHING), (fields, XY_NOTHING))
                XY_AST_DEFAULT_DESTRUCTOR(product_type_decl)

                virtual void print(std::ostream &os, symbol_table &stab) throw() {
                    support::print_separated_list(this->types, " * ", os, stab);
                }
            };

            struct arrow_type_decl : public support::ast_impl<arrow_type_decl, binary_type_decl> {
            public:
                XY_AST_DEFAULT_DESTRUCTOR(arrow_type_decl)

                virtual void print(std::ostream &os, symbol_table &stab) throw() {
                    os << "(";
                    support::print_separated_list(this->types, " -> ", os, stab);
                    os << ")";
                }

                /// is this a template type? i.e. do we generate something
                /// that is a type or is somehting that generates a type?
                virtual bool returns_type(void) throw() {
                    if(types.empty()) {
                        return false;
                    }

                    type_decl *return_type(types.back());

                    if(return_type->is_instance<type_type_decl>()) {
                        return true;
                    }

                    return return_type->returns_type();
                }
            };

}

#endif /* XY_AST_HPP_ */
