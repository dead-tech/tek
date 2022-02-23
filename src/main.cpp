#include <fmt/format.h>
#include <iostream>
#include <string>

#include "logger/Logger.hpp"
#include "parser/AstPrinter.hpp"
#include "parser/Expressions.hpp"
#include "tokenizer/Token.hpp"
#include "tokenizer/Tokenizer.hpp"
#include "utils/fs.hpp"

void usage() { fmt::print("Usage: tek [file]\n"); }

void run(const std::string& source_code) {
    tek::tokenizer::Tokenizer scanner(source_code);
    const auto tokens = scanner.tokenize();

    for (const auto& token : tokens) {
        fmt::print("{}\n", token.to_string());
    }
}

void run_prompt() {
    std::string input;
    while (true) {
        fmt::print("> ");
        std::getline(std::cin, input);

        if (std::cin.fail() || std::cin.eof()) {
            break;
        }

        run(input);
    }
}

void run_file(const std::string& file_path) {
    const auto source_code =
        tek::fs::read_file(std::filesystem::path(file_path));
    run(source_code);

    if (tek::logger::Logger::had_error) {
        exit(1);
    }
}

int main2(int argc, char** argv) {
    if (argc < 2) {
        usage();
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        run_prompt();
    }

    return 0;
}

int main() {
    std::unique_ptr<tek::parser::Expression> expression =
        std::make_unique<tek::parser::BinaryExpression>(
            std::make_unique<tek::parser::UnaryExpression>(
                tek::tokenizer::Token(tek::tokenizer::TokenType::MINUS, "-", "",
                                      1),
                std::make_unique<tek::parser::LiteralExpression>(123.0)),
            tek::tokenizer::Token(tek::tokenizer::TokenType::STAR, "*", "", 1),
            std::make_unique<tek::parser::GroupingExpression>(
                std::make_unique<tek::parser::LiteralExpression>(45.67)));

    fmt::print("{}\n", tek::parser::AstPrinter().print(std::move(expression)));
}
