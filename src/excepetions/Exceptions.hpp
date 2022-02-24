#ifndef TEK_EXCEPTIONS_HPP
#define TEK_EXCEPTIONS_HPP

#include <stdexcept>

namespace exceptions {
    class ParseError : public std::runtime_error {
      public:
        ParseError();
    };
} // namespace exceptions

#endif // TEK_EXCEPTIONS_HPP
