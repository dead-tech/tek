#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "../tokenizer/Token.hpp"
#include <fmt/format.h>
#include <string>

namespace tek::logger {
    class Logger {
      public:
        static bool had_error;

      public:
        static void report(const std::size_t line, const std::string& where,
                           const std::string& message);
        static void error(const tokenizer::Token& token,
                          const std::string& message);
    };
} // namespace tek::logger

#endif
