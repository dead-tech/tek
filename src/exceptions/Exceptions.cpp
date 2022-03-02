#include "Exceptions.hpp"

#include <utility>

namespace tek::exceptions {

    ParseError::ParseError() : std::runtime_error("") {}

    RuntimeError::RuntimeError(tokenizer::Token op, std::string message)
      : std::runtime_error(""), op{ std::move(op) }, message{ std::move(message) }
    {}

    Return::Return(types::Literal retval) : std::runtime_error(""), retval{ std::move(retval) } {}
}// namespace tek::exceptions
