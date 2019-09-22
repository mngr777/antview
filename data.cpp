#include "data.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

static std::string load_file_to_string(const std::string& filepath);

Ant::Trail load_trail(const std::string& filename) {
    std::string str = load_file_to_string(filename);
    
}

std::string load_file_to_string(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file)
        throw std::invalid_argument("File not found");
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}
