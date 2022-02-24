#include "../../src/parser/AstPrinter.hpp"
#include "../../src/parser/Parser.hpp"
#include "../../src/tokenizer/Tokenizer.hpp"
#include <catch2/catch.hpp>

TEST_CASE("Parser parse expressions (succeed)", "[Parser]") {
    tek::tokenizer::Tokenizer scanner("60 + 9 / 440 - 20");
    const auto tokens = scanner.tokenize();

    tek::parser::Parser parser(tokens);
    std::unique_ptr<tek::parser::Expression> expression = parser.parse();

    if (tek::logger::Logger::had_error) {
        fmt::print("Error!\n");
        return;
    }

    tek::parser::AstPrinter ast_printer;
    REQUIRE(ast_printer.print(std::move(expression)) ==
            "(- (+ 60.000000 (/ 9.000000 440.000000)) 20.000000)");
}
