#include "Exceptions.hpp"

#include <utility>

namespace tek::exceptions {

ParseError::ParseError() : std::runtime_error("") {}

RuntimeError::RuntimeError(tokenizer::Token op, std::string message)
  : std::runtime_error(""), op{ std::move(op) }, message{ std::move(message) }
{}
}// namespace tek::exceptions
