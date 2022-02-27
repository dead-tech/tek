#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "../exceptions/Exceptions.hpp"
#include "../tokenizer/Token.hpp"
#include <fmt/format.h>
#include <string>

namespace tek::logger {
class Logger
{
  public:
    // TODO: Do something about these maybe?
    static bool had_error;
    static bool had_runtime_error;

  public:
    static void report(const std::size_t line, const std::string &where, const std::string &message);
    static void error(const tokenizer::Token &token, const std::string &message);
    static void runtime_error(const exceptions::RuntimeError &error);
};
}// namespace tek::logger

#endif
