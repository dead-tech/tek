//
// Created by dead on 01/03/22.
//

#include "Callable.hpp"

#include "../interpreter/Environment.hpp"
#include "../interpreter/Interpreter.hpp"
#include "Literal.hpp"
#include <utility>

namespace tek::types {
    NativeCallable::NativeCallable(std::string identifier, const Callable::FnPtr &cpp_function, const std::size_t arity)
      : identifier{ std::move(identifier) }, cpp_function{ cpp_function }, arity{ arity }
    {}

    Literal NativeCallable::call(tek::interpreter::Interpreter &interpreter, std::vector<Literal> arguments)
    {
        return Literal("");
    }

    std::size_t NativeCallable::get_arity() const { return 0; }

    std::string NativeCallable::to_string() const { return "native function"; }

    TekFunction::TekFunction(TekFunction::FunctionStatementPtr declaration) : declaration{ std::move(declaration) } {}

    Literal TekFunction::call(interpreter::Interpreter &interpreter, std::vector<Literal> arguments)
    {
        auto environment = std::make_shared<interpreter::Environment>(interpreter.globals);
        for (std::size_t i = 0; i < this->declaration->parameters.size(); ++i) {
            environment->define(this->declaration->parameters.at(i).lexeme, arguments.at(i));
        }

        interpreter.execute_block(this->declaration->body, environment);
        // Useless value to return, but returning to shut the compiler warning !!
        return types::Literal("");
    }

    std::size_t TekFunction::get_arity() const { return this->declaration->parameters.size(); }
    std::string TekFunction::to_string() const { return fmt::format("<fn {} >", this->declaration->name.lexeme); }
}// namespace tek::types
