#include "import_sorter.h"

#include <filesystem>
#include <functional>
#include <iterator>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

namespace isort {
    using namespace std::string_literals;
    namespace fs = std::filesystem;

    std::string get_matching_header(const fs::path& filename) {
        auto extension = filename.extension().string();
        auto path = filename.string();
        auto start_index = path.size() - extension.size();
        return path.erase(start_index);
    }

    std::regex build_this_header_import_matcher(
            const fs::path& filename,
            const std::vector<std::string>& ignored_paths) {
        auto filename_string = filename.string();
        auto ignored_path = std::find_if(
                ignored_paths.begin(),
                ignored_paths.end(),
                [&] (const auto& p) { return filename_string.substr(0, p.size()) == p; });
        if (ignored_path != ignored_paths.end())
            filename_string = filename_string.substr(ignored_path->size(), filename_string.size());
        return std::regex("^\\s*#include[^\"]*\""s + get_matching_header(filename_string) + "\\.h\".*"s);
    }

    bool ImportSorter::is_this_header_import(const std::string& line) const {
        return std::regex_match(line, _this_header_import_matcher);
    }

    bool ImportSorter::is_third_party_import(const std::string& line) const {
        return std::regex_match(line, _third_party_import_matcher);
    }

    bool ImportSorter::is_builtin_import(const std::string& line) const {
        return std::regex_match(line, _builtin_import_matcher) && !is_third_party_import(line);
    }

    bool ImportSorter::is_local_import(const std::string& line) const {
        return std::regex_match(line, _local_import_matcher) && !is_this_header_import(line);
    }

    bool ImportSorter::is_any_import(const std::string& line) const {
        return is_builtin_import(line) ||
                is_third_party_import(line) ||
                is_local_import(line) ||
                is_this_header_import(line);
    }

    bool is_empty_line(const std::string& line) {
        if (line.size() == 0)
            return true;
        auto regex = std::regex("^\\s*$");
        return std::regex_match(line, regex);
    }

    bool ImportSorter::is_import_section_line(const std::string& line) const {
        return is_any_import(line) || is_empty_line(line);
    }

    std::string get_header_name(const std::string& line) {
        auto regex = std::regex("^\\s*#include[^(\"|<)]*[<|\"]([^(>|\")]*)[>|\"].*"s);
        std::smatch match;
        std::regex_match(line, match, regex);
        return match[1].str();
    }

    std::vector<std::string> get_all_imports(
            const std::vector<std::string>& lines,
            const std::function<bool(const std::string&)>& matcher) {
        auto imports = std::vector<std::string>();
        auto it = std::find_if(std::begin(lines), std::end(lines), matcher);
        while (it != std::end(lines)) {
            imports.emplace_back(*it);
            it = std::find_if(std::next(it), std::end(lines), matcher);
        }
        return imports;
    }

    std::vector<std::string> get_sorted_imports(
            const std::vector<std::string>& lines,
            const std::function<bool(const std::string&)>& matcher) {
        auto imports = get_all_imports(lines, matcher);
        std::sort(
            std::begin(imports),
            std::end(imports),
            [] (const auto& left, const auto& right) {
                return get_header_name(left).compare(get_header_name(right)) < 0;
            });
        return imports;
    }

    void append_import_lines(std::vector<std::string>& output, const std::vector<std::string>& to_add) {
        if (to_add.size() > 0) {
            if (output.size() > 0)
                output.emplace_back(""s);
            output.insert(std::end(output), std::begin(to_add), std::end(to_add));
        }
    }

    std::vector<std::string> get_beginning_padding(const std::vector<std::string>& lines) {
        auto it = std::find_if_not(std::begin(lines), std::end(lines), is_empty_line);
        return {std::begin(lines), it};
    }

    std::vector<std::string> get_ending_padding(const std::vector<std::string>& lines) {
        auto it = std::find_if_not(std::rbegin(lines), std::rend(lines), is_empty_line);
        auto distance = static_cast<long>(lines.size()) - std::distance(std::rbegin(lines), it);
        auto begin = std::next(std::begin(lines), distance);
        return {begin, std::end(lines)};
    }

    std::vector<std::string> ImportSorter::sort_section(const std::vector<std::string>& lines) const {
        if (!std::all_of(std::begin(lines), std::end(lines), [&] (const auto& l) { return is_import_section_line(l); }) ||
            std::all_of(std::begin(lines), std::end(lines), is_empty_line))
            return lines;

        auto pre_padding = get_beginning_padding(lines);
        auto header_import = get_sorted_imports(lines, [&] (const auto& l) { return is_this_header_import(l); });
        auto builtin_imports = get_sorted_imports(lines, [&] (const auto& l) { return is_builtin_import(l); }); 
        auto third_party_imports = get_sorted_imports(lines, [&] (const auto& l) { return is_third_party_import(l); });
        auto local_imports = get_sorted_imports(lines, [&] (const auto& l) { return is_local_import(l); });
        auto post_padding = get_ending_padding(lines);

        auto import_lines = std::vector<std::string>();
        append_import_lines(import_lines, header_import);
        append_import_lines(import_lines, builtin_imports);
        append_import_lines(import_lines, third_party_imports);
        append_import_lines(import_lines, local_imports);

        import_lines.insert(std::begin(import_lines), std::begin(pre_padding), std::end(pre_padding));
        import_lines.insert(std::end(import_lines), std::begin(post_padding), std::end(post_padding));

        return import_lines;
    }

    std::vector<std::string> ImportSorter::sort(const std::vector<std::string>& lines) const {
        auto output = lines;
        auto iterators = get_section(std::begin(output), std::end(output));
        while (std::get<0>(iterators) != std::get<1>(iterators)) {
            auto& [begin, end] = iterators;
            auto new_section = sort_section({begin, end});
            auto old_begin = std::vector<std::string>({std::begin(output), begin});
            auto old_end = std::vector<std::string>({end, std::end(output)});
            output = old_begin;
            output.insert(std::end(output), std::begin(new_section), std::end(new_section));
            output.insert(std::end(output), std::begin(old_end), std::end(old_end));
            auto offset = static_cast<long>(old_begin.size() + new_section.size());
            end = std::next(std::begin(output), offset);
            iterators = get_section(end, std::end(output));
        }

        return output;
    }
}
