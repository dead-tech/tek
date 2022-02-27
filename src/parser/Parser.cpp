#include "Parser.hpp"

namespace tek::parser {

Parser::Parser(Parser::TokensVec tokens) : tokens{ std::move(tokens) }, current{ 0 } {}

Parser::ExpressionPtr Parser::expression() { return this->equality(); }

Parser::ExpressionPtr Parser::equality()
{
    auto left = this->comparison();

    while (match({ tokenizer::TokenType::BANG_EQUAL, tokenizer::TokenType::EQUAL_EQUAL })) {
        const auto op    = this->previous();
        auto       right = this->comparison();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }

    return left;
}

Parser::ExpressionPtr Parser::comparison()
{
    auto left = this->term();

    while (this->match({ tokenizer::TokenType::GREATER,
      tokenizer::TokenType::GREATER_EQUAL,
      tokenizer::TokenType::LESS,
      tokenizer::TokenType::LESS_EQUAL })) {
        const auto op    = this->previous();
        auto       right = this->term();
        left             = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

Parser::ExpressionPtr Parser::term()
{
    auto left = this->factor();

    while (this->match({ tokenizer::TokenType::MINUS, tokenizer::TokenType::PLUS })) {
        const auto op    = this->previous();
        auto       right = this->factor();
        left             = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

Parser::ExpressionPtr Parser::factor()
{
    auto left = this->unary();

    while (this->match({ tokenizer::TokenType::SLASH, tokenizer::TokenType::STAR })) {
        const auto op    = this->previous();
        auto       right = this->unary();
        left             = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

Parser::ExpressionPtr Parser::unary()
{
    if (this->match({ tokenizer::TokenType::BANG, tokenizer::TokenType::MINUS })) {
        const auto op    = this->previous();
        auto       right = this->unary();
        return std::make_unique<UnaryExpression>(op, std::move(right));
    }

    return this->primary();
}

Parser::ExpressionPtr Parser::primary()
{
    if (this->match({ tokenizer::TokenType::FALSE })) {
        return std::make_unique<LiteralExpression>(false);
    } else if (this->match({ tokenizer::TokenType::TRUE })) {
        return std::make_unique<LiteralExpression>(true);
    } else if (this->match({ tokenizer::TokenType::TRUE })) {
        return std::make_unique<LiteralExpression>(nullptr);
    } else if (this->match({ tokenizer::TokenType::NUMBER, tokenizer::TokenType::STRING })) {
        return std::make_unique<LiteralExpression>(this->previous().literal.value());
    } else if (this->match({ tokenizer::TokenType::LEFT_PAREN })) {
        auto expression = this->expression();
        this->consume(tokenizer::TokenType::RIGHT_PAREN, "Expect ')' after expression");
        return std::make_unique<GroupingExpression>(std::move(expression));
    }

    throw this->error(this->peek(), "Expected expression.");
}

bool Parser::match(const std::vector<tokenizer::TokenType> &types)
{
    for (const auto &type : types) {
        if (this->check(type)) {
            this->advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(const tokenizer::TokenType &type)
{
    if (this->is_at_end()) {
        return false;
    } else {
        return this->peek().type == type;
    }
}

bool Parser::is_at_end() { return this->peek().type == tokenizer::TokenType::ENDOF; }

tek::tokenizer::Token Parser::advance()
{
    if (!this->is_at_end()) { this->current++; }
    return this->previous();
}

tek::tokenizer::Token Parser::peek() { return this->tokens.at(this->current); }

tek::tokenizer::Token Parser::previous() { return this->tokens.at(this->current - 1); }

tek::tokenizer::Token Parser::consume(const tokenizer::TokenType &type, const std::string &message)
{
    if (this->check(type)) { return this->advance(); }

    throw this->error(this->peek(), message);
}

tek::exceptions::ParseError Parser::error(const tek::tokenizer::Token &token, const std::string &message)
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

std::optional<Parser::ExpressionPtr> Parser::parse()
{
    try {
        return this->expression();
    } catch (const exceptions::ParseError &error) {
        return std::nullopt;
    }
}
};// namespace tek::parser
