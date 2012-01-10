
#ifdef XY_AST_IN_CPP

/// specify a constructor and which fields it should construct
#   define XY_AST_CONSTRUCTOR(class_name, ...) \
    class_name::class_name (XY_CAT(XY_AST_CONST_ARG_COMMA_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_CONST_ARG, void, __VA_ARGS__)) throw() { \
        XY_CAT(XY_AST_CONST_ARG_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_CONST_ARG_INIT, void, __VA_ARGS__) \
    }

#   define XY_AST_MEMBER_VAR(type, var)
#   define XY_AST_MEMBERS(A, ...) 

/// specify a destructor and which fields should be recursively destroyed
#   define XY_AST_DESTRUCTOR(class_name, ...) \
    class_name::~class_name(void) throw() { \
        XY_CAT(XY_AST_CONST_ARG_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_DESTROY_FIELD, void, __VA_ARGS__) \
    }

#   define XY_AST_DEFAULT_DESTRUCTOR(class_name) \
    class_name::~class_name(void) throw() { }

/// define a default constructor, with the initialization specialized with tuples,
/// e.g. (var, val).
#   define XY_AST_DEFAULT_CONSTRUCTOR(class_name, ...) \
    class_name::class_name(void) throw() \
        : XY_CAT(XY_AST_CONST_DEFAUL_ARG_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_CONST_DEFAULT_ARG_INIT, __VA_ARGS__) \
    { }

/// define an empty constructor
#   define XY_AST_EMPTY_CONSTRUCTOR(class_name) \
    class_name::class_name(void) throw() { }

#   define XY_AST_INHERIT_CONSTRUCTOR(class_name, parent_class)

/// make a constructor that forwords all of its arguments to a base class
/// constructor
#   define XY_AST_FORWARD_CONSTRUCTOR(class_name, parent_class, ...) \
    class_name::class_name (XY_CAT(XY_AST_CONST_ARG_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_PARENT_CONST_ARG, parent_class, __VA_ARGS__)) throw() { \
        XY_CAT(XY_AST_CONST_ARG_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_PARENT_CONST_ARG_INIT, parent_class, __VA_ARGS__) ; \
    }

#   define XY_AST_METHOD_DEF(class_name, virt, return_type, method_name, args, body) \
    return_type class_name::method_name args throw() body

#   define XY_AST_BEGIN(base, super) \
    void base::visit(ast_visitor &visitor) throw() { \
        visitor.visit(this); \
    } \
    const char *base::class_name(void) const throw() { \
        return #base; \
    }

#   define XY_AST_END

// in the header
#else

/// specify a constructor and which fields it should construct
#   define XY_AST_CONSTRUCTOR(class_name, ...) \
    class_name (XY_CAT(XY_AST_CONST_ARG_COMMA_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_CONST_ARG, void, __VA_ARGS__)) throw();

/// specify an AST member variable
#   define XY_AST_MEMBER_VAR(type, var) \
    type var ;

/// specify the member variables of an xy ast type
#   define XY_AST_MEMBERS(A, ...) \
    XY_CAT(XY_AST_MEMBER_UNROLL_, XY_NARG(__VA_ARGS__))(A, __VA_ARGS__)

/// specify a destructor and which fields should be recursively destroyed
#   define XY_AST_DESTRUCTOR(class_name, ...) \
    virtual ~class_name(void) throw();

#   define XY_AST_DEFAULT_DESTRUCTOR(class_name) \
    virtual ~class_name(void) throw();

/// define a default constructor, with the initialization specialized with tuples,
/// e.g. (var, val).
#   define XY_AST_DEFAULT_CONSTRUCTOR(class_name, ...) \
    class_name(void) throw();

/// define an empty constructor
#   define XY_AST_EMPTY_CONSTRUCTOR(class_name) \
    class_name(void) throw();

/// inherit a base class constructor
#   define XY_AST_INHERIT_CONSTRUCTOR(class_name, parent_class) \
    using mpl::rtti<ast_type_category,class_name,parent_class>::rtti;

/// make a constructor that forwords all of its arguments to a base class
/// constructor
#   define XY_AST_FORWARD_CONSTRUCTOR(class_name, parent_class, ...) \
    class_name (XY_CAT(XY_AST_CONST_ARG_UNROLL_, XY_NARG(__VA_ARGS__))(XY_AST_PARENT_CONST_ARG, parent_class, __VA_ARGS__)) throw();

#   define XY_AST_METHOD_DEF(class_name, virt, return_type, method_name, args, body) \
    virt return_type method_name args throw();

