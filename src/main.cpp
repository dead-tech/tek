#include <any>
#include <charconv>
#include <fmt/format.h>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
// TODO: There's gotta be a better way
#include <iostream>

class Logger {
public:
  static bool had_error;

public:
  static void report(const int line, const std::string &where,
                     const std::string &message) {
    fmt::print("line {} -> Error {} : {}\n", line, where, message);
    Logger::had_error = true;
  }

  static void error(const int line, const std::string &message) {
    report(line, "", message);
  }
};

bool Logger::had_error = false;

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

  ENDOF
};

std::string token_type_to_str(const TokenType token_type) {
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
    break;
  }
}

struct Token {
  TokenType type;
  std::string lexeme;
  std::any literal;
  std::size_t line;

  Token() = default;

  [[nodiscard("Why would you?")]] std::string to_string() const noexcept {

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
      return fmt::format(
          "[\n\tLexeme: {},\n\tType: {},\n\tLine: {},\n]",
          this->lexeme, token_type_to_str(this->type), this->line);
    }
  }
};

// TODO: rename this to tokenizer
class Scanner {
public:
  Scanner(const std::string &source_code) : source{source_code} {}

  // TODO: rename this to tokenize
  std::vector<Token> scan_tokens() {
    while (!this->is_at_end()) {
      this->start = this->current;
      this->scan_token();
    }

    this->tokens.push_back(Token{.type = TokenType::ENDOF,
                                 .lexeme = "",
                                 .literal = "",
                                 .line = this->line});
    return this->tokens;
  }

private:
  bool is_at_end() const { return this->current >= this->source.length(); }

  char advance() { return this->source[this->current++]; }

  char peek() {
    if (this->is_at_end()) {
      return '\0';
    }
    return this->source[this->current];
  }

  char peek_next() {
    if (this->current + 1 >= this->source.length()) {
      return '\0';
    }
    return this->source[this->current + 1];
  }

  bool match_next(const char expected) {
    if (this->is_at_end()) {
      return false;
    }

    if (this->source[current] != expected) {
      return false;
    }

    this->current++;
    return true;
  }

  void add_token(const TokenType type) { this->add_token(type, ""); }

  void add_token(const TokenType type, const std::any &literal) {
    const auto lexeme =
        this->source.substr(this->start, this->current - this->start);
    this->tokens.push_back(Token{type, lexeme, literal, this->line});
  }

  void string_literal() {
    while (this->peek() != '"' && !this->is_at_end()) {
      if (this->peek() == '\n') {
        this->line++;
      }
      this->advance();
    }

    this->advance();

    const auto string_literal =
        this->source.substr(this->start + 1, this->current - this->start - 2);
    this->add_token(TokenType::STRING, string_literal);
  }

  // TODO: Probably better ways to do this
  void number_literal() {
    while (std::isdigit(this->peek())) {
      this->advance();
    }

    if (this->peek() == '.' && std::isdigit(this->peek_next())) {
      this->advance();

      // TODO: this could be a do while
      while (std::isdigit(this->peek())) {
        this->advance();
      }
    }

    const auto str_number =
        this->source.substr(this->start, this->current - this->start);
    int number{};
    std::from_chars(str_number.data(), str_number.data() + str_number.size(),
                    number);
    this->add_token(TokenType::NUMBER, number);
  }

  void identifier() {
    while (std::isalnum(this->peek())) {
      this->advance();
    }

    const auto text =
        this->source.substr(this->start, this->current - this->start);
    const auto it = this->keywords.find(text);
    auto token_type = TokenType::IDENTIFIER;
    if (it != this->keywords.end()) {
      token_type = it->second;
    }
    this->add_token(token_type);
  }

  void scan_token() {
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
    // TODO: check if this can be done with std::isspace
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
        Logger::error(this->line, fmt::format("Unexepected character: {}", c));
      }
      break;
    }
  }

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

void usage() { fmt::print("Usage: tek [file]\n"); }

void run(const std::string &source_code) {
  // TODO: Rename this to tokenizer
  Scanner scanner(source_code);
  const auto tokens = scanner.scan_tokens();

  for (const auto token : tokens) {
    fmt::print("{}\n", token.to_string());
  }
}

void run_prompt() {
  std::string input;
  while (true) {
    fmt::print("> ");
    std::getline(std::cin, input);

    if (std::cin.fail() || std::cin.eof()) {
      break;
    }

    run(input);
  }
}

// TODO: Make this a std::filesystem::path
std::string read_file(const std::string &file_path) {
  std::ifstream file{file_path};
  std::stringstream ss;

  ss << file.rdbuf();

  file.close();
  return ss.str();
}

// TODO: Make this a std::filesystem::path
void run_file(const std::string &file_path) {
  const auto source_code = read_file(file_path);
  run(source_code);

  if (Logger::had_error) {
    exit(69);
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    usage();
  } else if (argc == 2) {
    run_file(argv[1]);
  } else {
    run_prompt();
  }

  return 0;
}