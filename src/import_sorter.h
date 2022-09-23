#pragma once

#include <filesystem>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

namespace isort {
    using namespace std::string_literals;
    namespace fs = std::filesystem;

    std::string get_matching_header(const fs::path& filename);
    std::regex build_this_header_import_matcher(
            const fs::path& filename,
            const std::vector<std::string>& ignored_paths);

    class ImportSorter {
        private:
            std::regex _this_header_import_matcher;
            std::regex _builtin_import_matcher;
            std::regex _third_party_import_matcher;
            std::regex _local_import_matcher;

            bool is_this_header_import(const std::string& line) const;
            bool is_third_party_import(const std::string& line) const;
            bool is_builtin_import(const std::string& line) const;
            bool is_local_import(const std::string& line) const;
            bool is_any_import(const std::string& line) const;

            bool is_import_section_line(const std::string& line) const;

            std::vector<std::string> sort_section(const std::vector<std::string>& lines) const;

            template <typename It>
            std::tuple<It, It> get_section(const It& begin, const It& end) const {
                auto section_begin = std::find_if(begin, end, [&] (const auto& l) { return is_any_import(l); });
                if (section_begin == end)
                    return {end, end};
                auto section_end = std::find_if_not(section_begin, end, [&] (const auto& l) { return is_import_section_line(l); });
                return {section_begin, section_end};
            }

        public:
            ImportSorter(const fs::path& filename, const std::vector<std::string>& ignored_paths)
                :
                _this_header_import_matcher(build_this_header_import_matcher(filename, ignored_paths)),
                _builtin_import_matcher("^\\s*#include[^<]*<[\\w\\d\\s/]*>.*"s),
                _third_party_import_matcher("^\\s*#include[^<]*<[\\w\\d\\s/]*\\.h>.*"s),
                _local_import_matcher("^\\s*#include[^\"]*\"[\\w\\d\\s/]*\\.h\".*"s)
            {}

            ImportSorter(const fs::path& filename) : ImportSorter(filename, {}) {}

            std::vector<std::string> sort(const std::vector<std::string>& lines) const;
    };
}
