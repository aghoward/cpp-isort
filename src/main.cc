#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "import_sorter.h"

namespace fs = std::filesystem;

std::vector<std::string> read_all_lines(const fs::path& path) {
    auto output = std::vector<std::string>();
    auto line = std::string();

    std::ifstream file;
    file.open(path);
    file.seekg(0, std::ios_base::end);
    auto file_end = file.tellg();
    file.seekg(0, std::ios_base::beg);

    while (file.tellg() < file_end) {
        std::getline(file, line);
        output.emplace_back(line);
    }
    file.close();

    return output;
}

void write_lines(const fs::path& path, const std::vector<std::string>& lines) {
    std::ofstream file;
    file.open(path);

    for (const auto& line : lines)
        file << line << std::endl;

    file.close();
}

void process_file(const std::string& filename) {
    auto path = fs::path(filename);
    auto sorter = isort::ImportSorter(path);

    if (!fs::exists(path)) {
        std::cerr << "File does not exist: " << filename << std::endl;
        return;
    }

    auto lines = read_all_lines(path);
    auto output_lines = sorter.sort(lines);
    write_lines(path, output_lines);
}

int main(int argc, char* argv[]) {
    for (auto i = 1; i < argc; i++)
        process_file(argv[i]);
    return 0;
}
