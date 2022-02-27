#ifndef TEK_TRAITS_HPP
#define TEK_TRAITS_HPP

#include <string>
#include <typeinfo>

namespace tek::traits {
template<typename Type>
struct TypeName
{
    [[nodiscard]] static std::string get() { return typeid(Type).name(); }
};

template<>
struct TypeName<double>
{
    [[nodiscard]] static std::string get() { return "double"; }
};

template<>
struct TypeName<std::string>
{
    [[nodiscard]] static std::string get() { return "string"; }
};

template<>
struct TypeName<bool>
{
    [[nodiscard]] static std::string get() { return "bool"; }
};

template<>
struct TypeName<std::nullptr_t>
{
    [[nodiscard]] static std::string get() { return "nil"; }
};
}// namespace tek::traits

#endif// TEK_TRAITS_HPP