#   define XY_AST_BEGIN(base, super) \
    struct base : public mpl::rtti<ast_type_category, base, super> { \
    public: \
        virtual void visit(ast_visitor &visitor) throw(); \
        virtual const char *class_name(void) const throw();

#   define XY_AST_END };

#endif

/// top-level expression AST type
XY_AST_BEGIN(expression, ast)
    XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
        (type *,    type_),
        (token,     location))

    XY_AST_DEFAULT_CONSTRUCTOR(expression, (type_, nullptr))
    XY_AST_CONSTRUCTOR(expression, type_)
    XY_AST_DEFAULT_DESTRUCTOR(expression)
XY_AST_END

    XY_AST_BEGIN(name_expression, expression)

        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (symtab::symbol, name))

        XY_AST_CONSTRUCTOR(name_expression, name)
        XY_AST_DEFAULT_DESTRUCTOR(name_expression)
    XY_AST_END

    XY_AST_BEGIN(function_call_expression, expression)
        // it is possible that we will get some arguments, categorized as
        // template arguments, that are all expressions, but are not known
        // if they are template args or function args
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (expression *,              function),
            (std::vector<ast *>,        template_parameters),
            (std::vector<expression *>, arguments),
            (bool,                      might_be_ambiguous))

        XY_AST_CONSTRUCTOR(function_call_expression, function)
        XY_AST_DESTRUCTOR(function_call_expression, function, template_parameters, arguments)
    XY_AST_END

    XY_AST_BEGIN(type_instance_expression, expression)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (type_declaration *,               declaration),
            (std::vector<expression *>, values))

        XY_AST_CONSTRUCTOR(type_instance_expression, declaration)
        XY_AST_DESTRUCTOR(type_instance_expression, declaration, values)
    XY_AST_END

    XY_AST_BEGIN(array_access_expression, expression)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (expression *,   array),
            (expression *,   index))

        XY_AST_CONSTRUCTOR(array_access_expression, array, index)
        XY_AST_DESTRUCTOR(array_access_expression, array, index)
    XY_AST_END

    XY_AST_BEGIN(infix_expression, expression)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (expression *,   left),
            (expression *,   right))

        XY_AST_CONSTRUCTOR(infix_expression, left, right)
        XY_AST_DESTRUCTOR(infix_expression, left, right)
    XY_AST_END

    XY_AST_BEGIN(prefix_expression, expression)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (expression *,   right))

        XY_AST_CONSTRUCTOR(prefix_expression, right)
        XY_AST_DESTRUCTOR(prefix_expression, right)
    XY_AST_END

    XY_AST_BEGIN(literal_expression, expression)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (const char *,   data))

        XY_AST_DEFAULT_CONSTRUCTOR(literal_expression, (data, nullptr))
        XY_AST_CONSTRUCTOR(literal_expression, data)
        XY_AST_DESTRUCTOR(literal_expression, data)
    XY_AST_END

        XY_AST_BEGIN(integer_literal_expression, literal_expression)
            XY_AST_FORWARD_CONSTRUCTOR(integer_literal_expression, literal_expression, data)
            XY_AST_DEFAULT_DESTRUCTOR(integer_literal_expression)
        XY_AST_END

        XY_AST_BEGIN(rational_literal_expression, literal_expression)
            XY_AST_FORWARD_CONSTRUCTOR(rational_literal_expression, literal_expression, data)
            XY_AST_DEFAULT_DESTRUCTOR(rational_literal_expression)
        XY_AST_END

        XY_AST_BEGIN(string_literal_expression, literal_expression)
            XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
                (unsigned,  byte_length))

            XY_AST_METHOD_DEF(string_literal_expression,
                XY_NOTHING, XY_NOTHING, string_literal_expression,
                (const char *data_, unsigned len_),
                {
                    this->data = data_;
                    this->byte_length = len_;
                })

            XY_AST_DEFAULT_DESTRUCTOR(string_literal_expression)
        XY_AST_END

XY_AST_BEGIN(statement, ast)
    XY_AST_EMPTY_CONSTRUCTOR(statement)
    XY_AST_DEFAULT_DESTRUCTOR(statement)
