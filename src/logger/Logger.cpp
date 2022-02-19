#include "Logger.hpp"

bool Logger::had_error = false;

void Logger::report(const int line, const std::string& where,
                    const std::string& message) {
    fmt::print("line {} -> Error {} : {}\n", line, where, message);
    Logger::had_error = true;
}

void Logger::error(const int line, const std::string& message) {
    report(line, "", message);
}
