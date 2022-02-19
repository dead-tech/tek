#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <charconv>
#include <string>
#include <unordered_map>
#include <vector>

#include "../logger/Logger.hpp"
#include "Token.hpp"

class Tokenizer {
  public:
    explicit Tokenizer(const std::string& source_code);

    std::vector<Token> tokenize();

  private:
    [[nodiscard]] bool is_at_end() const;

    char advance();

    [[nodiscard]] char peek();

    [[nodiscard]] char peek_next();

    [[nodiscard]] bool match_next(const char expected);

    void add_token(const TokenType type);

    void add_token(const TokenType type, const std::any& literal);

    void string_literal();

    void number_literal();

    void identifier();

    void scan_token();

  private:
    std::size_t start = 0;
    std::size_t current = 0;
    std::size_t line = 1;

    std::string source;
    std::vector<Token> tokens;

    const inline static std::unordered_map<std::string, TokenType> keywords = {
        {"and", TokenType::AND},       {"class", TokenType::CLASS},
        {"else", TokenType::ELSE},     {"false", TokenType::FALSE},
        {"for", TokenType::FOR},       {"fun", TokenType::FUN},
        {"if", TokenType::IF},         {"nil", TokenType::NIL},
        {"or", TokenType::OR},         {"print", TokenType::PRINT},
        {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
        {"this", TokenType::THIS},     {"true", TokenType::TRUE},
        {"var", TokenType::VAR},       {"while", TokenType::WHILE},
    };
};

#endif
