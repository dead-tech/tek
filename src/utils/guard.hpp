#ifndef TEK_GUARD_HPP
#define TEK_GUARD_HPP

namespace tek::utils {
template<typename Target, typename Lambda>
class ScopeGuard
{
  public:
    ScopeGuard(Target target, Lambda cleanup) : cleanup{ cleanup }, target{ target } {}
    ~ScopeGuard() { this->cleanup(target); }

  private:
    Target target;
    Lambda cleanup;
};
}// namespace tek::utils

#endif// TEK_GUARD_HPP
