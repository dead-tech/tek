#include <fmt/format.h>
#include <iostream>
#include <string>

#include "logger/Logger.hpp"
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

int main(int argc, char** argv) {
    if (argc < 2) {
        usage();
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        run_prompt();
    }
    return 0;
}
