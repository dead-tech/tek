#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fmt/format.h>
#include <string>

class Logger {
  public:
    static bool had_error;

  public:
    static void report(const int line, const std::string& where,
                       const std::string& message);
    static void error(const int line, const std::string& message);
};

#endif
