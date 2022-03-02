#include "Literal.hpp"

namespace tek::types {
    Literal::Literal(Literal::variant_t literal) : literal{ std::move(literal) } {}


    Literal::variant_t Literal::value() { return this->literal; }

    Literal::CallablePtr Literal::as_callable()
    {
        if (std::holds_alternative<NativeCallable>(this->literal)) {
            return std::make_unique<NativeCallable>(std::get<NativeCallable>(this->literal));
        } else if (std::holds_alternative<TekFunction>(this->literal)) {
            return std::make_unique<TekFunction>(std::move(std::get<TekFunction>(this->literal)));
        }

        // unreachable
        return nullptr;
    }

    std::string Literal::str() const
    {
        ValueVisitor visitor{ [](const double value) -> std::string { return std::to_string(value); },
                              [](const std::string &str) -> std::string { return str; },
                              [](const bool boolean) -> std::string { return boolean ? "true" : "false"; },
                              [](const std::nullptr_t nil) -> std::string { return "nil"; },
                              [](const NativeCallable &callable) -> std::string { return "native callable"; },
                              [](const TekFunction &callable) -> std::string { return "tek callable"; } };
        return std::visit(visitor, literal);
    }

    std::string Literal::str()
    {
        ValueVisitor visitor{ [](const double value) -> std::string { return std::to_string(value); },
                              [](const std::string &str) -> std::string { return str; },
                              [](const bool boolean) -> std::string { return boolean ? "true" : "false"; },
                              [](const std::nullptr_t nil) -> std::string { return "nil"; },
                              [](const NativeCallable &callable) -> std::string { return "native callable"; },
                              [](const TekFunction &callable) -> std::string { return "tek callable"; } };
        return std::visit(visitor, literal);
    }
}// namespace tek::types
