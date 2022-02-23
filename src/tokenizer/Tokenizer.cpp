#include "Tokenizer.hpp"

#include <utility>

namespace tek::tokenizer {
    Tokenizer::Tokenizer(std::string source_code)
        : source{std::move(source_code)} {}

    std::vector<Token> Tokenizer::tokenize() {
        while (!this->is_at_end()) {
            this->start = this->current;
            this->scan_token();
        }

        this->tokens.emplace_back(TokenType::ENDOF, "", "", this->line);
        return this->tokens;
    }

    bool Tokenizer::is_at_end() const {
        return this->current >= this->source.length();
    }

    char Tokenizer::advance() { return this->source[this->current++]; }

    char Tokenizer::peek() {
        if (this->is_at_end()) {
            return '\0';
        }
        return this->source[this->current];
    }

    char Tokenizer::peek_next() {
        if (this->current + 1 >= this->source.length()) {
            return '\0';
        }
        return this->source[this->current + 1];
    }

    bool Tokenizer::match_next(const char expected) {
        if (this->is_at_end()) {
            return false;
        }

        if (this->source[current] != expected) {
            return false;
        }

        this->current++;
        return true;
    }

    void Tokenizer::add_token(const TokenType type) {
        this->add_token(type, "");
    }

    void Tokenizer::add_token(const TokenType type, const std::any& literal) {
        const auto lexeme =
            this->source.substr(this->start, this->current - this->start);
        this->tokens.emplace_back(type, lexeme, literal, this->line);
    }

    void Tokenizer::string_literal() {
        while (this->peek() != '"' && !this->is_at_end()) {
            if (this->peek() == '\n') {
                this->line++;
            }
            this->advance();
        }

        this->advance();

        const auto string_literal = this->source.substr(
            this->start + 1, this->current - this->start - 2);
        this->add_token(TokenType::STRING, string_literal);
    }

    void Tokenizer::number_literal() {
        while (std::isdigit(this->peek())) {
            this->advance();
        }

        if (this->peek() == '.' && std::isdigit(this->peek_next())) {
            do {
                this->advance();
            } while (std::isdigit(this->peek()));
        }

        const auto str_number =
            this->source.substr(this->start, this->current - this->start);
        int number{};
        std::from_chars(str_number.data(),
                        str_number.data() + str_number.size(), number);
        this->add_token(TokenType::NUMBER, number);
    }

    void Tokenizer::identifier() {
        while (std::isalnum(this->peek())) {
            this->advance();
        }

        const auto text =
            this->source.substr(this->start, this->current - this->start);
        const auto it = Tokenizer::keywords.find(text);
        auto token_type = TokenType::IDENTIFIER;
        if (it != Tokenizer::keywords.end()) {
            token_type = it->second;
        }
        this->add_token(token_type);
    }

    void Tokenizer::scan_token() {
        const char c = this->advance();
        switch (c) {
        case '(': {
            this->add_token(TokenType::LEFT_PAREN);
            break;
        }
        case ')':
            this->add_token(TokenType::RIGHT_PAREN);
            break;
        case '{':
            this->add_token(TokenType::LEFT_BRACE);
            break;
        case '}':
            this->add_token(TokenType::RIGHT_BRACE);
            break;
        case ',':
            this->add_token(TokenType::COMMA);
            break;
        case '.':
            this->add_token(TokenType::DOT);
            break;
        case '-':
            this->add_token(TokenType::MINUS);
            break;
        case '+':
            this->add_token(TokenType::PLUS);
            break;
        case ';':
            this->add_token(TokenType::SEMICOLON);
            break;
        case '*':
            this->add_token(TokenType::STAR);
            break;
        case '!':
            this->add_token(match_next('=') ? TokenType::BANG_EQUAL
                                            : TokenType::BANG);
            break;
        case '=':
            this->add_token(match_next('=') ? TokenType::EQUAL_EQUAL
                                            : TokenType::EQUAL);
            break;
        case '<':
            this->add_token(match_next('=') ? TokenType::LESS_EQUAL
                                            : TokenType::LESS);
            break;
        case '>':
            this->add_token(match_next('=') ? TokenType::GREATER_EQUAL
                                            : TokenType::GREATER);
            break;
        case '/':
            if (this->match_next('/')) {
                while (this->peek() == '\n' && !this->is_at_end()) {
                    this->advance();
                }
            } else {
                this->add_token(TokenType::SLASH);
            }
            break;
        // Ignore whitespace character
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            this->line++;
            break;
        case '"':
            this->string_literal();
            break;
        default:
            if (std::isdigit(c)) {
                this->number_literal();
            } else if (std::isalpha(c)) {
                this->identifier();
            } else {
                logger::Logger::error(
                    this->line, fmt::format("Unexpected character: {}", c));
            }
            break;
        }
    }

} // namespace tek::tokenizer
