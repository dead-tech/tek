#include "Expressions.hpp"

#include <utility>
namespace tek::parser {
    BinaryExpression::BinaryExpression(Expression::ExpressionPtr left,
                                       tokenizer::Token op,
                                       Expression::ExpressionPtr right)
        : left{std::move(left)}, op{std::move(op)}, right{std::move(right)} {}

    std::string BinaryExpression::accept(Visitor<std::string>& visitor) {
        return visitor.visit_binary_expression(*this);
    }

    GroupingExpression::GroupingExpression(Expression::ExpressionPtr expression)
        : expression{std::move(expression)} {}

    std::string GroupingExpression::accept(Visitor<std::string>& visitor) {
        return visitor.visit_grouping_expression(*this);
    }

    LiteralExpression::LiteralExpression(types::Literal::variant_t literal)
        : literal{std::move(literal)} {}

    std::string LiteralExpression::accept(Visitor<std::string>& visitor) {
        return visitor.visit_literal_expression(*this);
    }

    UnaryExpression::UnaryExpression(tokenizer::Token op, ExpressionPtr right)
        : op{std::move(op)}, right{std::move(right)} {}

    std::string UnaryExpression::accept(Visitor<std::string>& visitor) {
        return visitor.visit_unary_expression(*this);
    }
} // namespace tek::parser