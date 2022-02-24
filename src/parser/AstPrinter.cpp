#include "AstPrinter.hpp"

namespace tek::parser {
    std::string AstPrinter::print(AstPrinter::ExpressionPtr expression) {
        return expression->accept(*this);
    }

    std::string
    AstPrinter::visit_binary_expression(BinaryExpression& expression) {
        return this->parenthesize(
            expression.op.lexeme,
            {expression.left.get(), expression.right.get()});
    }

    std::string
    AstPrinter::visit_grouping_expression(GroupingExpression& expression) {
        return this->parenthesize("group", {expression.expression.get()});
    }

    std::string
    AstPrinter::visit_literal_expression(LiteralExpression& expression) {
        return expression.literal.str();
    }

    std::string
    AstPrinter::visit_unary_expression(UnaryExpression& expression) {
        return this->parenthesize(expression.op.lexeme,
                                  {expression.right.get()});
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
} // namespace tek::parser
