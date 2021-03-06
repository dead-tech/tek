#include <fmt/format.h>
#include <iostream>
#include <string>

#include "interpreter/Interpreter.hpp"
#include "interpreter/Resolver.hpp"
#include "logger/Logger.hpp"
#include "parser/Expressions.hpp"
#include "parser/Parser.hpp"
#include "tokenizer/Tokenizer.hpp"
#include "utils/fs.hpp"

static tek::interpreter::Interpreter interpreter;

void run(const std::string &source_code)
{
    tek::tokenizer::Tokenizer scanner(source_code);
    const auto                tokens = scanner.tokenize();

    tek::parser::Parser parser(tokens);
    auto                statements = parser.parse();

    if (!statements) { return; }

    if (tek::logger::Logger::had_error) { return; }

    tek::interpreter::Resolver resolver(interpreter);
    resolver.resolve(*statements);

    if (tek::logger::Logger::had_error) { return; }

    interpreter.interpret(*statements);

    if (tek::logger::Logger::had_runtime_error) {
        fmt::print("Runtime error\n");
        std::exit(1);
    }
}

void run_prompt()
{
    std::string input;
    while (true) {
        fmt::print("> ");
        std::getline(std::cin, input);

        if (std::cin.fail() || std::cin.eof()) { break; }

        run(input);
    }
}

void run_file(const std::string &file_path)
{
    const auto source_code = tek::fs::read_file(std::filesystem::path(file_path));
    run(source_code);

    if (tek::logger::Logger::had_error) { exit(1); }
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        run_prompt();
    } else {
        run_file(argv[1]);
    }
    return 0;
}
