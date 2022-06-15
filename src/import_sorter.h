#pragma once

#include <string>
#include <vector>

namespace isort {
    class ImportSorter {
        public:
            std::vector<std::string> sort(const std::vector<std::string>& lines) const;
    };
}
