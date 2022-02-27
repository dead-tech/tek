#include "Literal.hpp"

namespace tek::types {
Literal::Literal(Literal::variant_t literal) : literal{ std::move(literal) } {}

Literal::variant_t Literal::value() const { return this->literal; }

Literal::variant_t Literal::value() { return this->literal; }

std::string Literal::str() const
{
    ValueVisitor visitor{ [](const double value) -> std::string { return std::to_string(value); },
        [](const std::string &str) -> std::string { return str; },
        [](const bool boolean) -> std::string { return boolean ? "true" : "false"; },
        [](const std::nullptr_t nil) -> std::string { return "nil"; } };
    return std::visit(visitor, literal);
}

std::string Literal::str()
{
    ValueVisitor visitor{ [](const double value) -> std::string { return std::to_string(value); },
        [](const std::string &str) -> std::string { return str; },
        [](const bool boolean) -> std::string { return boolean ? "true" : "false"; },
        [](const std::nullptr_t nil) -> std::string { return "nil"; } };
    return std::visit(visitor, literal);
}
}// namespace tek::types
