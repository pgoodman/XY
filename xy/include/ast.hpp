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

#include "xy/include/token.hpp"
#include "xy/include/mpl/id.hpp"
#include "xy/include/support/name_map.hpp"
#include "xy/include/support/unsafe_cast.hpp"

namespace xy {

    // forward declaration
    struct type;

    namespace {
        class type_category { };
    }

    typedef size_t ast_type;

    namespace {

        template <typename T, typename O=void>
        struct ast_impl;

        // non-virtual
        template <typename T, typename O>
        struct ast_impl : public O {
        public:

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

    struct ast : public ast_impl<ast> {
    public:

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

    struct expression : public ast_impl<expression, ast> {
    public:
        type *type_;

        virtual ~expression(void) throw() { }
    };

        struct function_call_expr : public ast_impl<function_call_expr, expression> {
        public:
            expression *function;
            std::vector<expression *> parameters;

            virtual ~function_call_expr(void) throw() {
                delete_ast(function);
                delete_ast_vector(parameters);
            }
        };

        struct type_instance_expr : public ast_impl<type_instance_expr, expression> {
        public:
            type_decl *type_decl;
            std::vector<expression *> values;

            virtual ~type_instance_expr(void) throw() {
                delete_ast(type_decl);
                delete_ast_vector(values);
            }
        };

        struct array_access_expr : public ast_impl<array_access_expr, expression> {
        public:
            expression *array;
            expression *index;

            virtual ~array_access_expr(void) throw() {
                delete_ast(array);
                delete_ast(index);
            }
        };

        struct infix_expr : public ast_impl<infix_expr, expression> {
        public:
            expression *left;
            expression *right;
            token_type op;

            virtual ~infix_expr(void) throw() {
                delete_ast(left);
                delete_ast(right);
            }
        };

        struct prefix_expr : public ast_impl<prefix_expr, expression> {
        public:
            expression *right;
            token_type op;

            virtual ~prefix_expr(void) throw() {
                delete_ast(right);
            }
        };

        struct array_expr : public ast_impl<array_expr, expression> {
        public:
            std::vector<expression *> elements;

            virtual ~array_expr(void) throw() {
                delete_ast_vector(elements);
            }
        };

        struct literal_expr : public ast_impl<literal_expr, expression> {
        public:
            const char *data;

            virtual ~literal_expr(void) throw() {
                cstring::free(data);
                data = nullptr;
            }
        };

            struct integer_literal_expr : public ast_impl<integer_literal_expr, literal_expr> {
            public:
                virtual ~integer_literal_expr(void) throw() { }
            };

            struct rational_literal_expr : public ast_impl<rational_literal_expr, literal_expr> {
            public:
                virtual ~rational_literal_expr(void) throw() { }
            };

            struct string_literal_expr : public ast_impl<string_literal_expr, literal_expr> {
            public:
                virtual ~string_literal_expr(void) throw() { }
            };

    struct statement_list : public ast_impl<statement_list, ast> {
    public:
        std::vector<statement *> statements;

        virtual ~statement_list(void) throw() {
            delete_ast_vector(statements);
        }
    };

    struct statement : public ast_impl<statement, ast> {
    public:
        virtual ~statement(void) throw() { }
    };

        struct type_def : public ast_impl<type_def, statement> {
        public:
            support::mapped_name name;
            type_decl *declaration;
            type_decl *func;

            virtual ~type_def(void) throw() {
                delete_ast(declaration);
                delete_ast(func);
            }
        };

        struct var_def : public ast_impl<var_def, statement> {
        public:
            support::mapped_name name;
            expression *value;

            virtual ~var_def(void) throw() {
                delete_ast(value);
            }
        };

        struct return_stmt : public ast_impl<return_stmt, statement> {
            expression *value;

            virtual ~return_stmt(void) throw() {
                delete_ast(value);
            }
        };

    struct type_decl : public ast_impl<type_decl, ast> {
        virtual ~type_decl(void) throw() { }
    };

        struct named_type_decl : public ast_impl<named_type_decl, type_decl> {
        public:
            support::mapped_name name;

            virtual ~named_type_decl(void) throw() { }
        };

        struct array_type_decl : public ast_impl<array_type_decl, type_decl> {
        public:
            type_decl *inner_type;

            virtual ~array_type_decl(void) throw() {
                delete_ast(inner_type);
            }
        };

        struct reference_type_decl : public ast_impl<reference_type_decl, type_decl> {
        public:
            type_decl *inner_type;

            virtual ~reference_type_decl(void) throw() {
                delete_ast(inner_type);
            }
        };

        struct binary_type_decl : public ast_impl<binary_type_decl, type_decl> {
        public:
            std::vector<type_decl *> types;

            virtual ~binary_type_decl(void) throw() {
                delete_ast_vector(types);
            }
        };

            struct sum_type_decl : public ast_impl<sum_type_decl, binary_type_decl> {
            public:
                std::vector<support::mapped_name> params;
                std::vector<support::mapped_name> fields;

                virtual ~sum_type_decl(void) throw() { }
            };

            struct product_type_decl : public ast_impl<product_type_decl, binary_type_decl> {
            public:
                std::vector<support::mapped_name> params;
                std::vector<support::mapped_name> fields;

                virtual ~product_type_decl(void) throw() {
                    delete_ast_vector(types);
                }
            };

            struct arrow_type_decl : public ast_impl<arrow_type_decl, binary_type_decl> {
            public:
                virtual ~arrow_type_decl(void) throw() { }
            };

}

#endif /* XY_AST_HPP_ */
