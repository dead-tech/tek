#ifndef TEK_PARSER_HPP
#define TEK_PARSER_HPP

#include "../excepetions/Exceptions.hpp"
#include "../logger/Logger.hpp"
#include "../tokenizer/Token.hpp"
#include "Expressions.hpp"
#include <utility>
#include <vector>

namespace tek::parser {
class Parser
{
  private:
    using TokensVec     = std::vector<tokenizer::Token>;
    using ExpressionPtr = std::unique_ptr<Expression>;

  public:
    explicit Parser(TokensVec tokens);
    [[nodiscard]] ExpressionPtr parse();

  private:
    [[nodiscard]] ExpressionPtr expression();
    [[nodiscard]] ExpressionPtr equality();
    [[nodiscard]] ExpressionPtr comparison();
    [[nodiscard]] ExpressionPtr term();
    [[nodiscard]] ExpressionPtr factor();
    [[nodiscard]] ExpressionPtr unary();
    [[nodiscard]] ExpressionPtr primary();

    bool match(const std::vector<tokenizer::TokenType> &types);
    bool check(const tokenizer::TokenType &type);
    bool is_at_end();

    tokenizer::Token advance();
    tokenizer::Token peek();
    tokenizer::Token previous();
    tokenizer::Token consume(const tokenizer::TokenType &type, const std::string &message);

    void synchronize();

    exceptions::ParseError error(const tokenizer::Token &token, const std::string &message);

  private:
    TokensVec   tokens;
    std::size_t current;
};
}// namespace tek::parser

#endif// TEK_PARSER_HPP
