#ifndef TEK_LITERAL_HPP
#define TEK_LITERAL_HPP

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "Callable.hpp"


namespace tek::types {
    template<typename... Visitors>
    struct ValueVisitor : Visitors...
    {
        using Visitors::operator()...;
    };

    template<typename... Visitors>
    ValueVisitor(Visitors...) -> ValueVisitor<Visitors...>;

    struct Literal
    {
      public:
        using variant_t   = std::variant<double, std::string, bool, std::nullptr_t, NativeCallable, TekFunction>;
        using CallablePtr = std::unique_ptr<Callable>;

      public:
        explicit Literal(variant_t literal);

        [[nodiscard]] variant_t value();

        [[nodiscard]] CallablePtr as_callable();

        [[nodiscard]] std::string str() const;
        [[nodiscard]] std::string str();

      private:
        variant_t literal;
    };
}// namespace tek::types

#endif// TEK_LITERAL_HPP
