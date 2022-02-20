#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "../../tokenizer/Token.hpp"
#include "../../types/Literal.hpp"
#include <variant>

// TODO: split into own file
// TODO: use namespaces

template <typename ReturnType> class Visitor;

class Expression {
  public:
    virtual std::string accept(Visitor<std::string>& visitor) = 0;

  protected:
    // TODO: use unique_ptr
    using ExpressionPtr = Expression*;
};

class BinaryExpression : public Expression {
  public:
    BinaryExpression(ExpressionPtr left, Token op, ExpressionPtr right);

    std::string accept(Visitor<std::string>& visitor) override;

  public:
    ExpressionPtr left;
    Token op;
    ExpressionPtr right;
};

class GroupingExpression : public Expression {
  public:
    explicit GroupingExpression(ExpressionPtr expression);

    std::string accept(Visitor<std::string>& visitor) override;

  public:
    ExpressionPtr expression;
};

class LiteralExpression : public Expression {
  public:
    explicit LiteralExpression(Literal::variant_t literal);

    std::string accept(Visitor<std::string>& visitor) override;

  public:
    Literal literal;
};

class UnaryExpression : public Expression {
  public:
    UnaryExpression(Token op, ExpressionPtr right);

    std::string accept(Visitor<std::string>& visitor) override;

  public:
    Token op;
    ExpressionPtr right;
};

template <typename ReturnType> class Visitor {
  public:
    virtual ReturnType
    visit_binary_expression(BinaryExpression& expression) = 0;
    virtual ReturnType
    visit_grouping_expression(GroupingExpression& expression) = 0;
    virtual ReturnType
    visit_literal_expression(LiteralExpression& expression) = 0;
    virtual ReturnType visit_unary_expression(UnaryExpression& expression) = 0;
};

#endif
