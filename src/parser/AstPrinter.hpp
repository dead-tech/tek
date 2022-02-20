#ifndef AstPrinter_HPP
#define AstPrinter_HPP

#include "Expressions.hpp"
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class AstPrinter : public Visitor<std::string> {
  private:
    using ExpressionPtr = std::unique_ptr<Expression>;

  public:
    std::string print(ExpressionPtr expression);

    [[nodiscard]] std::string
    visit_binary_expression(BinaryExpression& expression) override;
    [[nodiscard]] std::string
    visit_grouping_expression(GroupingExpression& expression) override;
    [[nodiscard]] std::string
    visit_literal_expression(LiteralExpression& expression) override;
    [[nodiscard]] std::string
    visit_unary_expression(UnaryExpression& expression) override;

  private:
    std::string parenthesize(const std::string& name,
                             const std::vector<Expression*>& expressions);
};

#endif
