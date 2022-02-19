#include "Token.hpp"

std::string token_type_to_str(TokenType token_type) {
    static_assert(static_cast<int>(TokenType::COUNT) == 39 &&
                  "Exhaustive handling for each token is required\n");
    switch (token_type) {
    case TokenType::LEFT_PAREN:
        return "(";
        break;
    case TokenType::RIGHT_PAREN:
        return ")";
        break;
    case TokenType::LEFT_BRACE:
        return "{";
        break;
    case TokenType::RIGHT_BRACE:
        return "}";
        break;
    case TokenType::COMMA:
        return ",";
        break;
    case TokenType::DOT:
        return ".";
        break;
    case TokenType::MINUS:
        return "-";
        break;
    case TokenType::PLUS:
        return "+";
        break;
    case TokenType::SEMICOLON:
        return ";";
        break;
    case TokenType::SLASH:
        return "/";
        break;
    case TokenType::STAR:
        return "*";
        break;
    case TokenType::BANG:
        return "=";
        break;
    case TokenType::BANG_EQUAL:
        return "!=";
        break;
    case TokenType::EQUAL:
        return "=";
        break;
    case TokenType::EQUAL_EQUAL:
        return "==";
        break;
    case TokenType::GREATER:
        return ">";
        break;
    case TokenType::GREATER_EQUAL:
        return ">=";
        break;
    case TokenType::IDENTIFIER:
        return "identifier";
        break;
    case TokenType::STRING:
        return "string";
        break;
    case TokenType::NUMBER:
        return "number";
        break;
    case TokenType::AND:
        return "and";
        break;
    case TokenType::CLASS:
        return "class";
        break;
    case TokenType::ELSE:
        return "else";
        break;
    case TokenType::FALSE:
        return "false";
        break;
    case TokenType::FUN:
        return "fun";
        break;
    case TokenType::FOR:
        return "for";
        break;
    case TokenType::IF:
        return "if";
        break;
    case TokenType::NIL:
        return "nil";
        break;
    case TokenType::OR:
        return "or";
        break;
    case TokenType::PRINT:
        return "print";
        break;
    case TokenType::RETURN:
        return "return";
        break;
    case TokenType::SUPER:
        return "super";
        break;
    case TokenType::THIS:
        return "this";
        break;
    case TokenType::TRUE:
        return "true";
        break;
    case TokenType::VAR:
        return "var";
        break;
    case TokenType::WHILE:
        return "while";
        break;
    case TokenType::ENDOF:
        return "EOF";
        break;
    default:
        assert(0 && "Unreachable");
        break;
    }
}

std::string Token::to_string() const noexcept {
    if (this->literal.type() == typeid(int)) {
        return fmt::format(
            "[\n\tLexeme: {},\n\tType: {},\n\tLiteral: {},\n\tLine: {},\n]",
            this->lexeme, token_type_to_str(this->type),
            std::any_cast<int>(this->literal), this->line);
    } else if (this->literal.type() == typeid(std::string)) {
        return fmt::format(
            "[\n\tLexeme: {},\n\tType: {},\n\tLiteral: {},\n\tLine: {},\n]",
            this->lexeme, token_type_to_str(this->type),
            std::any_cast<std::string>(this->literal), this->line);
    } else {
        return fmt::format("[\n\tLexeme: {},\n\tType: {},\n\tLine: {},\n]",
                           this->lexeme, token_type_to_str(this->type),
                           this->line);
    }
}
