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

namespace xy { namespace pass {

    /// set up an expectation of a name
    struct name_expectation {
    public:
        symtab::symbol expected_name;
        ast *expected_by;
        io::message_id message;
    };

    /// list of expected names
    struct name_expectation_list : public std::list<name_expectation> {
    public:
        void push_back(symtab::symbol sym_, ast *ast_, io::message_id id_) throw() {
            name_expectation exp;
            exp.expected_by = ast_;
            exp.expected_name = sym_;
            exp.message = id_;
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

        name_resolver_visitor(symbol_table &stab_, diagnostic_context &ctx_)
            : ast_visitor()
            , expected_names()
            , top(nullptr)
            , stab(stab_)
            , ctx(ctx_)
        { }

        virtual ~name_resolver_visitor(void) throw() { }

        /// visit a list of statements
        virtual void visit(conjunctive_statement *ls) throw() {
            printf("visiting conjunctive statement\n");
            conjunctive_statement *old_top(top);
            top = ls;
            statement *stmt(nullptr);

            const size_t expected_orig_size(expected_names.size());
            const size_t num_stmts(ls->statements.size());
            symtab::entry *entry(nullptr);
            definition_statement *def_stmt(nullptr);
            symtab::entry_type def_type(symtab::NONE);
            printf("a\n");
            // visit all sub-statements to find the names that should be
            // "pulled up"
            for(size_t i(0); i < num_stmts; ++i) {
                printf("b\n");
                stmt = ls->statements[i];
                def_stmt = stmt->reinterpret<definition_statement>();
                printf("stmt = %p\n", reinterpret_cast<void *>(stmt));
                // expression statement, conjunctive statement
                if(nullptr == def_stmt) {
                    printf("wtf?\n");
                    continue;

                // simple type def
                } else if(stmt->is_instance<type_definition>()) {

                    printf("type def\n");
                    printf("stmt->type_id=%llu & type_definition::exact_id=%llu = %llu\n", stmt->type_id(), type_definition::exact_id(), stmt->type_id() & type_definition::exact_id());

                    printf("variable_definition::type_id = %llu\n", variable_definition::static_id());
                    printf("variable_definition::exact_id = %llu\n", variable_definition::exact_id());
                    printf("definition_statement::exact_id = %llu\n", definition_statement::exact_id());
                    printf("statement::exact_id = %llu\n", statement::exact_id());
                    printf("ast::exact_id = %llu\n", ast::exact_id());

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

                    printf("structured type def\n");

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

                    printf("function def\n");

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
                    printf("found top-level var def\n");
                    printf("   var name is %s\n", stab[def_stmt->name]);

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
                    ctx.report_here(entry->origin->reinterpret<definition_statement>()->location, io::n_duplicate_symbol);
                }
            }

            // visit
            for(size_t i(0); i < num_stmts; ++i) {
                stmt = ls->statements[i];
            }

            // ensure all expected things are found
            for(; expected_names.size() > expected_orig_size; expected_names.pop_front()) {

            }

            top = old_top;
        }

        /// visit an expression that is a variable name
        virtual void visit(name_expression *expr) throw() {
            symtab::entry *entry(stab.lookup(top, expr->name));
            if(nullptr == entry) {
                expected_names.push_back(expr->name, expr, io::e_unknown_var);
            } else {
                expr->type_ = entry->type_;
            }
        }
    };

    /// resolve the definition locations of every name, look for undefined variables,
    /// and some other stuff
    void resolve_names(diagnostic_context &ctx, symbol_table &stab, ast *ast_) throw() {
        if(nullptr != ast_) {
            name_resolver_visitor visitor(stab, ctx);
            printf("about to visit\n");
            ast_->visit(visitor);
        }
    }
}}


