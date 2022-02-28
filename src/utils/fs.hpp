#ifndef FS_HPP
#define FS_HPP

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <fmt/format.h>

namespace tek::fs {
    [[nodiscard]] std::string read_file(const std::filesystem::path &path);
}// namespace tek::fs

#endif
