#include "Parser.hpp"

namespace tek::parser {

    Parser::Parser(Parser::TokensVec tokens) : tokens{ std::move(tokens) }, current{ 0 } {}

    Parser::ExpressionPtr Parser::expression() { return this->assignment(); }

    Parser::ExpressionPtr Parser::equality()
    {
        auto left = this->comparison();

        while (this->match(tokenizer::TokenType::BANG_EQUAL, tokenizer::TokenType::EQUAL_EQUAL)) {
            const auto op    = this->previous();
            auto       right = this->comparison();

            left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
        }

        return left;
    }

    Parser::ExpressionPtr Parser::assignment()
    {
        auto expression = this->logical_or();

        if (this->match(tokenizer::TokenType::EQUAL)) {
            const auto equals = this->previous();
            auto       value  = this->assignment();

            if (auto *var_expr = dynamic_cast<VarExpression *>(expression.get())) {
                return std::make_unique<AssignExpression>(var_expr->name, std::move(value));
            }

            Parser::error(equals, "Invalid assignment target.");
        }

        return expression;
    }

    Parser::ExpressionPtr Parser::comparison()
    {
        auto left = this->term();

        while (this->match(
          tokenizer::TokenType::GREATER,
          tokenizer::TokenType::GREATER_EQUAL,
          tokenizer::TokenType::LESS,
          tokenizer::TokenType::LESS_EQUAL)) {
            const auto op    = this->previous();
            auto       right = this->term();
            left             = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
        }
        return left;
    }

    Parser::ExpressionPtr Parser::term()
    {
        auto left = this->factor();

        while (this->match(tokenizer::TokenType::MINUS, tokenizer::TokenType::PLUS)) {
            const auto op    = this->previous();
            auto       right = this->factor();
            left             = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
        }
        return left;
    }

    Parser::ExpressionPtr Parser::factor()
    {
        auto left = this->unary();

        while (this->match(tokenizer::TokenType::SLASH, tokenizer::TokenType::STAR)) {
            const auto op    = this->previous();
            auto       right = this->unary();
            left             = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
        }
        return left;
    }

    Parser::ExpressionPtr Parser::unary()
    {
        if (this->match(tokenizer::TokenType::BANG, tokenizer::TokenType::MINUS)) {
            const auto op    = this->previous();
            auto       right = this->unary();
            return std::make_unique<UnaryExpression>(op, std::move(right));
        }

        return this->call();
    }

    Parser::ExpressionPtr Parser::primary()
    {
        if (this->match(tokenizer::TokenType::FALSE)) {
            return std::make_unique<LiteralExpression>(false);
        } else if (this->match(tokenizer::TokenType::TRUE)) {
            return std::make_unique<LiteralExpression>(true);
        } else if (this->match(tokenizer::TokenType::NIL)) {
            return std::make_unique<LiteralExpression>(nullptr);
        } else if (this->match(tokenizer::TokenType::NUMBER, tokenizer::TokenType::STRING)) {
            return std::make_unique<LiteralExpression>(this->previous().literal.value());
        } else if (this->match(tokenizer::TokenType::LEFT_PAREN)) {
            auto expression = this->expression();
            this->consume(tokenizer::TokenType::RIGHT_PAREN, "Expect ')' after expression");
            return std::make_unique<GroupingExpression>(std::move(expression));
        } else if (this->match(tokenizer::TokenType::IDENTIFIER)) {
            return std::make_unique<VarExpression>(this->previous());
        }

        throw Parser::error(this->peek(), "Expected expression.");
    }

    Parser::ExpressionPtr Parser::logical_or()
    {
        auto left = this->logical_and();

        while (this->match(tokenizer::TokenType::OR)) {
            const auto op    = this->previous();
            auto       right = this->logical_and();
            left             = std::make_unique<LogicalExpression>(std::move(left), op, std::move(right));
        }

        return left;
    }

    Parser::ExpressionPtr Parser::logical_and()
    {
        auto left = this->equality();

        while (this->match(tokenizer::TokenType::AND)) {
            const auto op    = this->previous();
            auto       right = this->equality();
            left             = std::make_unique<LogicalExpression>(std::move(left), op, std::move(right));
        }

        return left;
    }

    Parser::ExpressionPtr Parser::call()
    {
        auto expression = this->primary();

        while (true) {
            if (this->match(tokenizer::TokenType::LEFT_PAREN)) {
                expression = this->finish_call(expression);
            } else {
                break;
            }
        }

        return expression;
    }

