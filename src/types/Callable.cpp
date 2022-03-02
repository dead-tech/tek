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

    TekFunction::TekFunction(TekFunction::FunctionStatementPtr declaration, EnvironmentPtr environment)
      : declaration{ std::move(declaration) }, closure{ std::move(environment) }
    {}

    Literal TekFunction::call(interpreter::Interpreter &interpreter, std::vector<Literal> arguments)
    {
        auto environment = std::make_shared<interpreter::Environment>(this->closure);
        for (std::size_t i = 0; i < this->declaration->parameters.size(); ++i) {
            environment->define(this->declaration->parameters.at(i).lexeme, arguments.at(i));
        }

        // Return statement it's handled through throwing an exception. Grrrr..
        try {
            interpreter.execute_block(this->declaration->body, environment);
        } catch (const exceptions::Return &ret) {
            return ret.retval;
        }
        // Return this as in dynamically typed language you could take the return value of a void a function.
        // With this you always get back nil if you try to do so.
        return types::Literal(nullptr);
    }

    std::size_t TekFunction::get_arity() const { return this->declaration->parameters.size(); }
    std::string TekFunction::to_string() const { return fmt::format("<fn {} >", this->declaration->name.lexeme); }
}// namespace tek::types
