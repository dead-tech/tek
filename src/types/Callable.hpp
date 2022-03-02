#ifndef TEK_CALLABLE_HPP
#define TEK_CALLABLE_HPP

#include <memory>
#include <string>
#include <vector>

namespace tek::parser {
    class FunctionStatement;
}

namespace tek::interpreter {
    class Interpreter;
}

namespace tek::types {
    struct Literal;

    class Callable
    {
      public:
        using FnPtr = Literal (*)(interpreter::Interpreter &interpreter, const std::vector<Literal> &arguments);

      public:
        [[nodiscard]] virtual Literal call(interpreter::Interpreter &interpreter, std::vector<Literal> arguments) = 0;
        [[nodiscard]] virtual std::size_t get_arity() const                                                       = 0;
        [[nodiscard]] virtual std::string to_string() const                                                       = 0;

        friend constexpr bool operator==(const Callable &rhs, const Callable &lhs) { return true; }
    };

    class NativeCallable : public Callable
    {
      public:
        NativeCallable(std::string identifier, const FnPtr &cpp_function, const std::size_t arity);

        [[nodiscard]] Literal     call(interpreter::Interpreter &interpreter, std::vector<Literal> arguments) override;
        [[nodiscard]] std::size_t get_arity() const override;
        [[nodiscard]] std::string to_string() const override;

      public:
        std::string identifier;

      private:
        FnPtr       cpp_function;
        std::size_t arity;
    };

    class TekFunction : public Callable
    {
      public:
        using FunctionStatementPtr = std::shared_ptr<parser::FunctionStatement>;

      public:
        explicit TekFunction(FunctionStatementPtr declaration);

        [[nodiscard]] Literal     call(interpreter::Interpreter &interpreter, std::vector<Literal> arguments) override;
        [[nodiscard]] std::size_t get_arity() const override;
        [[nodiscard]] std::string to_string() const override;

      private:
        FunctionStatementPtr declaration;
    };
}// namespace tek::types


#endif// TEK_CALLABLE_HPP
