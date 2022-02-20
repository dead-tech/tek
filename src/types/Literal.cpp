#include "Literal.hpp"

Literal::Literal(Literal::variant_t literal) : literal{std::move(literal)} {}

auto Literal::get() const {
    ValueVisitor visitor{
        [](double value) -> std::string { return std::to_string(value); },
        [](const std::string& str) -> std::string { return str; }};
    return std::visit(visitor, literal);
}

auto Literal::get() {
    ValueVisitor visitor{
        [](double value) -> std::string { return std::to_string(value); },
        [](const std::string& str) -> std::string { return str; }};
    return std::visit(visitor, literal);
}
