#ifndef TEK_ENVIRONMENT_HPP
#define TEK_ENVIRONMENT_HPP

#include "../exceptions/Exceptions.hpp"
#include "../tokenizer/Token.hpp"
#include "../types/Literal.hpp"
#include <string>
#include <unordered_map>

namespace tek::interpreter {
    class Environment
    {
      private:
        using EnvironmentPtr = std::shared_ptr<Environment>;

      public:
        Environment();
        explicit Environment(EnvironmentPtr enclosing);

        void                         define(const std::string &name, const types::Literal &initializer);
        [[nodiscard]] types::Literal get(const tokenizer::Token &name);
        [[nodiscard]] types::Literal get_at(const size_t distance, const std::string &name);
        void                         assign(const tokenizer::Token &name, const types::Literal &value);
        void assign_at(const size_t distance, const tokenizer::Token &name, const types::Literal &value);

      private:
        EnvironmentPtr ancestor(const size_t distance);

      private:
        std::unordered_map<std::string, types::Literal> variables;
        EnvironmentPtr                                  enclosing;
    };
}// namespace tek::interpreter


#endif// TEK_ENVIRONMENT_HPP
