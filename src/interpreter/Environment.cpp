#include "Environment.hpp"

namespace tek::interpreter {

    Environment::Environment() : enclosing{ nullptr } {}

    Environment::Environment(EnvironmentPtr enclosing) : enclosing{ std::move(enclosing) } {}

    void Environment::define(const std::string &name, const tek::types::Literal &initializer)
    {
        this->variables.insert_or_assign(name, initializer);
    }

    types::Literal Environment::get(const tokenizer::Token &name)
    {
        const auto it = this->variables.find(name.lexeme);
        if (it != this->variables.end()) { return this->variables.at(name.lexeme); }

        if (this->enclosing != nullptr) { return this->enclosing->get(name); }

        throw exceptions::RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    // TODO: Fix variables shadowing
    void Environment::assign(const tokenizer::Token &name, const types::Literal &value)
    {
        const auto it = this->variables.find(name.lexeme);
        if (it != this->variables.end()) {
            this->variables.at(name.lexeme) = value;
            return;
        }

        if (this->enclosing != nullptr) {
            this->enclosing->assign(name, value);
            return;
        }

        throw exceptions::RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

}// namespace tek::interpreter
