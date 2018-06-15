#include "file.hpp"

#include <fstream>

std::vector<uint8_t> read_file(const char* file_name) {
    std::ifstream file{file_name, std::ios::ate | std::ios::binary | std::ios::in};
    if (!file.is_open()) throw "Failed to open file.";
    const auto file_size = static_cast<size_t>(file.tellg());
    std::vector<uint8_t> buffer{file_size};
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(buffer.data()), file_size);
    return buffer;
}