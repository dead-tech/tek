#ifndef TEK_VARIANTS_HPP
#define TEK_VARIANTS_HPP

#include <tuple>
#include <variant>

namespace tek::variants {
    template<typename Alternative, typename... Variants>
    [[nodiscard]] constexpr static bool have_type_of(Variants &&...variants) noexcept
    {
        return (std::holds_alternative<Alternative>(variants) && ...);
    }

    template<typename Alternative, typename... Variants>
    [[nodiscard]] constexpr static auto to_tuple(Variants &&...variants) noexcept
    {
        return std::tuple{ std::get<Alternative>(variants)... };
    }
}// namespace tek::variants

#endif// TEK_VARIANTS_HPP
