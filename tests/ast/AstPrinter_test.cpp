#include "../../src/parser/AstPrinter.hpp"
#include <catch2/catch.hpp>

TEST_CASE("AstPrinter test (succeed)", "[AstPrinter]")
{
    tek::parser::AstPrinter                  ast_printer;
    std::unique_ptr<tek::parser::Expression> expression = std::make_unique<tek::parser::BinaryExpression>(
      std::make_unique<tek::parser::UnaryExpression>(
        tek::tokenizer::Token(tek::tokenizer::TokenType::MINUS, "-", "", 1),
        std::make_unique<tek::parser::LiteralExpression>(123.0)),
      tek::tokenizer::Token(tek::tokenizer::TokenType::STAR, "*", "", 1),
      std::make_unique<tek::parser::GroupingExpression>(std::make_unique<tek::parser::LiteralExpression>(45.67)));
    REQUIRE(ast_printer.print(std::move(expression)) == "(* (- 123.000000) (group 45.670000))");
}
