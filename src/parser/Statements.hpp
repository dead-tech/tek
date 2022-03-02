#ifndef STATEMENTS_HPP
#define STATEMENTS_HPP


#include "../tokenizer/Token.hpp"
#include "../types/Literal.hpp"
#include "Expressions.hpp"
#include <memory>
#include <utility>
#include <variant>

namespace tek::parser {

    template<typename ReturnType>
    class StatementVisitor;

    class Statement
    {
      public:
        virtual std::string accept(StatementVisitor<std::string> &visitor) = 0;
        virtual void        accept(StatementVisitor<void> &visitor)        = 0;

      protected:
        using ExpressionPtr = std::unique_ptr<Expression>;
    };

    class PrintStatement : public Statement
    {
      public:
        explicit PrintStatement(ExpressionPtr expression);

        std::string accept(StatementVisitor<std::string> &visitor) override;
        void        accept(StatementVisitor<void> &visitor) override;

      public:
        ExpressionPtr expression;
    };

    class ExpressionStatement : public Statement
    {
      public:
        explicit ExpressionStatement(ExpressionPtr expression);

        std::string accept(StatementVisitor<std::string> &visitor) override;
        void        accept(StatementVisitor<void> &visitor) override;

      public:
        ExpressionPtr expression;
    };

    class VarStatement : public Statement
    {
      public:
        VarStatement(tokenizer::Token name, ExpressionPtr initializer);

        std::string accept(StatementVisitor<std::string> &visitor) override;
        void        accept(StatementVisitor<void> &visitor) override;

      public:
        tokenizer::Token name;
        ExpressionPtr    initializer;
    };


    class BlockStatement : public Statement
    {
      private:
        using StatementPtr  = std::unique_ptr<Statement>;
        using StatementsVec = std::vector<StatementPtr>;

      public:
        explicit BlockStatement(StatementsVec statements);

        std::string accept(StatementVisitor<std::string> &visitor) override;
        void        accept(StatementVisitor<void> &visitor) override;

      public:
        StatementsVec statements;
    };

    class IfStatement : public Statement
    {
      private:
        using StatementPtr = std::unique_ptr<Statement>;

      public:
        IfStatement(ExpressionPtr condition, StatementPtr then_branch, StatementPtr else_branch);

        std::string accept(StatementVisitor<std::string> &visitor) override;
        void        accept(StatementVisitor<void> &visitor) override;

      public:
        ExpressionPtr condition;
        StatementPtr  then_branch;
        StatementPtr  else_branch;
    };

    class WhileStatement : public Statement
    {
      private:
        using StatementPtr = std::unique_ptr<Statement>;

      public:
        WhileStatement(ExpressionPtr condition, StatementPtr body);

        std::string accept(StatementVisitor<std::string> &visitor) override;
        void        accept(StatementVisitor<void> &visitor) override;

      public:
        ExpressionPtr condition;
        StatementPtr  body;
    };

    class ForStatement : public Statement
    {
      private:
        using StatementPtr = std::unique_ptr<Statement>;

      public:
        ForStatement(StatementPtr initializer, ExpressionPtr condition, StatementPtr body);

        std::string accept(StatementVisitor<std::string> &visitor) override;
        void        accept(StatementVisitor<void> &visitor) override;

      public:
        StatementPtr  initializer;
        ExpressionPtr condition;
        StatementPtr  body;
    };

    class FunctionStatement : public Statement
    {
      private:
        using StatementPtr  = std::unique_ptr<Statement>;
        using StatementsVec = std::vector<StatementPtr>;

      public:
        FunctionStatement(tokenizer::Token name, std::vector<tokenizer::Token> parameters, StatementsVec body);

        std::string accept(StatementVisitor<std::string> &visitor) override;
        void        accept(StatementVisitor<void> &visitor) override;

      public:
        tokenizer::Token              name;
        std::vector<tokenizer::Token> parameters;
        StatementsVec                 body;
    };

    class ReturnStatement : public Statement
    {
      private:
        using StatementPtr = std::unique_ptr<Statement>;

      public:
        ReturnStatement(tokenizer::Token keyword, ExpressionPtr expression);

        std::string accept(StatementVisitor<std::string> &visitor) override;
        void        accept(StatementVisitor<void> &visitor) override;

      public:
        tokenizer::Token keyword;
        ExpressionPtr    expression;
    };

    template<typename ReturnType>
    class StatementVisitor
    {
      public:
        virtual ReturnType visit_print_statement(PrintStatement &statement)           = 0;
        virtual ReturnType visit_expression_statement(ExpressionStatement &statement) = 0;
        virtual ReturnType visit_var_statement(VarStatement &statement)               = 0;
        virtual ReturnType visit_block_statement(BlockStatement &statement)           = 0;
        virtual ReturnType visit_if_statement(IfStatement &statement)                 = 0;
        virtual ReturnType visit_while_statement(WhileStatement &statement)           = 0;
        virtual ReturnType visit_for_statement(ForStatement &statement)               = 0;
        virtual ReturnType visit_function_statement(FunctionStatement &statement)     = 0;
        virtual ReturnType visit_return_statement(ReturnStatement &statement)         = 0;
    };
}// namespace tek::parser

#endif
