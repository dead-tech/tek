#include "Logger.hpp"

namespace tek::logger {

    bool Logger::had_error = false;
    bool Logger::had_runtime_error = false;

    void Logger::report(const std::size_t line, const std::string& where,
                        const std::string& message) {
        fmt::print("line {} -> Error {} : {}\n", line, where, message);
        Logger::had_error = true;
    }

    void Logger::error(const tokenizer::Token& token,
                       const std::string& message) {
        if (token.type == tokenizer::TokenType::ENDOF) {
            report(token.line, "at the end", message);
        } else {
            report(token.line, "at '" + token.lexeme + "'", message);
        }
    }

    void Logger::runtime_error(const exceptions::RuntimeError& error) {
        fmt::print("{}\n[line {} ]\n", error.message, error.op.line);
        Logger::had_runtime_error = true;
    }
} // namespace tek::logger
