#include "data.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "trail_parser.hpp"

static std::ifstream open_file(const std::string& filepath);

Trail load_trail(const std::string& filename) {
    auto file = open_file(filename);
    TrailParser parser;
    parser.parse(file);
    if (!parser.is_done())
        throw TrailParserException(parser);
    return parser.result();
}

std::ifstream open_file(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file)
        throw std::invalid_argument("File not found");
    return file;
}
