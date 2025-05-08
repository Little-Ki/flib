#pragma once
#include <vector>

namespace fl {

    class VMT {
    private:
        void *_base{0};

        uintptr_t *_origin_vmt{nullptr};
        uintptr_t *_shadow_vmt{nullptr};

        std::vector<uintptr_t> _table;

        size_t _method_count{0};

    public:
        VMT() = default;

        VMT(const VMT &other) = delete;

        bool install(void *base);

        bool uninstall();

        bool hook(size_t index, void *function);

        template <typename T = uintptr_t>
        __forceinline T getOrigin(size_t index) {
            return reinterpret_cast<T>(_origin_vmt[index]);
        }

        __forceinline static uintptr_t getMethod(void *p, size_t index) {
            return (*reinterpret_cast<uintptr_t **>(p))[index];
        }

        template <typename T>
        __forceinline static T getMethod(void *p, size_t index) {
            return reinterpret_cast<T>(getMethod(p, index));
        }
    };
}