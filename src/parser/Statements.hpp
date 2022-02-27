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

template<typename ReturnType>
class StatementVisitor
{
  public:
    virtual ReturnType visit_print_statement(PrintStatement &statement)           = 0;
    virtual ReturnType visit_expression_statement(ExpressionStatement &statement) = 0;
    virtual ReturnType visit_var_statement(VarStatement &statement)               = 0;
    virtual ReturnType visit_block_statement(BlockStatement &statement)           = 0;
};
}// namespace tek::parser

#endif
