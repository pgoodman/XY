/*
 * ast.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#include "xy/include/ast.hpp"
#include "xy/include/symbol_table.hpp"

namespace xy {

    ast::ast(void) throw() { }

    ast::~ast(void) throw() { }

    void ast::print(std::ostream &, symbol_table &) throw() { }

#   define XY_AST_IN_CPP
#   include "xy/include/ast.inc"

#define XY_AST_DEFINE_VISITORS(type) \
    void ast_visitor::visit(type *) throw() { }

    XY_AST_TYPES(XY_AST_DEFINE_VISITORS)

#define XY_AST_VISITOR_SWITCH(type) \
    if(type_id == type::static_id()) { \
        this->visit(ast_->reinterpret<type>()); \
        return; \
    }

    void ast_visitor::visit(ast *ast_) throw() {
        if(nullptr != ast_) {
            ast_->visit(*this);
        }
        //const ast_type type_id(ast_->type_id());
        //XY_AST_TYPES(XY_AST_VISITOR_SWITCH)
    }
}


