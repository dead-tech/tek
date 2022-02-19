#ifndef FS_HPP
#define FS_HPP

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <fmt/format.h>

[[nodiscard]] std::string read_file(const std::filesystem::path& path);

#endif
