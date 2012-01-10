/*
 * resolve_names.cpp
 *
 *  Created on: Jan 2, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#include <list>

#include "xy/include/pass/resolve_names.hpp"
#include "xy/include/diagnostic_context.hpp"
#include "xy/include/symbol_table.hpp"
#include "xy/include/token.hpp"
#include "xy/include/ast.hpp"
#include "xy/include/io/message.hpp"

#include "xy/include/type.hpp"
#include "xy/include/type_system.hpp"

#define D(x) x

namespace xy { namespace pass {

    /// set up an expectation of a name
    struct name_expectation {
    public:
        symtab::symbol expected_name;
        symtab::entry *needed_by;
    };

    /// list of expected names
    struct name_expectation_list : public std::list<name_expectation> {
    public:
        void push_back(symtab::symbol sym_, symtab::entry *entry) throw() {
            name_expectation exp;
            exp.expected_name = sym_;
            exp.needed_by = entry;
            this->std::list<name_expectation>::push_back(exp);
        }
    };

    /// resolve all names
    struct name_resolver_visitor : public ast_visitor {
    public:

        name_expectation_list expected_names;
        conjunctive_statement *top;
        symbol_table &stab;
        diagnostic_context &ctx;
        type_system &ts;

        type *any_type;

        symtab::symbol name;
        symtab::entry *entry;

        name_resolver_visitor(symbol_table &stab_, type_system &ts_, diagnostic_context &ctx_)
            : ast_visitor()
            , expected_names()
            , top(nullptr)
            , stab(stab_)
            , ctx(ctx_)
            , ts(ts_)
            , any_type(nullptr /* TODO */)
        { }

        virtual ~name_resolver_visitor(void) throw() { }

        /// visit all AST nodes in a vector
        template <typename T>
        void visit(std::vector<T *> &asts_) throw() {
            for(size_t i(0), len(asts_.size()); i < len; ++i) {
                asts_[i]->visit(*this);
            }
        }

        void report_existing_symbol(token &here_location) throw() {
            report_existing_symbol(here_location, entry->origin.def->location);
        }

        void report_existing_symbol(token &here_location, token &there_location) throw() {
            io::message_id message_id(io::e_duplicate_name);
            token location;

            switch(entry->name_type) {
            case symtab::TEMPLATE_TYPE_FUNCTION:
            case symtab::TEMPLATE_TYPE_RECORD:
            case symtab::TEMPLATE_TYPE_UNION:
            case symtab::TEMPLATE_TYPE_VARIABLE:
            case symtab::ARROW_TYPE:
            case symtab::SUM_TYPE:
            case symtab::PRODUCT_TYPE:
            case symtab::ARRAY_TYPE:
            case symtab::REF_TYPE:

                message_id = io::e_duplicate_type;
                location = entry->origin.def->location;
                break;
            default: break;
            }

            ctx.report_here(here_location, message_id, stab[name]);
            ctx.report_here(there_location, io::n_duplicate_symbol, stab[name]);
        }

        /// visit the top-level conjunctive statements. This needs to be
        /// done differently than non-top level things so that top-level
        /// functions can have access to each-other, as well as type stuff
        /*void visit_top_level(conjunctive_statement *ls) throw() {
            const size_t num_stmts(ls->statements.size());
            symtab::entry *entry(nullptr);
            definition_statement *def_stmt(nullptr);
            symtab::entry_type def_type(symtab::NONE);

            // visit all sub-statements to find the names that should be
            // "pulled up"
            for(size_t i(0); i < num_stmts; ++i) {

                stmt = ls->statements[i];
                def_stmt = stmt->reinterpret<definition_statement>();

                // expression statement, conjunctive statement
                if(nullptr == def_stmt) {
                    continue;

                // simple type def
                } else if(stmt->is_instance<type_definition>()) {

                    D( printf("type def\n"); )

                    type_definition *def(stmt->reinterpret<type_definition>());
                    entry = stab.lookup(top, def->name);

                    type_declaration *decl(def->declaration);

                    // array
                    if(decl->is_instance<array_type_declaration>()) {
                        def_type = symtab::ARRAY_TYPE;

                    // function
                    } else if(decl->is_instance<arrow_type_declaration>()) {
                        def_type = symtab::ARROW_TYPE;

                    // sum
                    } else if(decl->is_instance<sum_type_declaration>()) {
                        def_type = symtab::SUM_TYPE;

                    // product
                    } else if(decl->is_instance<product_type_declaration>()) {
                        def_type = symtab::PRODUCT_TYPE;

                    // reference
                    } else {
                        def_type = symtab::REF_TYPE;
                    }

                // complicated type def
                } else if(stmt->is_instance<structured_type_definition>()) {

                    D( printf("structured type def\n"); )

                    structured_type_definition *def(stmt->reinterpret<structured_type_definition>());
                    entry = stab.lookup(top, def->name);

                    if(def->declaration->is_instance<function_definition>()) {
                        def_type = symtab::TEMPLATE_TYPE_FUNCTION;
                    } else {
                        assert(false);
                        // TODO
                    }

                // function def
                } else if(stmt->is_instance<function_definition>()) {

                    D( printf("function def\n"); )

                    function_definition *def(stmt->reinterpret<function_definition>());
                    entry = stab.lookup(top, def->name);

                    if(nullptr == def->template_arg_types) {
                        def_type = symtab::FUNCTION;
                    } else {
                        def_type = symtab::TEMPLATE_FUNCTION;
                    }

                // top-level variable def
                } else if(stmt->is_instance<variable_definition>()
                       && nullptr == old_top) {

                    D( printf("found top-level var def\n"); )
                    D( printf("   var name is %s\n", stab[def_stmt->name]); )

                    variable_definition *def(stmt->reinterpret<variable_definition>());

                    entry = stab.lookup(top, def->name);
                    def_type = symtab::VARIABLE;

                // possible var def, non top-level
                } else {
                    continue;
                }

                // name already exists
                if(nullptr != entry) {
                    ctx.report_here(def_stmt->location, io::e_duplicate_symbol, stab[def_stmt->name]);
                    ctx.report_here(entry->origin->reinterpret<definition_statement>()->location, io::n_duplicate_symbol, stab[def_stmt->name]);

                // no name, make it
                } else {
                    entry = stab.insert(top, def_stmt->name);
                    entry->name_type = def_type;
                    entry->origin = def_stmt;
                    entry->type_ = nullptr; // TODO
                }
            }
        }*/

        /// visit a list of statements
        virtual void visit(conjunctive_statement *ls) throw() {
            D( printf("visiting conjunctive statement\n"); )

            conjunctive_statement *old_top(top);
            top = ls;
            statement *stmt(nullptr);

            const size_t expected_orig_size(expected_names.size());
            const size_t num_stmts(ls->statements.size());

            for(size_t i(0); i < num_stmts; ++i) {
                stmt = ls->statements[i];
                stmt->visit(*this);
            }

            // ensure all expected things are found
            for(; expected_names.size() > expected_orig_size; expected_names.pop_front()) {

            }

            top = old_top;
        }

        /// visit an expression that is a variable name. This must already be
        /// resolved.
        virtual void visit(name_expression *expr) throw() {
            symtab::entry *entry(stab.lookup(top, expr->name));
            if(nullptr == entry) {
                ctx.report_here(expr->location, io::e_unknown_var, stab[expr->name]);
                expr->type_ = any_type;
            } else {
                expr->type_ = entry->type.base;
            }
        }

        /// visit a function call
        virtual void visit(function_call_expression *expr) throw() {
            expr->function->visit(*this);
            visit(expr->arguments);
        }

        virtual void visit(type_instance_expression *expr) throw() {
            expr->declaration->visit(*this);
            // TODO make a type
            visit(expr->values);
        }

        virtual void visit(array_access_expression *expr) throw() {
            expr->array->visit(*this);
            expr->index->visit(*this);
        }

        virtual void visit(infix_expression *expr) throw() {
            expr->left->visit(*this);
            expr->right->visit(*this);
        }

        virtual void visit(prefix_expression *expr) throw() {
            expr->right->visit(*this);
        }

        virtual void visit(expression_statement *stmt) throw() {
            stmt->expression->visit(*this);
        }

        virtual void visit(type_definition *def) throw() {
            name = def->name;
            entry = stab.lookup(top, name);

            // type redefined in same scope
            if(nullptr != entry && entry->scope == top) {
                report_existing_symbol(def->location);
            } else {
                entry = stab.insert(top, def->name);
            }

            entry->origin.def = def;

            // make new entry or overwrite
            type_declaration *decl(def->declaration);

            // array
            if(decl->is_instance<array_type_declaration>()) {
                entry->name_type = symtab::ARRAY_TYPE;
                entry->type.array = ts.make_array();

            // function
            } else if(decl->is_instance<arrow_type_declaration>()) {
                entry->name_type = symtab::ARROW_TYPE;
                entry->type.arrow = ts.make_arrow();

            // sum
            } else if(decl->is_instance<sum_type_declaration>()) {
                entry->name_type = symtab::SUM_TYPE;
                entry->type.sum = ts.make_sum();

            // product
            } else if(decl->is_instance<product_type_declaration>()) {
                entry->name_type = symtab::PRODUCT_TYPE;
                entry->type.product = ts.make_product();

            // reference
            } else {
                entry->name_type = symtab::REF_TYPE;
                entry->type.ref = ts.make_reference();
            }


            decl->type_ = entry->type.base;
            decl->visit(*this);

        }

        virtual void visit(structured_type_definition *def) throw() {
            name = def->name;
            entry = stab.lookup(top, name);

            // type redefined in same scope
            if(nullptr != entry && entry->scope == top) {
                report_existing_symbol(def->location);
            } else {
                entry = stab.insert(top, name);
            }

            entry->origin.def = def;

            if(def->def->is_instance<function_definition>()) {
                entry->name_type = symtab::TEMPLATE_TYPE_FUNCTION;

            } else {
                assert(false); // TODO
            }

            def->def->visit(*this);
        }

        virtual void visit(variable_definition *def) throw() {
            name = def->name;
            entry = stab.lookup(top, name);

            // variable's can't be self-referential, so visit their
            // assignments first before assigning their names
            def->value->visit(*this);

            // type redefined in same scope
            if(nullptr != entry && entry->scope == top) {
                report_existing_symbol(def->location);
            } else {
                entry = stab.insert(top, name);
            }

            entry->origin.def = def;
            entry->type.base = def->value->type_;
        }

        /// visit a function definition. Because of the way things are brought
        /// into scope, template arguments are able to represent anonymously
        /// defined recursive types, or anonymous CRTP
        virtual void visit(function_definition *def) throw() {
            const size_t num_args(def->arg_names.size());
            const size_t num_tpl_args(def->template_arg_names.size());

            // introduce the names of template argument types
            for(size_t i(0); i < num_tpl_args; ++i) {
                name = def->template_arg_names[i].second;
                entry = stab.shallow_lookup(def->statements, name);
                if(nullptr != entry) {
                    report_existing_symbol(
                        def->template_arg_names[i].first,
                        *(entry->origin.location)
                    );
                } else {
                    entry = stab.insert(def->statements, name);
                }

                entry->origin.location = &(def->template_arg_names[i].first);
                if(def->template_arg_types->types[i]->is_instance<type_type_declaration>()) {
                    entry->name_type = symtab::TEMPLATE_TYPE_VARIABLE;
                    entry->type.base = ts.make_type_var();
                } else {
                    entry->name_type = symtab::TEMPLATE_VARIABLE;

                    // entry->type.base // TODO
                }
            }

            // then the types of the arguments, given the template arguments
            // as being in scope
            for(size_t i(0); i < num_tpl_args; ++i) {
                type_declaration *decl(def->template_arg_types->types[i]);
                if(!decl->is_instance<type_type_declaration>()
                && !decl->is_instance<unit_type_declaration>()) {
                    decl->visit(*this);
                }
            }
            for(size_t i(0); i < num_args; ++i) {
                type_declaration *decl(def->arg_types->types[i]);
                if(!decl->is_instance<type_type_declaration>()
                && !decl->is_instance<unit_type_declaration>()) {
                    decl->visit(*this);
                }
            }

            // then bring the names of argument types into scope; make sure they
            // don't clash with template argument names!
            for(size_t i(0); i < num_args; ++i) {
                name = def->arg_names[i].second;
                entry = stab.shallow_lookup(def->statements, name);
                if(nullptr != entry) {
                    report_existing_symbol(
                        def->arg_names[i].first,
                        *(entry->origin.location)
                    );
                } else {
                    entry = stab.insert(def->statements, name);
                }

                entry->origin.location = &(def->arg_names[i].first);
            }

            // then the body
            def->statements->visit(*this);
        }
    };

    /// resolve the definition locations of every name, look for undefined variables,
    /// and some other stuff
    void resolve_names(diagnostic_context &ctx, symbol_table &stab, type_system &ts, ast *ast_) throw() {
        if(nullptr != ast_) {
            name_resolver_visitor visitor(stab, ts, ctx);
            D( printf("about to resolve names\n"); )
            ast_->visit(visitor);
        }
    }
}}


