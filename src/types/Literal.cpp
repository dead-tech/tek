#include "Literal.hpp"

namespace tek::types {
    Literal::Literal(Literal::variant_t literal)
        : literal{std::move(literal)} {}

    std::string Literal::get() const {
        ValueVisitor visitor{
            [](double value) -> std::string { return std::to_string(value); },
            [](const std::string& str) -> std::string { return str; }};
        return std::visit(visitor, literal);
    }

    std::string Literal::get() {
        ValueVisitor visitor{
            [](double value) -> std::string { return std::to_string(value); },
            [](const std::string& str) -> std::string { return str; }};
        return std::visit(visitor, literal);
    }
} // namespace tek::types
