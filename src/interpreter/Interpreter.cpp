#include "Interpreter.hpp"

namespace tek::interpreter {
void Interpreter::interpret(Interpreter::ExpressionPtr expression)
{
    try {
        const auto value = this->evaluate(expression);
        fmt::print("{}\n", tek::interpreter::Interpreter::stringify(value));
    } catch (const exceptions::RuntimeError &error) {
        logger::Logger::runtime_error(error);
    }
}

types::Literal Interpreter::visit_literal_expression(parser::LiteralExpression &expression)
{
    return expression.literal;
}

types::Literal Interpreter::visit_grouping_expression(parser::GroupingExpression &expression)
{
    return this->evaluate(expression.expression);
}

types::Literal Interpreter::evaluate(const Interpreter::ExpressionPtr &expression) { return expression->accept(*this); }

types::Literal Interpreter::visit_unary_expression(parser::UnaryExpression &expression)
{
    const auto right = this->evaluate(expression.right).value();

    switch (expression.op.type) {
    case tokenizer::TokenType::MINUS: {
        this->assert_operand_types<double>(expression.op, right);
        const auto value = std::get<double>(right);
        return types::Literal(-value);
    }
    case tokenizer::TokenType::BANG:
        return types::Literal(!tek::interpreter::Interpreter::is_truthy(right));
    }

    // unreachable
    return types::Literal("");
}

types::Literal Interpreter::visit_binary_expression(parser::BinaryExpression &expression)
{
    const auto right = this->evaluate(expression.right).value();
    const auto left  = this->evaluate(expression.left).value();

    // TODO: Abstract this out in some way.
    switch (expression.op.type) {
    case tokenizer::TokenType::MINUS: {
        this->assert_operand_types<double>(expression.op, left, right);
        const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
        return types::Literal(left_value - right_value);
    }
    case tokenizer::TokenType::PLUS: {
        if (variants::have_type_of<std::string>(left, right)) {
            const auto &[left_value, right_value] = variants::to_tuple<std::string>(left, right);
            return types::Literal(left_value + right_value);
        } else if (variants::have_type_of<double>(left, right)) {
            const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
            return types::Literal(left_value + right_value);
        } else {
            throw exceptions::RuntimeError(expression.op, "Operands must be both of type `string` or `number`");
        }
    }
    case tokenizer::TokenType::SLASH: {
        this->assert_operand_types<double>(expression.op, left, right);
        const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
        return types::Literal(left_value / right_value);
    }
    case tokenizer::TokenType::STAR: {
        this->assert_operand_types<double>(expression.op, left, right);
        const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
        return types::Literal(left_value * right_value);
    }
    case tokenizer::TokenType::GREATER: {
        this->assert_operand_types<double>(expression.op, left, right);
        const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
        return types::Literal(left_value > right_value);
    }
    case tokenizer::TokenType::GREATER_EQUAL: {
        this->assert_operand_types<double>(expression.op, left, right);
        const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
        return types::Literal(left_value >= right_value);
    }
    case tokenizer::TokenType::LESS: {
        this->assert_operand_types<double>(expression.op, left, right);
        const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
        return types::Literal(left_value < right_value);
    }
    case tokenizer::TokenType::LESS_EQUAL: {
        this->assert_operand_types<double>(expression.op, left, right);
        const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
        return types::Literal(left_value <= right_value);
    }
    case tokenizer::TokenType::BANG_EQUAL: {
        return types::Literal(!tek::interpreter::Interpreter::is_equal(left, right));
    }
    case tokenizer::TokenType::EQUAL_EQUAL: {
        return types::Literal(tek::interpreter::Interpreter::is_equal(left, right));
    }
    }

    // unreachable
    return types::Literal("");
}

bool Interpreter::is_truthy(const types::Literal::variant_t &value)
{
    if (std::holds_alternative<std::nullptr_t>(value)) {
        return false;
    } else if (const auto boolean = std::get<bool>(value)) {
        return boolean;
    } else {
        return true;
    }
}

bool Interpreter::is_equal(const types::Literal::variant_t &left, const types::Literal::variant_t &right)
{
    if (variants::have_type_of<std::nullptr_t>(left, right)) {
        return true;
    } else if (std::holds_alternative<std::nullptr_t>(left)) {
        return false;
    }

    // This might be a problem, or maybe not
    return left == right;
}

template<typename AssertType, typename... Variants>
constexpr void Interpreter::assert_operand_types(const tokenizer::Token &op, Variants &&...variants)
{
    if ((variants::have_type_of<AssertType>(variants) && ...)) { return; }

    throw exceptions::RuntimeError(op, "Operand must be of type" + traits::TypeName<AssertType>::get());
}

std::string Interpreter::stringify(const types::Literal &value) { return value.str(); }

}// namespace tek::interpreter
