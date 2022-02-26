#ifndef TEK_LITERAL_HPP
#define TEK_LITERAL_HPP

#include <string>
#include <variant>
#include <vector>

namespace tek::types {
    template <typename... Visitors>
    struct ValueVisitor : Visitors... {
        using Visitors::operator()...;
    };

    template <typename... Visitors>
    ValueVisitor(Visitors...) -> ValueVisitor<Visitors...>;

    struct Literal {
      public:
        using variant_t =
            std::variant<double, std::string, bool, std::nullptr_t>;

      public:
        explicit Literal(variant_t literal);

        [[nodiscard]] variant_t value() const;
        [[nodiscard]] variant_t value();

        [[nodiscard]] std::string str() const;
        [[nodiscard]] std::string str();

      private:
        variant_t literal;
    };
} // namespace tek::types

#endif // TEK_LITERAL_HPP
