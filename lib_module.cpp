#include "lib_module.h"
#include "lib_nt.h"

#include <algorithm>

namespace fl::pe {
    uintptr_t Module::processPointer(const hash_t hash) {
        auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(_module_base);
        auto ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(_module_base + dosHeader->e_lfanew);
        auto dataDir = ntHeader->OptionalHeader.DataDirectory;

        if (!dataDir[IMAGE_DIRECTORY_ENTRY_EXPORT].Size) {
            return 0;
        }

        PIMAGE_EXPORT_DIRECTORY exportDir = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(
            _module_base + dataDir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

        DWORD nameCount = exportDir->NumberOfNames;

        UINT_PTR *addressTable = reinterpret_cast<UINT_PTR *>(_module_base + exportDir->AddressOfFunctions);
        DWORD *nameRvaTable = reinterpret_cast<DWORD *>(_module_base + exportDir->AddressOfNames);
        WORD *indexTable = reinterpret_cast<WORD *>(_module_base + exportDir->AddressOfNameOrdinals);

        for (DWORD i = 0; i < nameCount; i++) {
            std::string name{reinterpret_cast<char *>(_module_base + nameRvaTable[i])};
            if (hash::fnv1a(name) == hash) {
                return _module_base + addressTable[indexTable[i]];
            }
        }

        return uintptr_t();
    }

    uintptr_t Module::processPointer(std::string name) {
        std::transform(name.begin(), name.end(), name.begin(), std::tolower);
        return processPointer(hash::fnv1a(name));
    }
}