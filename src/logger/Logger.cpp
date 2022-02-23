#include "Logger.hpp"

namespace tek::logger {

    bool Logger::had_error = false;

    void Logger::report(const std::size_t line, const std::string& where,
                        const std::string& message) {
        fmt::print("line {} -> Error {} : {}\n", line, where, message);
        Logger::had_error = true;
    }

    void Logger::error(const std::size_t line, const std::string& message) {
        report(line, "", message);
    }
} // namespace tek::logger
