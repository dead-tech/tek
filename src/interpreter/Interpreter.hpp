#ifndef TEK_INTERPRETER_HPP
#define TEK_INTERPRETER_HPP

#include "../exceptions/Exceptions.hpp"
#include "../logger/Logger.hpp"
#include "../parser/Expressions.hpp"
#include "../parser/Statements.hpp"
#include "../types/Literal.hpp"
#include "../utils/guard.hpp"
#include "../utils/traits.hpp"
#include "../utils/variants.hpp"
#include "Environment.hpp"

namespace tek::interpreter {
class Interpreter
  : public parser::ExpressionVisitor<types::Literal>
  , public parser::StatementVisitor<void>
{
  private:
    using ExpressionPtr  = std::unique_ptr<parser::Expression>;
    using StatementPtr   = std::unique_ptr<parser::Statement>;
    using EnvironmentPtr = std::shared_ptr<Environment>;
    using StatementsVec  = std::vector<StatementPtr>;

  public:
    void interpret(const StatementsVec &statements);

    [[nodiscard]] types::Literal visit_literal_expression(parser::LiteralExpression &expression) override;
    [[nodiscard]] types::Literal visit_grouping_expression(parser::GroupingExpression &expression) override;
    [[nodiscard]] types::Literal visit_unary_expression(parser::UnaryExpression &expression) override;
    [[nodiscard]] types::Literal visit_binary_expression(parser::BinaryExpression &expression) override;
    [[nodiscard]] types::Literal visit_var_expression(parser::VarExpression &expression) override;
    [[nodiscard]] types::Literal visit_assign_expression(parser::AssignExpression &expression) override;

    void visit_print_statement(parser::PrintStatement &statement) override;
    void visit_expression_statement(parser::ExpressionStatement &statement) override;
    void visit_var_statement(parser::VarStatement &statement) override;
    void visit_block_statement(parser::BlockStatement &statement) override;

    // Statement impl
  private:
    [[nodiscard]] static types::Literal interpret_unary_minus(const parser::UnaryExpression &expression,
      const types::Literal::variant_t                                                       &right);

    [[nodiscard]] static types::Literal interpret_binary_minus(const parser::BinaryExpression &expression,
      const types::Literal::variant_t                                                         &left,
      const types::Literal::variant_t                                                         &right);

    [[nodiscard]] static types::Literal interpret_binary_plus(parser::BinaryExpression &expression,
      const types::Literal::variant_t                                                  &left,
      const types::Literal::variant_t                                                  &right);

    [[nodiscard]] static types::Literal interpret_binary_slash(const parser::BinaryExpression &expression,
      const types::Literal::variant_t                                                         &left,
      const types::Literal::variant_t                                                         &right);

    [[nodiscard]] static types::Literal interpret_binary_star(const parser::BinaryExpression &expression,
      const types::Literal::variant_t                                                        &left,
      const types::Literal::variant_t                                                        &right);

    [[nodiscard]] static types::Literal interpret_binary_greater(const parser::BinaryExpression &expression,
      const types::Literal::variant_t                                                           &left,
      const types::Literal::variant_t                                                           &right);

    [[nodiscard]] static types::Literal interpret_binary_greater_equal(const parser::BinaryExpression &expression,
      const types::Literal::variant_t                                                                 &left,
      const types::Literal::variant_t                                                                 &right);

    [[nodiscard]] static types::Literal interpret_binary_less(const parser::BinaryExpression &expression,
      const types::Literal::variant_t                                                        &left,
      const types::Literal::variant_t                                                        &right);

    [[nodiscard]] static types::Literal interpret_binary_less_equal(const parser::BinaryExpression &expression,
      const types::Literal::variant_t                                                              &left,
      const types::Literal::variant_t                                                              &right);

    // Helpers
  private:
    types::Literal evaluate(const ExpressionPtr &expression);

    void execute(const StatementPtr &statement);
    void execute_block(const StatementsVec &statements, const EnvironmentPtr &environment);

    [[nodiscard]] constexpr static bool is_truthy(const types::Literal::variant_t &value);
    [[nodiscard]] constexpr static bool is_equal(const types::Literal::variant_t &left,
      const types::Literal::variant_t                                            &right);

    template<typename AssertType, typename... Variants>
    constexpr static void assert_operand_types(const tokenizer::Token &op, Variants &&...variants);

    [[nodiscard]] static std::string stringify(const types::Literal &value);

  private:
    EnvironmentPtr environment = std::make_shared<Environment>();
};
}// namespace tek::interpreter

#endif// TEK_INTERPRETER_HPP
