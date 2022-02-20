#include "AstPrinter.hpp"

std::string AstPrinter::print(AstPrinter::ExpressionPtr expression) {
    return expression->accept(*this);
}

std::string AstPrinter::visit_binary_expression(BinaryExpression& expression) {
    return this->parenthesize(expression.op.lexeme,
                              {expression.left, expression.right});
}

std::string
AstPrinter::visit_grouping_expression(GroupingExpression& expression) {
    return this->parenthesize("group", {expression.expression});
}

std::string
AstPrinter::visit_literal_expression(LiteralExpression& expression) {
    if (std::holds_alternative<std::string>(expression.literal)) {
        return std::get<std::string>(expression.literal);
    } else if (std::holds_alternative<double>(expression.literal)) {
        return std::to_string(std::get<double>(expression.literal));
    } else {
        return "";
    }
}

std::string AstPrinter::visit_unary_expression(UnaryExpression& expression) {
    return this->parenthesize(expression.op.lexeme, {expression.right});
}

std::string
AstPrinter::parenthesize(const std::string& name,
                         const std::vector<Expression*>& expressions) {
    std::stringstream ss;

    for (const auto& expression : expressions) {
        ss << " " << expression->accept(*this);
    }

    return fmt::format("({}{})", name, ss.str());
}
