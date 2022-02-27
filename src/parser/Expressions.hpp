#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "../tokenizer/Token.hpp"
#include "../types/Literal.hpp"
#include <memory>
#include <variant>

namespace tek::parser {

template<typename ReturnType>
class Visitor;

class Expression
{
  public:
    virtual std::string         accept(Visitor<std::string> &visitor)         = 0;
    virtual tek::types::Literal accept(Visitor<tek::types::Literal> &visitor) = 0;

  protected:
    using ExpressionPtr = std::unique_ptr<Expression>;
};

class BinaryExpression : public Expression
{
  public:
    BinaryExpression(ExpressionPtr left, tokenizer::Token op, ExpressionPtr right);

    std::string    accept(Visitor<std::string> &visitor) override;
    types::Literal accept(Visitor<types::Literal> &visitor) override;

  public:
    ExpressionPtr    left;
    tokenizer::Token op;
    ExpressionPtr    right;
};

class GroupingExpression : public Expression
{
  public:
    explicit GroupingExpression(ExpressionPtr expression);

    std::string    accept(Visitor<std::string> &visitor) override;
    types::Literal accept(Visitor<types::Literal> &visitor) override;

  public:
    ExpressionPtr expression;
};

class LiteralExpression : public Expression
{
  public:
    explicit LiteralExpression(types::Literal::variant_t literal);

    std::string    accept(Visitor<std::string> &visitor) override;
    types::Literal accept(Visitor<types::Literal> &visitor) override;

  public:
    types::Literal literal;
};

class UnaryExpression : public Expression
{
  public:
    UnaryExpression(tokenizer::Token op, ExpressionPtr right);

    std::string    accept(Visitor<std::string> &visitor) override;
    types::Literal accept(Visitor<types::Literal> &visitor) override;

  public:
    tokenizer::Token op;
    ExpressionPtr    right;
};

template<typename ReturnType>
class Visitor
{
  public:
    virtual ReturnType visit_binary_expression(BinaryExpression &expression)     = 0;
    virtual ReturnType visit_grouping_expression(GroupingExpression &expression) = 0;
    virtual ReturnType visit_literal_expression(LiteralExpression &expression)   = 0;
    virtual ReturnType visit_unary_expression(UnaryExpression &expression)       = 0;
};
}// namespace tek::parser

#endif
