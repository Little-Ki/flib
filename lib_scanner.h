#pragma once
#include <stdint.h>
#include <string>
#include <vector>

#include "lib_module.h"

namespace fl::scanner {
    class PatternElem {
    private:
        uint8_t _byte{0};
        bool _wild{true};

    public:
        PatternElem(uint8_t byte) : _byte(byte), _wild(false) {}

        PatternElem() = default;

        bool equal(const uint8_t other) const {
            return _wild || _byte == other;
        }
    };

    class Pattern {
    private:
        std::vector<PatternElem> _elements;

    public:
        Pattern(const std::string str);

        bool empty() const { return _elements.empty(); }

        auto begin() const { return _elements.cbegin(); }
        auto end() const { return _elements.cend(); }
    };

    uintptr_t search(uintptr_t begin, uintptr_t length, const Pattern &pattern, uint32_t offset);

    template <typename T = uintptr_t>
    T search(pe::Module module, const std::string &pattern, uint32_t offset) {
        return reinterpret_cast<T>(search(module.base(), module.size(), pattern, offset));
    }
}