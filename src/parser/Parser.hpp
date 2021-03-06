#ifndef TEK_PARSER_HPP
#define TEK_PARSER_HPP

#include "../exceptions/Exceptions.hpp"
#include "../logger/Logger.hpp"
#include "../tokenizer/Token.hpp"
#include "Expressions.hpp"
#include "Statements.hpp"
#include <optional>
#include <utility>
#include <vector>

namespace tek::parser {
    class Parser
    {
      private:
        using TokensVec     = std::vector<tokenizer::Token>;
        using ExpressionPtr = std::unique_ptr<Expression>;
        using StatementPtr  = std::unique_ptr<Statement>;
        using StatementsVec = std::vector<StatementPtr>;

      public:
        explicit Parser(TokensVec tokens);
        [[nodiscard]] std::optional<StatementsVec> parse();

      private:
        [[nodiscard]] ExpressionPtr expression();
        [[nodiscard]] ExpressionPtr equality();
        [[nodiscard]] ExpressionPtr assignment();
        [[nodiscard]] ExpressionPtr comparison();
        [[nodiscard]] ExpressionPtr term();
        [[nodiscard]] ExpressionPtr factor();
        [[nodiscard]] ExpressionPtr unary();
        [[nodiscard]] ExpressionPtr primary();
        [[nodiscard]] ExpressionPtr logical_or();
        [[nodiscard]] ExpressionPtr logical_and();
        [[nodiscard]] ExpressionPtr call();

        [[nodiscard]] StatementPtr  statement();
        [[nodiscard]] StatementPtr  declaration();
        [[nodiscard]] StatementPtr  print_statement();
        [[nodiscard]] StatementPtr  expression_statement();
        [[nodiscard]] StatementPtr  var_statement();
        [[nodiscard]] StatementsVec block_statement();
        [[nodiscard]] StatementPtr  if_statement();
        [[nodiscard]] StatementPtr  while_statement();
        [[nodiscard]] StatementPtr  for_statement();
        [[nodiscard]] StatementPtr  function_statement(const std::string &kind);
        [[nodiscard]] StatementPtr  return_statement();

        // for loop helpers
        [[nodiscard]] StatementPtr  for_statement_initializer();
        [[nodiscard]] ExpressionPtr for_statement_condition();
        [[nodiscard]] ExpressionPtr for_statement_increment();
        [[nodiscard]] StatementPtr  for_statement_body(ExpressionPtr &increment);

        // call helpers
        [[nodiscard]] ExpressionPtr finish_call(ExpressionPtr &expression);

        template<typename Match>
        [[nodiscard]] constexpr bool match(Match &&match);

        template<typename... Matches>
        [[nodiscard]] constexpr bool match(Matches &&...matches);

        bool check(const tokenizer::TokenType &type);
        bool is_at_end();

        tokenizer::Token advance();
        tokenizer::Token peek();
        tokenizer::Token previous();
        tokenizer::Token consume(const tokenizer::TokenType &type, const std::string &message);

        void synchronize();

        static exceptions::ParseError error(const tokenizer::Token &token, const std::string &message);

      private:
        TokensVec   tokens;
        std::size_t current;
    };
}// namespace tek::parser

#endif// TEK_PARSER_HPP
