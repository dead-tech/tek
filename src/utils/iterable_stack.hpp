#ifndef TEK_ITERABLE_STACK_HPP
#define TEK_ITERABLE_STACK_HPP

#include <deque>
#include <stack>

namespace tek::utils {
    template<typename T, typename Container = std::deque<T>>
    class iterable_stack : public std::stack<T, Container>
    {
      private:
        using std::stack<T, Container>::c;

      public:
        auto begin() const { return std::begin(c); }
        auto end() const { return std::end(c); }

        auto begin() { return std::begin(c); }
        auto end() { return std::end(c); }
    };
}// namespace tek::utils

#endif// TEK_ITERABLE_STACK_HPP
