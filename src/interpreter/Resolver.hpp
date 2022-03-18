#ifndef TEK_RESOLVER_HPP
#define TEK_RESOLVER_HPP

#include "../parser/Expressions.hpp"
#include "../parser/Statements.hpp"
#include "../tokenizer/Token.hpp"
#include "../utils/iterable_stack.hpp"
#include "Interpreter.hpp"
#include <stack>

namespace tek::interpreter {
    class Resolver
      : public parser::ExpressionVisitor<void>
      , public parser::StatementVisitor<void>
    {
      private:
        using StatementPtr  = std::unique_ptr<parser::Statement>;
        using ExpressionPtr = std::unique_ptr<parser::Expression>;
        using StatementsVec = std::vector<StatementPtr>;
        using Scope         = std::unordered_map<std::string, bool>;
        using ScopesStack   = utils::iterable_stack<Scope>;

      public:
        explicit Resolver(Interpreter interpreter);
        void resolve(const StatementsVec &statements);

        // Expressions
      public:
        void visit_var_expression(parser::VarExpression &expression) override;
        void visit_assign_expression(parser::AssignExpression &expression) override;

        void visit_binary_expression(parser::BinaryExpression &expression) override;
        void visit_call_expression(parser::CallExpression &expression) override;
        void visit_grouping_expression(parser::GroupingExpression &expression) override;
        void visit_literal_expression(parser::LiteralExpression &expression) override;
        void visit_logical_expression(parser::LogicalExpression &expression) override;
        void visit_unary_expression(parser::UnaryExpression &expression) override;

        // Statements
      public:
        void visit_block_statement(parser::BlockStatement &statement) override;
        void visit_var_statement(parser::VarStatement &statement) override;
        void visit_function_statement(parser::FunctionStatement &statement) override;

        void visit_expression_statement(parser::ExpressionStatement &statement) override;
        void visit_if_statement(parser::IfStatement &statement) override;
        void visit_print_statement(parser::PrintStatement &statement) override;
        void visit_return_statement(parser::ReturnStatement &statement) override;
        void visit_while_statement(parser::WhileStatement &statement) override;
        void visit_for_statement(parser::ForStatement &statement) override;

        // TODO: Move this
      private:
        enum class FunctionType {
            NONE = 0,
            FUNCTION,
        };

      private:
        void begin_scope();
        void end_scope();
        void resolve(const StatementPtr &statement);
        void resolve(const ExpressionPtr &statement);

        void declare(const tokenizer::Token &name);
        void define(const tokenizer::Token &name);

        void resolve_local(parser::Expression *expression, const tokenizer::Token &name);
        void resolve_function(const parser::FunctionStatement &function, const FunctionType &function_type);

      private:
        Interpreter  interpreter;
        ScopesStack  scopes;
        FunctionType current_function = FunctionType::NONE;
    };
}// namespace tek::interpreter

#endif// TEK_RESOLVER_HPP
