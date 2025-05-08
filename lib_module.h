#pragma once

#include "lib_hash.hpp"
#include <stdint.h>

namespace fl::pe {
    class Module {
    private:
        uintptr_t _module_base{0};
        size_t _module_size{0};

    public:
        Module() = default;

        template <typename T>
        Module(T base, size_t size) : _module_base(reinterpret_cast<uintptr_t>(base)), _module_size(size) {}

        bool valid() {
            return _module_base && _module_size;
        }

        uintptr_t base() {
            return _module_base;
        }

        size_t size() {
            return _module_size;
        }

        uintptr_t processPointer(std::string name);
        uintptr_t processPointer(const hash_t hash);

        template <typename T>
        T getProcess(std::string name) {
            return reinterpret_cast<T>(processPointer(name));
        }

        template <typename T>
        T getProcess(const hash_t &hash) {
            return reinterpret_cast<T>(processPointer(hash));
        }
    };
}