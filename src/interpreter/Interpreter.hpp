#ifndef TEK_INTERPRETER_HPP
#define TEK_INTERPRETER_HPP

#include "../excepetions/Exceptions.hpp"
#include "../logger/Logger.hpp"
#include "../parser/Expressions.hpp"
#include "../types/Literal.hpp"
#include "../utils/traits.hpp"

namespace tek::interpreter {
    class Interpreter : public parser::Visitor<types::Literal> {
      private:
        using ExpressionPtr = std::unique_ptr<parser::Expression>;

      public:
        void interpret(ExpressionPtr expression);

        [[nodiscard]] types::Literal visit_literal_expression(
            parser::LiteralExpression& expression) override;
        [[nodiscard]] types::Literal visit_grouping_expression(
            parser::GroupingExpression& expression) override;
        [[nodiscard]] types::Literal
        visit_unary_expression(parser::UnaryExpression& expression) override;
        [[nodiscard]] types::Literal
        visit_binary_expression(parser::BinaryExpression& expression) override;

      private:
        [[nodiscard]] types::Literal evaluate(const ExpressionPtr& expression);

        [[nodiscard]] bool is_truthy(const types::Literal::variant_t& value);
        [[nodiscard]] bool is_equal(const types::Literal::variant_t& left,
                                    const types::Literal::variant_t& right);
        template <typename Type>
        constexpr void
        assert_operand_type(const tokenizer::Token& op,
                            const types::Literal::variant_t& operand);

        template <typename Type>
        constexpr void
        assert_operand_types(const tokenizer::Token& op,
                             const types::Literal::variant_t& left,
                             const types::Literal::variant_t& right);

        template <typename Type>
        [[nodiscard]] constexpr bool
        have_type_of(const types::Literal::variant_t& left,
                     const types::Literal::variant_t& right);
        template <typename Type>
        [[nodiscard]] std::tuple<Type, Type>
        get_values(const types::Literal::variant_t& left,
                   const types::Literal::variant_t& right);

        [[nodiscard]] std::string
        stringify(const types::Literal::variant_t& value);
    };
} // namespace tek::interpreter

#endif // TEK_INTERPRETER_HPP
