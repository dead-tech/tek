#include "Expressions.hpp"

#include <utility>
namespace tek::parser {
BinaryExpression::BinaryExpression(Expression::ExpressionPtr left, tokenizer::Token op, Expression::ExpressionPtr right)
  : left{ std::move(left) }, op{ std::move(op) }, right{ std::move(right) }
{}

std::string BinaryExpression::accept(ExpressionVisitor<std::string> &visitor)
{
    return visitor.visit_binary_expression(*this);
}

types::Literal BinaryExpression::accept(ExpressionVisitor<types::Literal> &visitor)
{
    return visitor.visit_binary_expression(*this);
}

void BinaryExpression::accept(ExpressionVisitor<void> &visitor) { return visitor.visit_binary_expression(*this); }

GroupingExpression::GroupingExpression(Expression::ExpressionPtr expression) : expression{ std::move(expression) } {}

std::string GroupingExpression::accept(ExpressionVisitor<std::string> &visitor)
{
    return visitor.visit_grouping_expression(*this);
}

types::Literal GroupingExpression::accept(ExpressionVisitor<types::Literal> &visitor)
{
    return visitor.visit_grouping_expression(*this);
}

void GroupingExpression::accept(ExpressionVisitor<void> &visitor) { return visitor.visit_grouping_expression(*this); }

LiteralExpression::LiteralExpression(types::Literal::variant_t literal) : literal{ std::move(literal) } {}

std::string LiteralExpression::accept(ExpressionVisitor<std::string> &visitor)
{
    return visitor.visit_literal_expression(*this);
}

types::Literal LiteralExpression::accept(ExpressionVisitor<types::Literal> &visitor)
{
    return visitor.visit_literal_expression(*this);
}

void LiteralExpression::accept(ExpressionVisitor<void> &visitor) { return visitor.visit_literal_expression(*this); }

UnaryExpression::UnaryExpression(tokenizer::Token op, ExpressionPtr right)
  : op{ std::move(op) }, right{ std::move(right) }
{}

std::string UnaryExpression::accept(ExpressionVisitor<std::string> &visitor)
{
    return visitor.visit_unary_expression(*this);
}

types::Literal UnaryExpression::accept(ExpressionVisitor<types::Literal> &visitor)
{
    return visitor.visit_unary_expression(*this);
}

void UnaryExpression::accept(ExpressionVisitor<void> &visitor) { return visitor.visit_unary_expression(*this); }

VarExpression::VarExpression(tokenizer::Token name) : name{ std::move(name) } {}

std::string VarExpression::accept(ExpressionVisitor<std::string> &visitor)
{
    return visitor.visit_var_expression(*this);
}

types::Literal VarExpression::accept(ExpressionVisitor<types::Literal> &visitor)
{
    return visitor.visit_var_expression(*this);
}

void VarExpression::accept(ExpressionVisitor<void> &visitor) { return visitor.visit_var_expression(*this); }

AssignExpression::AssignExpression(tokenizer::Token name, Expression::ExpressionPtr value)
  : name{ std::move(name) }, value{ std::move(value) }
{}

std::string AssignExpression::accept(ExpressionVisitor<std::string> &visitor)
{
    return visitor.visit_assign_expression(*this);
}

types::Literal AssignExpression::accept(ExpressionVisitor<types::Literal> &visitor)
{
    return visitor.visit_assign_expression(*this);
}

void AssignExpression::accept(ExpressionVisitor<void> &visitor) { return visitor.visit_assign_expression(*this); }
}// namespace tek::parser
