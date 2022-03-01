#ifndef TEK_GUARD_HPP
#define TEK_GUARD_HPP

namespace tek::utils {
    template<typename Lambda>
    class ScopeGuard
    {
      public:
        explicit ScopeGuard(Lambda cleanup) : cleanup{ cleanup } {}
        ~ScopeGuard() { this->cleanup(); }

      private:
        Lambda cleanup;
    };
}// namespace tek::utils

#endif// TEK_GUARD_HPP
