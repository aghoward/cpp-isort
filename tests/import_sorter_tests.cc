#include <string>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "import_sorter.h"

namespace isort::tests {
    using namespace std::string_literals;
    using namespace ::testing;

    class ImportSorterTests : public Test {
        protected:
            ImportSorter _subject;

            template <typename ... Ts>
            void assert_elements_equal(const std::vector<std::string>& actual, Ts&& ... expected) const
            {
                ASSERT_THAT(actual, ElementsAre(StrEq(expected)...));
            }

        public:
            ImportSorterTests()
                : _subject()
            {}
    };

    TEST_F(ImportSorterTests, sort_given_empty_input_returns_empty_list) {
        auto result = _subject.sort({});

        ASSERT_THAT(result, IsEmpty());
    }

    TEST_F(ImportSorterTests, sort_given_no_import_lines_returns_unmodified_lines) {
        auto result = _subject.sort({"foo"s, "bar"s, "\n"s, "baz"s});

        assert_elements_equal(result, "foo"s, "bar"s, "\n"s, "baz"s);
    }

    TEST_F(ImportSorterTests, sort_given_single_line_returns_single_line) {
        auto result = _subject.sort({"#include <foo>"s});

        assert_elements_equal(result, "#include <foo>"s);
    }
    TEST_F(ImportSorterTests, sort_given_two_ordered_import_lines_does_not_reorder_lines) {
        auto result = _subject.sort({"#include <foo>"s, "#include <xylophone>"});

        assert_elements_equal(result, "#include <foo>"s, "#include <xylophone>"s);
    }

    TEST_F(ImportSorterTests, sort_given_two_unordered_import_lines_reorders_lines) {
        auto result = _subject.sort({"#include <foo>"s, "#include <bar>"s});

        assert_elements_equal(result, "#include <bar>"s, "#include <foo>"s);
    }

    TEST_F(ImportSorterTests, sort_given_builtin_import_lines_can_sort_lines) {
        auto result = _subject.sort({"#include <beta>"s, "#include <alpha>"s});

        assert_elements_equal(result, "#include <alpha>"s, "#include <beta>"s);
    }

    TEST_F(ImportSorterTests, sort_given_third_party_import_lines_can_sort_lines) {
        auto result = _subject.sort({"#include <beta.h>"s, "#include <alpha.h>"s});

        assert_elements_equal(result, "#include <alpha.h>"s, "#include <beta.h>"s);
    }

    TEST_F(ImportSorterTests, sort_given_local_import_lines_can_sort_lines) {
        auto result = _subject.sort({"#include \"beta.h\""s, "#include \"alpha.h\""s});

        assert_elements_equal(result, "#include \"alpha.h\""s, "#include \"beta.h\""s);
    }

    TEST_F(ImportSorterTests, sort_given_multiple_unordered_import_lines_reorders_lines) {
        auto result = _subject.sort({"#include <f>"s, "#include <g>"s, "#include <c>"s, "#include <a>"s, "#include <b>"s});

        assert_elements_equal(result, "#include <a>"s, "#include <b>"s, "#include <c>"s, "#include <f>"s, "#include <g>"s);
    }

    TEST_F(ImportSorterTests, sort_given_mixed_import_levels_separates_import_levels_by_empty_lines) {
        auto result = _subject.sort({"#include \"alpha.h\""s, "#include <beta>"s, "#include \"aimless.h\""s, "#include <algorithm>"s});

        assert_elements_equal(result, "#include <algorithm>"s, "#include <beta>"s, ""s, "#include \"aimless.h\"", "#include \"alpha.h\"");
    }

    TEST_F(ImportSorterTests, sort_given_lines_containing_whitespace_sorts_based_on_header_name) {
        auto result = _subject.sort({"         #include <beta>"s, "     #include <alpha>"s});

        assert_elements_equal(result, "     #include <alpha>"s, "         #include <beta>"s);
    }

    TEST_F(ImportSorterTests, sort_given_lines_containing_comments_retains_comments) {
        auto result = _subject.sort({"#include <beta> // beta"s, "#include <alpha> // import alpha lib"s});

        assert_elements_equal(result, "#include <alpha> // import alpha lib"s, "#include <beta> // beta"s);
    }

    TEST_F(ImportSorterTests, sort_given_imports_broken_by_comments_sorts_sections_surrounding_comments) {
        auto result = _subject.sort({
            "#include \"beta.h\""s,
            "#include <alpha>"s,
            "// other imports to follow"s,
            "#include <regular>"s,
            "#include <algorithm>"s});

        assert_elements_equal(result,
            "#include <alpha>"s,
            ""s,
            "#include \"beta.h\""s,
            "// other imports to follow"s,
            "#include <algorithm>"s,
            "#include <regular>"s);
    }

    TEST_F(ImportSorterTests, sort_given_import_sections_broken_including_whitespace_preserves_whitespace_between_sections) {
        auto result = _subject.sort({
            "#include \"beta.h\""s,
            "#include <alpha>"s,
            "\n"s,
            "\n"s,
            "// other imports to follow"s,
            "\n"s,
            "\n"s,
            "#include <regular>"s,
            "#include <algorithm>"s});

        assert_elements_equal(result,
            "#include <alpha>"s,
            ""s,
            "#include \"beta.h\""s,
            "\n"s,
            "\n"s,
            "// other imports to follow"s,
            "\n"s,
            "\n"s,
            "#include <algorithm>"s,
            "#include <regular>"s);
    }

    TEST_F(ImportSorterTests, sort_given_mixed_import_groups_puts_single_newline_between_groups) {
        auto result = _subject.sort({
            "#include <vector>"s,
            "#include <regex>"s,
            "#include \"import_sorter.h\""s,
            "#include <tuple>"s,
            "#include <string>"s,
            "#include <iterator>"s,
            "\n"s,
            "namespace isort {"s,
            "}"s});

        assert_elements_equal(result,
            "#include <iterator>"s,
            "#include <regex>"s,
            "#include <string>"s,
            "#include <tuple>"s,
            "#include <vector>"s,
            ""s,
            "#include \"import_sorter.h\""s,
            "\n"s,
            "namespace isort {"s,
            "}"s);
    }
}
