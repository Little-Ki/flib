#include "lib_scanner.h"

#include <algorithm>

namespace fl::scanner {

    __forceinline char Hex2Char(char c) {
        c = std::tolower(c);
        if (c >= '0' && c <= '9') {
            return c - '0';
        } else if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        }
        return 0;
    }

    __forceinline bool IsX(char c) {
        c = std::tolower(c);
        return (c >= '0' && c <= '9') ||
               (c >= 'a' && c <= 'f');
    }

    Pattern::Pattern(const std::string str) {
        auto c = str.c_str();
        auto end = c + str.size() - 1;
        if (str.empty() || str.size() < 2) {
            return;
        }

        while (c != end) {
            if (*c == '?') {
                _elements.push_back({});
            }

            if (IsX(c[0]) && IsX(c[1])) {
                _elements.push_back({static_cast<uint8_t>(Hex2Char(c[0]) << 4 | Hex2Char(c[1]))});
            }
            c++;
        }
    }

    uintptr_t search(uintptr_t begin, uintptr_t length, const Pattern &pattern, uint32_t offset) {
        if (!begin || !length || pattern.empty()) {
            return 0;
        }
        uint8_t *start = reinterpret_cast<uint8_t *>(begin);
        uint8_t *end = start + length;

        auto it = std::search(
            start,
            end,
            pattern.begin(),
            pattern.end(),
            [](const uint8_t a, const PatternElem &b) {
                return b.equal(a);
            });

        if (it == end) {
            return 0;
        }

        return reinterpret_cast<uintptr_t>(it) + offset;
    }
}