XY_AST_END

    XY_AST_BEGIN(conjunctive_statement, statement)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (std::vector<statement *>,  statements),
            (conjunctive_statement *,   parent_scope),
            (name_binding,              bound_names))

        XY_AST_DEFAULT_CONSTRUCTOR(conjunctive_statement,
            (statements,    XY_NOTHING),
            (parent_scope,  nullptr),
            (bound_names,   XY_NOTHING))

        XY_AST_DESTRUCTOR(conjunctive_statement, statements)
    XY_AST_END

    XY_AST_BEGIN(expression_statement, statement)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (expression *,  expression))

        XY_AST_CONSTRUCTOR(expression_statement, expression)
        XY_AST_DESTRUCTOR(expression_statement, expression)
    XY_AST_END

    XY_AST_BEGIN(definition_statement, statement)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (token,             location),
            (symtab::symbol,    name))

        XY_AST_EMPTY_CONSTRUCTOR(definition_statement)
        XY_AST_DEFAULT_DESTRUCTOR(definition_statement)
    XY_AST_END

        /// simple type definition
        XY_AST_BEGIN(type_definition, definition_statement)
            XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
                (type_declaration *,       declaration))

            XY_AST_CONSTRUCTOR(type_definition, location, name, declaration)
            XY_AST_DESTRUCTOR(type_definition, declaration)
        XY_AST_END
        
        /// complex type definition
        XY_AST_BEGIN(structured_type_definition, definition_statement)
            XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
                (definition_statement *,    def))

            XY_AST_CONSTRUCTOR(structured_type_definition, location, name, def)
            XY_AST_DESTRUCTOR(structured_type_definition, def)
        XY_AST_END

        /// simple variable definition
        XY_AST_BEGIN(variable_definition, definition_statement)
            XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
                (expression *,   value))

            XY_AST_CONSTRUCTOR(variable_definition, location, name, value)
            XY_AST_DESTRUCTOR(variable_definition, value)
        XY_AST_END

        /// function definition/declaration
        XY_AST_BEGIN(function_definition, definition_statement)
            XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
                (arrow_type_declaration *, template_arg_types),
                (arrow_type_declaration *, arg_types),
                (name_list,         template_arg_names),
                (name_list,         arg_names),
                (conjunctive_statement *,  statements))

            XY_AST_DEFAULT_CONSTRUCTOR(function_definition,
                (template_arg_types,    nullptr),
                (arg_types,             nullptr),
                (template_arg_names,    XY_NOTHING),
                (arg_names,             XY_NOTHING),
                (statements,            nullptr))

            XY_AST_DESTRUCTOR(function_definition,
                template_arg_types, arg_types, statements)
        XY_AST_END

    XY_AST_BEGIN(return_expression_statement, statement)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (expression *,  value))

        XY_AST_CONSTRUCTOR(return_expression_statement, value)
        XY_AST_DESTRUCTOR(return_expression_statement, value)
    XY_AST_END

    XY_AST_BEGIN(return_type_statement, statement)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (type_declaration *,   value))

        XY_AST_CONSTRUCTOR(return_type_statement, value)
        XY_AST_DESTRUCTOR(return_type_statement, value)
    XY_AST_END

XY_AST_BEGIN(type_declaration, statement)
    XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
        (token, location),
        (type *, type_))

    XY_AST_EMPTY_CONSTRUCTOR(type_declaration)
    XY_AST_DEFAULT_DESTRUCTOR(type_declaration)
    XY_AST_METHOD_DEF(type_declaration, virtual, bool, returns_type,
        (void),
        { return false; })
