#include "lib_vmt.h"

#include <Windows.h>

namespace fl {

    bool validate_ptr(uintptr_t address) {
        if (!address)
            return false;

        MEMORY_BASIC_INFORMATION mbi;

        if (!VirtualQuery(reinterpret_cast<const void *>(address), &mbi, sizeof mbi))
            return false;

        if (mbi.Protect & PAGE_EXECUTE_READWRITE ||
            mbi.Protect & PAGE_EXECUTE_READ) {
            return true;
        }

        return false;
    }

    __forceinline size_t count_methods(uintptr_t *vmt) {
        size_t i = 0;
        while (validate_ptr(vmt[i]))
            ++i;
        return i;
    }

    bool VMT::install(void *base) {
        if (!base) {
            return false;
        }

        _base = base;
        _origin_vmt = *reinterpret_cast<uintptr_t **>(base);
        _method_count = count_methods(_origin_vmt);

        if (!_method_count) {
            return false;
        }

        _table.resize(_method_count + 1L);
        if (!_table.size()) {
            return false;
        }

        auto table_base = reinterpret_cast<uintptr_t>(_table.data());

        std::memcpy(
            reinterpret_cast<void *>(_table.data()),
            reinterpret_cast<void *>(_origin_vmt - 1L),
            (_method_count + 1L) * sizeof(uintptr_t));

        _shadow_vmt = reinterpret_cast<uintptr_t *>(table_base + sizeof(uintptr_t));

        *reinterpret_cast<UINT_PTR *>(base) = reinterpret_cast<uintptr_t>(_shadow_vmt);

        return true;
    }

    bool VMT::uninstall() {
        if (!_origin_vmt || !_base) {
            return false;
        }

        *reinterpret_cast<uintptr_t *>(_base) = reinterpret_cast<uintptr_t>(_origin_vmt);

        _table.clear();

        _origin_vmt = nullptr;
        _base = nullptr;

        return true;
    }

    bool VMT::hook(size_t index, void *function) {
        if (!_origin_vmt || !_shadow_vmt || index > _method_count || !function)
            return false;

        _shadow_vmt[index] = reinterpret_cast<uintptr_t>(function);

        return true;
    }

}