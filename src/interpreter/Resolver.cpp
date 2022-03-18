#include "Resolver.hpp"

#include <utility>

namespace tek::interpreter {

    Resolver::Resolver(Interpreter interpreter) : interpreter{ std::move(interpreter) } {}

    void Resolver::resolve(const StatementsVec &statements)
    {
        for (const auto &statement : statements) { this->resolve(statement); }
    }

    void Resolver::visit_var_expression(parser::VarExpression &expression)
    {
        if (!this->scopes.empty() && !this->scopes.top().at(expression.name.lexeme)) {
            logger::Logger::error(expression.name, "Can't read local variable in its own initializer.");
        }

        this->resolve_local(&expression, expression.name);
    }

    void Resolver::visit_assign_expression(parser::AssignExpression &expression)
    {
        this->resolve(expression.value);
        const auto name = expression.name;
        this->resolve_local(&expression, name);
    }

    void Resolver::visit_binary_expression(parser::BinaryExpression &expression)
    {
        this->resolve(expression.left);
        this->resolve(expression.right);
    }

    void Resolver::visit_call_expression(parser::CallExpression &expression)
    {
        this->resolve(expression.callee);

        for (const auto &argument : expression.arguments) { this->resolve(argument); }
    }

    void Resolver::visit_grouping_expression(parser::GroupingExpression &expression)
    {
        this->resolve(expression.expression);
    }

    void Resolver::visit_literal_expression(parser::LiteralExpression &expression) { return; }

    void Resolver::visit_logical_expression(parser::LogicalExpression &expression)
    {
        this->resolve(expression.left);
        this->resolve(expression.right);
    }

    void Resolver::visit_unary_expression(parser::UnaryExpression &expression) { this->resolve(expression.right); }

    void Resolver::visit_block_statement(parser::BlockStatement &statement)
    {
        this->begin_scope();
        this->resolve(statement.statements);
        this->end_scope();
    }

    void Resolver::visit_var_statement(parser::VarStatement &statement)
    {
        this->declare(statement.name);

        if (statement.initializer != nullptr) { this->resolve(statement.initializer); }

        this->define(statement.name);
    }

    void Resolver::visit_function_statement(parser::FunctionStatement &statement)
    {
        this->declare(statement.name);
        this->define(statement.name);

        this->resolve_function(statement, FunctionType::FUNCTION);
    }

    void Resolver::visit_expression_statement(parser::ExpressionStatement &statement)
    {
        this->resolve(statement.expression);
    }

    void Resolver::visit_if_statement(parser::IfStatement &statement)
    {
        this->resolve(statement.condition);
        this->resolve(statement.then_branch);
        if (statement.else_branch != nullptr) { this->resolve(statement.else_branch); }
    }

    void Resolver::visit_print_statement(parser::PrintStatement &statement) { this->resolve(statement.expression); }

    void Resolver::visit_return_statement(parser::ReturnStatement &statement)
    {
        if (this->current_function == FunctionType::NONE) {
            logger::Logger::error(statement.keyword, "Can't return from top-level code");
        }

        if (statement.expression != nullptr) { this->resolve(statement.expression); }
    }

    void Resolver::visit_while_statement(parser::WhileStatement &statement)
    {
        this->resolve(statement.condition);
        this->resolve(statement.body);
    }

    void Resolver::visit_for_statement(parser::ForStatement &statement)
    {
        this->resolve(statement.initializer);
        this->resolve(statement.condition);
        this->resolve(statement.body);
    }

    void Resolver::begin_scope() { this->scopes.push(Scope{}); }

    void Resolver::end_scope() { this->scopes.pop(); }

    void Resolver::resolve(const StatementPtr &statement) { statement->accept(*this); }

    void Resolver::resolve(const ExpressionPtr &statement) { statement->accept(*this); }

    void Resolver::declare(const tokenizer::Token &name)
    {
        if (this->scopes.empty()) { return; }

        auto &current_scope = this->scopes.top();

        if (const auto it = current_scope.find(name.lexeme); it != current_scope.end()) {
            logger::Logger::error(name, "A variable with this name already exists in this scope");
        }

        current_scope.insert_or_assign(name.lexeme, false);
    }

    void Resolver::define(const tokenizer::Token &name)
    {
        if (this->scopes.empty()) { return; }

        auto &current_scope = this->scopes.top();
        current_scope.insert_or_assign(name.lexeme, true);
    }

    void Resolver::resolve_local(parser::Expression *expression, const tokenizer::Token &name)
    {
        // This might not work because of how the iterators for the iterable stack are implemented
        for (const auto &scope : this->scopes) {
            if (const auto it = scope.find(name.lexeme); it != scope.end()) {
                // This might not work because of the logic with the difference of ITs
                this->interpreter.resolve(expression, this->scopes.size() - 1 - std::distance(it, scope.begin()));
            }
        }
    }

    void Resolver::resolve_function(const parser::FunctionStatement &function, const FunctionType &function_type)
    {
        const auto enclosing_function = this->current_function;
        this->current_function        = function_type;

        this->begin_scope();

        for (const auto &param : function.parameters) {
            this->declare(param);
            this->define(param);
        }

        this->resolve(function.body);

        this->end_scope();
        this->current_function = enclosing_function;
    }

}// namespace tek::interpreter