XY_AST_END

    XY_AST_BEGIN(template_instance_type_declaration, type_declaration)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (type_declaration *,    template_type),
            (std::vector<ast *>,    parameters))

        XY_AST_CONSTRUCTOR(template_instance_type_declaration, template_type)
        XY_AST_DESTRUCTOR(template_instance_type_declaration, template_type, parameters)

        XY_AST_METHOD_DEF(template_instance_type_declaration, virtual, void, print,
            (std::ostream &os, symbol_table &stab),
            {
                template_type->print(os, stab);
                os << "(";
                support::print_separated_list(parameters, ", ", os, stab);
                os << ")";
            })
    XY_AST_END

    XY_AST_BEGIN(named_type_declaration, type_declaration)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (symtab::symbol,    name))

        XY_AST_CONSTRUCTOR(named_type_declaration, name)
        XY_AST_DEFAULT_DESTRUCTOR(named_type_declaration)
        XY_AST_METHOD_DEF(named_type_declaration, virtual, void, print,
            (std::ostream &os, symbol_table &stab),
            {
                os << stab[name];
            })
    XY_AST_END

    XY_AST_BEGIN(type_type_declaration, type_declaration)
        XY_AST_METHOD_DEF(type_type_declaration, virtual, void, print,
            (std::ostream &os, symbol_table &),
            {
                os << "Type";
            })
    XY_AST_END

    XY_AST_BEGIN(unit_type_declaration, type_declaration)
        XY_AST_METHOD_DEF(unit_type_declaration, virtual, void, print,
            (std::ostream &os, symbol_table &),
            {
                os << "Unit";
            })
    XY_AST_END

    XY_AST_BEGIN(array_type_declaration, type_declaration)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (type_declaration *,   inner_type))

        XY_AST_CONSTRUCTOR(array_type_declaration, inner_type)
        XY_AST_DESTRUCTOR(array_type_declaration, inner_type)
        XY_AST_METHOD_DEF(array_type_declaration, virtual, void, print,
            (std::ostream &os, symbol_table &stab),
            {
                os << "[";
                inner_type->print(os, stab);
                os << "]";
            })
    XY_AST_END

    XY_AST_BEGIN(reference_type_declaration, type_declaration)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (type_declaration *,    inner_type))

        XY_AST_CONSTRUCTOR(reference_type_declaration, inner_type)
        XY_AST_DESTRUCTOR(reference_type_declaration, inner_type)
        XY_AST_METHOD_DEF(reference_type_declaration, virtual, void, print,
            (std::ostream &os, symbol_table &stab),
            {
                os << "&";
                if(inner_type->is_instance<named_type_declaration>()
                || inner_type->is_instance<reference_type_declaration>()
                || inner_type->is_instance<array_type_declaration>()) {
                    inner_type->print(os, stab);
                } else {
                    os << "(";
                    inner_type->print(os, stab);
                    os << ")";
                }
            })
    XY_AST_END

    XY_AST_BEGIN(binary_type_declaration, type_declaration)
        XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
            (std::vector<type_declaration *>,  types),
            (bool,                      is_wrapped))
        XY_AST_DEFAULT_CONSTRUCTOR(binary_type_declaration, (types, XY_NOTHING), (is_wrapped, false))
        XY_AST_DESTRUCTOR(binary_type_declaration, types)
    XY_AST_END

        XY_AST_BEGIN(sum_type_declaration, binary_type_declaration)
            XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
                (name_list,  params),
                (name_list,  fields))

            XY_AST_DEFAULT_CONSTRUCTOR(sum_type_declaration, (params, XY_NOTHING), (fields, XY_NOTHING))
            XY_AST_DEFAULT_DESTRUCTOR(sum_type_declaration)
            XY_AST_METHOD_DEF(sum_type_declaration, virtual, void, print,
                (std::ostream &os, symbol_table &stab),
                {
                    if(this->is_wrapped) os << "(";
                    support::print_separated_list(this->types, " + ", os, stab);
                    if(this->is_wrapped) os << ")";
                })
        XY_AST_END

        XY_AST_BEGIN(product_type_declaration, binary_type_declaration)
            XY_AST_MEMBERS(XY_AST_MEMBER_VAR,
                (name_list,  params),
                (name_list,  fields))

            XY_AST_DEFAULT_CONSTRUCTOR(product_type_declaration, (params, XY_NOTHING), (fields, XY_NOTHING))
            XY_AST_DEFAULT_DESTRUCTOR(product_type_declaration)

            XY_AST_METHOD_DEF(product_type_declaration, virtual, void, print,
                (std::ostream &os, symbol_table &stab),
                {
                    if(this->is_wrapped) os << "(";
                    support::print_separated_list(this->types, " * ", os, stab);
                    if(this->is_wrapped) os << ")";
                })
        XY_AST_END

        XY_AST_BEGIN(arrow_type_declaration, binary_type_declaration)
            XY_AST_DEFAULT_DESTRUCTOR(arrow_type_declaration)
            XY_AST_METHOD_DEF(arrow_type_declaration, virtual, void, print,
                (std::ostream &os, symbol_table &stab),
                {
                    if(this->is_wrapped) os << "(";
                    support::print_separated_list(this->types, " -> ", os, stab);
                    if(this->is_wrapped) os << ")";
                })

            /// is this a template type? i.e. do we generate something
            /// that is a type or is somehting that generates a type?
            XY_AST_METHOD_DEF(arrow_type_declaration, virtual, bool, returns_type,
                (void),
                {
                    if(types.empty()) {
                        return false;
                    }

                    type_declaration *return_type(types.back());

                    if(return_type->is_instance<type_type_declaration>()) {
                        return true;
                    }

                    return return_type->returns_type();
                })
        XY_AST_END

#undef XY_AST_CONSTRUCTOR
#undef XY_AST_MEMBER_VAR
#undef XY_AST_MEMBERS
#undef XY_AST_DESTRUCTOR
#undef XY_AST_DEFAULT_DESTRUCTOR
#undef XY_AST_DEFAULT_CONSTRUCTOR
#undef XY_AST_EMPTY_CONSTRUCTOR
#undef XY_AST_INHERIT_CONSTRUCTOR
#undef XY_AST_FORWARD_CONSTRUCTOR
#undef XY_AST_METHOD_DEF
#undef XY_AST_BEGIN
#undef XY_AST_END