    Parser::StatementPtr Parser::statement()
    {
        // check this if it doesn't work
        if (this->match(tokenizer::TokenType::PRINT)) {
            return this->print_statement();
        } else if (this->match(tokenizer::TokenType::LEFT_BRACE)) {
            return std::make_unique<BlockStatement>(this->block_statement());
        } else if (this->match(tokenizer::TokenType::IF)) {
            return this->if_statement();
        } else if (this->match(tokenizer::TokenType::WHILE)) {
            return this->while_statement();
        } else if (this->match(tokenizer::TokenType::FOR)) {
            return this->for_statement();
        } else if (this->match(tokenizer::TokenType::RETURN)) {
            return this->return_statement();
        }

        return this->expression_statement();
    }

    Parser::StatementPtr Parser::declaration()
    {
        try {
            if (this->match(tokenizer::TokenType::VAR)) {
                return this->var_statement();
            } else if (this->match(tokenizer::TokenType::FUN)) {
                return this->function_statement("function");
            }

            return this->statement();
        } catch (const exceptions::RuntimeError &error) {
            this->synchronize();
            return nullptr;
        }
    }

    Parser::StatementPtr Parser::print_statement()
    {
        auto value = this->expression();
        this->consume(tokenizer::TokenType::SEMICOLON, "Expected ';' after value.");

        return std::make_unique<PrintStatement>(std::move(value));
    }

    Parser::StatementPtr Parser::expression_statement()
    {
        auto value = this->expression();
        this->consume(tokenizer::TokenType::SEMICOLON, "Expected ';' after expression.");

        return std::make_unique<ExpressionStatement>(std::move(value));
    }

    Parser::StatementPtr Parser::var_statement()
    {
        const auto name = this->consume(tokenizer::TokenType::IDENTIFIER, "Expected variable name.");

        ExpressionPtr initializer = nullptr;

        if (this->match(tokenizer::TokenType::EQUAL)) { initializer = this->expression(); }

        this->consume(tokenizer::TokenType::SEMICOLON, "Expected semicolon after variable declaration.");

        return std::make_unique<VarStatement>(name, std::move(initializer));
    }

    Parser::StatementsVec Parser::block_statement()
    {
        StatementsVec out;

        while (!this->check(tokenizer::TokenType::RIGHT_BRACE) && !this->is_at_end()) {
            out.push_back(std::move(this->declaration()));
        }

        this->consume(tokenizer::TokenType::RIGHT_BRACE, "Expect '}' after block.");
        return out;
    }

    Parser::StatementPtr Parser::if_statement()
    {
        this->consume(tokenizer::TokenType::LEFT_PAREN, "Expected '(' after if keyword.");
        auto condition = this->expression();
        this->consume(tokenizer::TokenType::RIGHT_PAREN, "Expected ')' after condition in if statement.");

        auto         then_branch = this->statement();
        StatementPtr else_branch = nullptr;

        if (this->match(tokenizer::TokenType::ELSE)) { else_branch = this->statement(); }

        return std::make_unique<IfStatement>(std::move(condition), std::move(then_branch), std::move(else_branch));
    }

    Parser::StatementPtr Parser::while_statement()
    {
        this->consume(tokenizer::TokenType::LEFT_PAREN, "Expected '(' after while keyword.");
        auto condition = this->expression();
        this->consume(tokenizer::TokenType::RIGHT_PAREN, "Expected ')' after condition in while loop.");

        auto body = this->statement();
        return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
    }

    Parser::StatementPtr Parser::for_statement()
    {
        this->consume(tokenizer::TokenType::LEFT_PAREN, "Expected '(' after for keyword.");

        auto initializer = this->for_statement_initializer();

        auto condition = this->for_statement_condition();
        this->consume(tokenizer::TokenType::SEMICOLON, "Expected ';' after for loop condition.");

        auto increment = this->for_statement_increment();
        this->consume(tokenizer::TokenType::RIGHT_PAREN, "Expected ')' after for loop increment expression.");

        auto body = this->for_statement_body(increment);

        return std::make_unique<ForStatement>(std::move(initializer), std::move(condition), std::move(body));
    }

    Parser::StatementPtr Parser::function_statement(const std::string &kind)
    {
        const tokenizer::Token name =
          this->consume(tokenizer::TokenType::IDENTIFIER, fmt::format("Expected {} name.", kind));
        this->consume(tokenizer::TokenType::LEFT_PAREN, fmt::format("Expected '(' after {} keyword", kind));

        std::vector<tokenizer::Token> parameters;
        if (!this->check(tokenizer::TokenType::RIGHT_PAREN)) {
            do {
                if (parameters.size() >= 255) { Parser::error(this->peek(), "Can't have more than 255 parameters."); }

                parameters.emplace_back(this->consume(tokenizer::TokenType::IDENTIFIER, "Expected parameter name."));
            } while (this->match(tokenizer::TokenType::COMMA));
        }

        this->consume(tokenizer::TokenType::RIGHT_PAREN, "Expected ')' after parameter list in function definition.");

        this->consume(
          tokenizer::TokenType::LEFT_BRACE, fmt::format("Expected '{{' after parameter list in {} definition.", kind));

        std::vector<StatementPtr> body = this->block_statement();

        return std::make_unique<FunctionStatement>(name, parameters, std::move(body));
    }

