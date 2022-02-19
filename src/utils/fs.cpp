#include "fs.hpp"

std::string read_file(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        fmt::print(stderr, "No such file: {}", path.string());
    }

    std::ifstream file{path.string()};
    std::stringstream ss{};

    ss << file.rdbuf();
    file.close();

    return ss.str();
}
