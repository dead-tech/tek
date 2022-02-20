#ifndef TOKEN_HPP
#define TOKEN_HPP
#include <any>
#include <cassert>
#include <fmt/format.h>
#include <string>

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN = 0,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    ENDOF,
    COUNT
};

std::string token_type_to_str(const TokenType token_type);

struct Token {
    TokenType type;
    std::string lexeme;
    std::any literal;
    std::size_t line;

    Token(const TokenType& type, const std::string& lexeme,
          const std::any& literal, const size_t line)
        : type{type}, lexeme{lexeme}, literal{literal}, line{line} {};

    [[nodiscard]] std::string to_string() const noexcept;
};

#endif
