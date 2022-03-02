#include "Interpreter.hpp"

namespace tek::interpreter {

    types::Literal clock(Interpreter &interpreter, const std::vector<types::Literal> &arguments)
    {
        auto current_time        = std::chrono::system_clock::now();
        auto duration_in_seconds = std::chrono::duration<double>(current_time.time_since_epoch());

        return types::Literal(duration_in_seconds.count());
    }

    Interpreter::Interpreter()
    {
        // TODO: Take this into the standard library
        this->globals->define("clock", types::Literal(types::NativeCallable("clock", &clock, 0)));
    }

    void Interpreter::interpret(const Interpreter::StatementsVec &statements)
    {
        try {
            for (const auto &statement : statements) { this->execute(statement); }
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

    types::Literal Interpreter::evaluate(const ExpressionPtr &expression) { return expression->accept(*this); }

    void Interpreter::execute(const StatementPtr &statement) { statement->accept(*this); }

    void Interpreter::execute_block(const StatementsVec &statements, const EnvironmentPtr &environment)
    {
        const auto previous = this->environment;

        try {
            utils::ScopeGuard guard([&]() { this->environment = previous; });
            this->environment = environment;
            for (const auto &statement : statements) { this->execute(statement); }
        } catch (const exceptions::RuntimeError &error) {
        }
    }

    types::Literal Interpreter::visit_unary_expression(parser::UnaryExpression &expression)
    {
        const auto right = this->evaluate(expression.right).value();

        switch (expression.op.type) {
            case tokenizer::TokenType::MINUS: {
                return Interpreter::interpret_unary_minus(expression, right);
            }
            case tokenizer::TokenType::BANG:
                return types::Literal(!tek::interpreter::Interpreter::is_truthy(right));
        }

        // unreachable
        return types::Literal("");
    }


    types::Literal Interpreter::visit_binary_expression(parser::BinaryExpression &expression)
    {
        const auto left  = this->evaluate(expression.left).value();
        const auto right = this->evaluate(expression.right).value();

        switch (expression.op.type) {
            case tokenizer::TokenType::MINUS: {
                return Interpreter::interpret_binary_minus(expression, left, right);
            }
            case tokenizer::TokenType::PLUS: {
                return Interpreter::interpret_binary_plus(expression, left, right);
            }
            case tokenizer::TokenType::SLASH: {
                return Interpreter::interpret_binary_slash(expression, left, right);
            }
            case tokenizer::TokenType::STAR: {
                return Interpreter::interpret_binary_star(expression, left, right);
            }
            case tokenizer::TokenType::GREATER: {
                return Interpreter::interpret_binary_greater(expression, left, right);
            }
            case tokenizer::TokenType::GREATER_EQUAL: {
                return Interpreter::interpret_binary_greater_equal(expression, left, right);
            }
            case tokenizer::TokenType::LESS: {
                return Interpreter::interpret_binary_less(expression, left, right);
            }
            case tokenizer::TokenType::LESS_EQUAL: {
                return Interpreter::interpret_binary_less_equal(expression, left, right);
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

    types::Literal Interpreter::visit_var_expression(parser::VarExpression &expression)
    {
        try {
            return this->environment->get(expression.name);
        } catch (const exceptions::RuntimeError &error) {
            logger::Logger::runtime_error(error);
        }

        // unreachable
        return types::Literal("");
    }

    types::Literal Interpreter::visit_assign_expression(parser::AssignExpression &expression)
    {
        auto value = this->evaluate(expression.value);
        this->environment->assign(expression.name, value);
        return value;
    }

    types::Literal Interpreter::visit_logical_expression(parser::LogicalExpression &expression)
    {
        const auto left = this->evaluate(expression.left).value();

        if (expression.op.type == tokenizer::TokenType::OR) {
            if (Interpreter::is_truthy(left)) { return types::Literal(left); }
        } else {
            if (!Interpreter::is_truthy(left)) { return types::Literal(left); }
        }

        return this->evaluate(expression.right);
    }

    types::Literal Interpreter::visit_call_expression(parser::CallExpression &expression)
    {
        auto callee = this->evaluate(expression.callee);

        std::vector<types::Literal> evaluated_argumensts;

        for (const auto &arg : expression.arguments) { evaluated_argumensts.emplace_back(this->evaluate(arg).value()); }

        if (const auto function = callee.as_callable()) {

            // Check for the number of arguments
            const auto actual_argument_num   = evaluated_argumensts.size();
            const auto expected_argument_num = function->get_arity();
            if (actual_argument_num != expected_argument_num) {
                throw exceptions::RuntimeError(
                  expression.paren,
                  fmt::format("Expected {} arguments but got {}.", expected_argument_num, actual_argument_num));
            }

            return function->call(*this, evaluated_argumensts);
        }

        throw exceptions::RuntimeError(expression.paren, "Call operator lhs is not a callable.");
    }

    void Interpreter::visit_print_statement(parser::PrintStatement &statement)
    {
        const types::Literal value = this->evaluate(statement.expression);
        fmt::print("{}\n", tek::interpreter::Interpreter::stringify(value));
    }

    void Interpreter::visit_expression_statement(parser::ExpressionStatement &statement)
    {
        this->evaluate(statement.expression);
    }

    void Interpreter::visit_var_statement(parser::VarStatement &statement)
    {
        types::Literal value(nullptr);
        if (statement.initializer != nullptr) { value = this->evaluate(statement.initializer); }
        this->environment->define(statement.name.lexeme, value);
    }

    void Interpreter::visit_block_statement(parser::BlockStatement &statement)
    {
        this->execute_block(statement.statements, std::make_shared<Environment>(this->environment));
    }

    void Interpreter::visit_if_statement(parser::IfStatement &statement)
    {
        if (Interpreter::is_truthy(this->evaluate(statement.condition).value())) {
            this->execute(statement.then_branch);
        } else if (statement.else_branch) {
            this->execute(statement.else_branch);
        }
    }

    void Interpreter::visit_while_statement(parser::WhileStatement &statement)
    {
        while (Interpreter::is_truthy(this->evaluate(statement.condition).value())) { this->execute(statement.body); }
    }


    void Interpreter::visit_for_statement(parser::ForStatement &statement)
    {
        const auto previous        = this->environment;
        const auto new_environment = std::make_shared<Environment>(this->environment);

        utils::ScopeGuard guard([&]() { this->environment = previous; });
        this->environment = new_environment;

        if (statement.initializer) { this->execute(statement.initializer); }
        while (Interpreter::is_truthy(this->evaluate(statement.condition).value())) { this->execute(statement.body); }
    }

    void Interpreter::visit_function_statement(parser::FunctionStatement &statement)
    {
        const auto function_name = statement.name.lexeme;
        auto       function      = std::make_shared<parser::FunctionStatement>(std::move(statement));
        this->environment->define(function_name, types::Literal(types::TekFunction(function)));
    }

    types::Literal Interpreter::interpret_unary_minus(
      const parser::UnaryExpression   &expression,
      const types::Literal::variant_t &right)
    {
        Interpreter::assert_operand_types<double>(expression.op, right);
        const auto value = std::get<double>(right);
        return types::Literal(-value);
    }

    types::Literal Interpreter::interpret_binary_minus(
      const parser::BinaryExpression  &expression,
      const types::Literal::variant_t &left,
      const types::Literal::variant_t &right)
    {
        Interpreter::assert_operand_types<double>(expression.op, left, right);
        const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
        return types::Literal(left_value - right_value);
    }

    types::Literal Interpreter::interpret_binary_plus(
      parser::BinaryExpression        &expression,
      const types::Literal::variant_t &left,
      const types::Literal::variant_t &right)
    {
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

    types::Literal Interpreter::interpret_binary_slash(
      const parser::BinaryExpression  &expression,
      const types::Literal::variant_t &left,
      const types::Literal::variant_t &right)
    {
        Interpreter::assert_operand_types<double>(expression.op, left, right);
        const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
        return types::Literal(left_value / right_value);
    }

    types::Literal Interpreter::interpret_binary_star(
      const parser::BinaryExpression  &expression,
      const types::Literal::variant_t &left,
      const types::Literal::variant_t &right)
    {
        Interpreter::assert_operand_types<double>(expression.op, left, right);
        const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
        return types::Literal(left_value * right_value);
    }

    types::Literal Interpreter::interpret_binary_greater(
      const parser::BinaryExpression  &expression,
      const types::Literal::variant_t &left,
      const types::Literal::variant_t &right)
    {
        Interpreter::assert_operand_types<double>(expression.op, left, right);
        const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
        return types::Literal(left_value > right_value);
    }

    types::Literal Interpreter::interpret_binary_greater_equal(
      const parser::BinaryExpression  &expression,
      const types::Literal::variant_t &left,
      const types::Literal::variant_t &right)
    {
        Interpreter::assert_operand_types<double>(expression.op, left, right);
        const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
        return types::Literal(left_value >= right_value);
    }

    types::Literal Interpreter::interpret_binary_less(
      const parser::BinaryExpression  &expression,
      const types::Literal::variant_t &left,
      const types::Literal::variant_t &right)
    {
        Interpreter::assert_operand_types<double>(expression.op, left, right);
        const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
        return types::Literal(left_value < right_value);
    }

    types::Literal Interpreter::interpret_binary_less_equal(
      const parser::BinaryExpression  &expression,
      const types::Literal::variant_t &left,
      const types::Literal::variant_t &right)
    {
        Interpreter::assert_operand_types<double>(expression.op, left, right);
        const auto &[left_value, right_value] = variants::to_tuple<double>(left, right);
        return types::Literal(left_value <= right_value);
    }

    constexpr bool Interpreter::is_truthy(const types::Literal::variant_t &value)
    {
        if (std::holds_alternative<std::nullptr_t>(value)) {
            return false;
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        } else {
            return true;
        }
    }

    constexpr bool Interpreter::is_equal(const types::Literal::variant_t &left, const types::Literal::variant_t &right)
    {
        if (variants::have_type_of<std::nullptr_t>(left, right)) {
            return true;
        } else if (std::holds_alternative<std::nullptr_t>(left)) {
            return false;
        }

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
