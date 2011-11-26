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

#include "xy/include/pp.hpp"
#include "xy/include/token.hpp"
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

/// specify a constructor and which fields it should construct
#define XY_AST_CONSTRUCTOR(class_name, ...) \
    class_name (XY_CAT(XY_AST_CONST_ARG_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_CONST_ARG, void, __VA_ARGS__)) throw() \
        : XY_CAT(XY_AST_CONST_ARG_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_CONST_ARG_INIT, void, __VA_ARGS__) \
    { }

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
        class type_category { };
    }

    typedef size_t ast_type;

    namespace support {

        template <typename T, typename O=void>
        struct ast_impl;

        // non-virtual
        template <typename T, typename O>
        struct ast_impl : public O {
        public:

            ast_impl(void) throw() { }

            static ast_type static_id(void) throw() {
                static bool has_id(false);
                static ast_type id(0);

                if(!has_id) {
                    has_id = true;
                    id = O::static_id() | (1 << mpl::id<T, type_category>::get());
                }

                return id;
            }

            static ast_type exact_id(void) throw() {
                return 1 << mpl::id<T, type_category>::get();
            }

            virtual ~ast_impl(void) throw() { }

            virtual ast_type type_id(void) const throw() {
                return static_id();
            }
        };

        // top-level, virtual
        template <typename T>
        struct ast_impl<T, void> {
        public:

            ast_impl(void) throw() { }

            static ast_type static_id(void) throw() {
                static bool has_id(false);
                static ast_type id(0);

                if(!has_id) {
                    has_id = true;
                    id = (1 << mpl::id<T, type_category>::get());
                }

                return id;
            }

            static ast_type exact_id(void) throw() {
                return 1 << mpl::id<T, type_category>::get();
            }

            virtual ~ast_impl(void) throw() { }

            virtual ast_type type_id(void) const throw() {
                return static_id();
            }
        };

        template <typename T>
        void delete_ast(T &ptr) throw() {
            if(nullptr != ptr) {
                delete ptr;
                ptr = nullptr;
            }
        }

        template <typename T>
        void delete_ast_vector(std::vector<T *> &vec) throw() {
            //for(T *type : vec) {
            for(size_t i(0); i < vec.size(); ++i) {
                if(nullptr != vec[i]) {
                    delete vec[i];
                    vec[i] = nullptr;
                }
            }
            vec.clear();
        }
    }

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
    };

    struct expression;
    struct statement;
    struct type_decl;

    struct expression : public support::ast_impl<expression, ast> {
    public:
        type *type_;

        XY_AST_DEFAULT_CONSTRUCTOR(expression, (type_, nullptr))
        XY_AST_CONSTRUCTOR(expression, type_)

        virtual ~expression(void) throw() { }
    };

        struct name_expr : public support::ast_impl<name_expr, expression> {
        public:

            support::mapped_name name;

            XY_AST_CONSTRUCTOR(name_expr, name)

            virtual ~name_expr(void) throw() { }
        };

        struct function_call_expr : public support::ast_impl<function_call_expr, expression> {
        public:
            expression *function;
            std::vector<expression *> parameters;

            XY_AST_CONSTRUCTOR(function_call_expr, function)
            //XY_AST_CONSTRUCTOR(function_call_expr, function, parameters)

            virtual ~function_call_expr(void) throw() {
                support::delete_ast(function);
                support::delete_ast_vector(parameters);
            }
        };

        struct type_instance_expr : public support::ast_impl<type_instance_expr, expression> {
        public:
            type_decl *declaration;
            std::vector<expression *> values;

            XY_AST_CONSTRUCTOR(type_instance_expr, declaration)
            //XY_AST_CONSTRUCTOR(type_instance_expr, declaration, values)

            virtual ~type_instance_expr(void) throw() {
                support::delete_ast(declaration);
                support::delete_ast_vector(values);
            }
        };

        struct array_access_expr : public support::ast_impl<array_access_expr, expression> {
        public:
            expression *array;
            expression *index;

            XY_AST_CONSTRUCTOR(array_access_expr, array, index)

            virtual ~array_access_expr(void) throw() {
                support::delete_ast(array);
                support::delete_ast(index);
            }
        };

        struct infix_expr : public support::ast_impl<infix_expr, expression> {
        public:
            expression *left;
            expression *right;
            token_type op;

            XY_AST_CONSTRUCTOR(infix_expr, left, right, op)

            virtual ~infix_expr(void) throw() {
                support::delete_ast(left);
                support::delete_ast(right);
            }
        };

        struct prefix_expr : public support::ast_impl<prefix_expr, expression> {
        public:
            expression *right;
            token_type op;

            XY_AST_CONSTRUCTOR(prefix_expr, right, op)

            virtual ~prefix_expr(void) throw() {
                support::delete_ast(right);
            }
        };

        struct array_expr : public support::ast_impl<array_expr, expression> {
        public:
            std::vector<expression *> elements;

            XY_AST_DEFAULT_CONSTRUCTOR(array_expr, (elements, XY_NOTHING))
            //XY_AST_CONSTRUCTOR(array_expr, elements)

            virtual ~array_expr(void) throw() {
                support::delete_ast_vector(elements);
            }
        };

        struct literal_expr : public support::ast_impl<literal_expr, expression> {
        public:
            const char *data;

            XY_AST_DEFAULT_CONSTRUCTOR(literal_expr, (data, nullptr))
            XY_AST_CONSTRUCTOR(literal_expr, data)

            virtual ~literal_expr(void) throw() {
                cstring::free(data);
                data = nullptr;
            }
        };

            struct integer_literal_expr : public support::ast_impl<integer_literal_expr, literal_expr> {
            public:

                XY_AST_FORWARD_CONSTRUCTOR(integer_literal_expr, literal_expr, data)

                virtual ~integer_literal_expr(void) throw() { }
            };

            struct rational_literal_expr : public support::ast_impl<rational_literal_expr, literal_expr> {
            public:

                XY_AST_FORWARD_CONSTRUCTOR(rational_literal_expr, literal_expr, data)

                virtual ~rational_literal_expr(void) throw() { }
            };

            struct string_literal_expr : public support::ast_impl<string_literal_expr, literal_expr> {
            public:

                XY_AST_FORWARD_CONSTRUCTOR(string_literal_expr, literal_expr, data)

                virtual ~string_literal_expr(void) throw() { }
            };

    struct statement_list : public support::ast_impl<statement_list, ast> {
    public:
        std::vector<statement *> statements;

        //XY_AST_CONSTRUCTOR(statement_list, statements)
        XY_AST_DEFAULT_CONSTRUCTOR(statement_list, (statements, XY_NOTHING))

        virtual ~statement_list(void) throw() {
            support::delete_ast_vector(statements);
        }
    };

    struct statement : public support::ast_impl<statement, ast> {
    public:
        statement(void) throw() { }
        virtual ~statement(void) throw() { }
    };

        struct type_def : public support::ast_impl<type_def, statement> {
        public:
            support::mapped_name name;
            type_decl *declaration;
            type_decl *func;

            XY_AST_CONSTRUCTOR(type_def, name, declaration, func)

            virtual ~type_def(void) throw() {
                support::delete_ast(declaration);
                support::delete_ast(func);
            }
        };

        struct var_def : public support::ast_impl<var_def, statement> {
        public:
            support::mapped_name name;
            expression *value;

            XY_AST_CONSTRUCTOR(var_def, name, value)

            virtual ~var_def(void) throw() {
                support::delete_ast(value);
            }
        };

        struct return_stmt : public support::ast_impl<return_stmt, statement> {
            expression *value;

            XY_AST_CONSTRUCTOR(return_stmt, value)

            virtual ~return_stmt(void) throw() {
                support::delete_ast(value);
            }
        };

    struct type_decl : public support::ast_impl<type_decl, ast> {
        type_decl(void) throw() { }
        virtual ~type_decl(void) throw() { }
    };

        struct template_instance_type_decl : public support::ast_impl<template_instance_type_decl, type_decl> {

            type_decl *template_type;
            std::vector<ast *> parameters;

            XY_AST_CONSTRUCTOR(template_instance_type_decl, template_type)

            virtual ~template_instance_type_decl(void) throw() {
                support::delete_ast(template_type);
                support::delete_ast_vector(parameters);
            }
        };

        struct named_type_decl : public support::ast_impl<named_type_decl, type_decl> {
        public:
            support::mapped_name name;

            XY_AST_CONSTRUCTOR(named_type_decl, name)

            virtual ~named_type_decl(void) throw() { }
        };

        struct array_type_decl : public support::ast_impl<array_type_decl, type_decl> {
        public:
            type_decl *inner_type;

            XY_AST_CONSTRUCTOR(array_type_decl, inner_type)

            virtual ~array_type_decl(void) throw() {
                support::delete_ast(inner_type);
            }
        };

        struct reference_type_decl : public support::ast_impl<reference_type_decl, type_decl> {
        public:
            type_decl *inner_type;

            XY_AST_CONSTRUCTOR(reference_type_decl, inner_type)

            virtual ~reference_type_decl(void) throw() {
                support::delete_ast(inner_type);
            }
        };

        struct binary_type_decl : public support::ast_impl<binary_type_decl, type_decl> {
        public:
            std::vector<type_decl *> types;

            XY_AST_DEFAULT_CONSTRUCTOR(binary_type_decl, (types, XY_NOTHING))
            //XY_AST_CONSTRUCTOR(binary_type_decl, types)

            virtual ~binary_type_decl(void) throw() {
                support::delete_ast_vector(types);
            }
        };

            struct sum_type_decl : public support::ast_impl<sum_type_decl, binary_type_decl> {
            public:
                std::vector<support::mapped_name> params;
                std::vector<support::mapped_name> fields;

                //XY_AST_FORWARD_CONSTRUCTOR(sum_type_decl, binary_type_decl, types)
                XY_AST_DEFAULT_CONSTRUCTOR(sum_type_decl, (params, XY_NOTHING), (fields, XY_NOTHING))

                virtual ~sum_type_decl(void) throw() { }
            };

            struct product_type_decl : public support::ast_impl<product_type_decl, binary_type_decl> {
            public:
                std::vector<support::mapped_name> params;
                std::vector<support::mapped_name> fields;

                //XY_AST_FORWARD_CONSTRUCTOR(product_type_decl, binary_type_decl, types)
                XY_AST_DEFAULT_CONSTRUCTOR(product_type_decl, (params, XY_NOTHING), (fields, XY_NOTHING))

                virtual ~product_type_decl(void) throw() {
                    support::delete_ast_vector(types);
                }
            };

            struct arrow_type_decl : public support::ast_impl<arrow_type_decl, binary_type_decl> {
            public:

                //XY_AST_FORWARD_CONSTRUCTOR(arrow_type_decl, binary_type_decl, types)
                //XY_AST_DEFAULT_CONSTRUCTOR(sum_type_decl, (params, XY_NOTHING), (fields, XY_NOTHING))

                virtual ~arrow_type_decl(void) throw() { }
            };

}

#endif /* XY_AST_HPP_ */
