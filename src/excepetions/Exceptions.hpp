#ifndef TEK_EXCEPTIONS_HPP
#define TEK_EXCEPTIONS_HPP

#include "../tokenizer/Token.hpp"
#include <stdexcept>
#include <utility>

namespace tek::exceptions {
    class ParseError : public std::runtime_error {
      public:
        ParseError();
    };

    class RuntimeError : public std::runtime_error {
      public:
        RuntimeError(tokenizer::Token op, std::string message);

      public:
        tokenizer::Token op;
        std::string message;
    };
} // namespace tek::exceptions

#endif // TEK_EXCEPTIONS_HPP