    Parser::StatementPtr Parser::return_statement()
    {
        auto          keyword    = this->previous();
        ExpressionPtr expression = nullptr;
        if (!this->check(tokenizer::TokenType::COMMA)) { expression = this->expression(); }

        this->consume(tokenizer::TokenType::SEMICOLON, "Expected ';' after return statement.");
        return std::make_unique<ReturnStatement>(keyword, std::move(expression));
    }

    Parser::StatementPtr Parser::for_statement_initializer()
    {
        if (this->match(tokenizer::TokenType::VAR)) {
            return this->var_statement();
        } else if (this->match(tokenizer::TokenType::SEMICOLON)) {
            return nullptr;
        }

        // Not quite sure
        return this->expression_statement();
    }

    Parser::ExpressionPtr Parser::for_statement_condition()
    {
        if (!this->check(tokenizer::TokenType::SEMICOLON)) {
            return this->expression();
        } else {
            return std::make_unique<LiteralExpression>(types::Literal(true).value());
        }
    }

    Parser::ExpressionPtr Parser::for_statement_increment()
    {
        if (!this->check(tokenizer::TokenType::RIGHT_PAREN)) {
            return this->expression();
        } else {
            return nullptr;
        }
    }

    Parser::StatementPtr Parser::for_statement_body(ExpressionPtr &increment)
    {
        auto body = this->statement();

        // TODO: Find a better way to do this
        StatementsVec out;
        out.push_back(std::move(body));
        if (increment) { out.push_back(std::make_unique<ExpressionStatement>(std::move(increment))); }

        return std::make_unique<BlockStatement>(std::move(out));
    }

    Parser::ExpressionPtr Parser::finish_call(Parser::ExpressionPtr &callee)
    {
        std::vector<ExpressionPtr> arguments;
        if (!this->check(tokenizer::TokenType::RIGHT_PAREN)) {
            do {
                if (arguments.size() >= 255) {
                    Parser::error(this->peek(), "Function call can't accept more than 255 arguments.");
                }
                arguments.push_back(this->expression());
            } while (this->match(tokenizer::TokenType::COMMA));
        }

        const tokenizer::Token paren =
          this->consume(tokenizer::TokenType::RIGHT_PAREN, "Expected '(' after argument list in function call.");

        return std::make_unique<CallExpression>(std::move(callee), paren, std::move(arguments));
    }

    template<typename Match>
    constexpr bool Parser::match(Match &&match)
    {
        if (this->check(std::forward<Match>(match))) {
            this->advance();
            return true;
        }
        return false;
    }

    template<typename... Matches>
    constexpr bool Parser::match(Matches &&...matches)
    {
        return (match(std::forward<Matches>(matches)) || ...);
    }

    bool Parser::check(const tokenizer::TokenType &type) { return !this->is_at_end() && this->peek().type == type; }

    bool Parser::is_at_end() { return this->peek().type == tokenizer::TokenType::ENDOF; }

    tokenizer::Token Parser::advance()
    {
        if (!this->is_at_end()) { this->current++; }
        return this->previous();
    }

    tokenizer::Token Parser::peek() { return this->tokens.at(this->current); }

    tokenizer::Token Parser::previous() { return this->tokens.at(this->current - 1); }

    tokenizer::Token Parser::consume(const tokenizer::TokenType &type, const std::string &message)
    {
        if (this->check(type)) { return this->advance(); }

        throw Parser::error(this->peek(), message);
    }

    exceptions::ParseError Parser::error(const tokenizer::Token &token, const std::string &message)
    {
        logger::Logger::error(token, message);
        throw exceptions::ParseError();
    }

    void Parser::synchronize()
    {
        this->advance();

        while (!this->is_at_end()) {
            if (this->previous().type == tokenizer::TokenType::SEMICOLON) { return; }

            switch (this->peek().type) {
                case tokenizer::TokenType::CLASS:
                case tokenizer::TokenType::FUN:
                case tokenizer::TokenType::VAR:
                case tokenizer::TokenType::FOR:
                case tokenizer::TokenType::IF:
                case tokenizer::TokenType::WHILE:
                case tokenizer::TokenType::PRINT:
                case tokenizer::TokenType::RETURN:
                    return;
            }
            this->advance();
        }
    }

    std::optional<Parser::StatementsVec> Parser::parse()
    {
        StatementsVec out;
        try {
            while (!this->is_at_end()) { out.push_back(this->declaration()); }
        } catch (const exceptions::ParseError &error) {
            return std::nullopt;
        }

        return out;
    }
}// namespace tek::parser
