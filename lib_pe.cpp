#include "lib_pe.h"
#include "lib_nt.h"

#include <algorithm>

using namespace fl::hash;

__forceinline PEB *GetPEB() {
#ifdef _WIN64
    auto teb = reinterpret_cast<TEB *>(__readgsqword(0x30));
#else
    auto teb - reinterpret_cast<TEB *>(__readfsdword(0x18));
#endif
    return teb ? teb->Peb : nullptr;
}

namespace fl::pe {
    bool findModule(Module &module, const hash_t &hash) {
        auto peb = GetPEB();

        if (!peb) {
            return false;
        }

        auto ldrData = peb->LoaderData;
        auto modList = &ldrData->InMemoryOrderModuleList;
        auto node = modList->Flink;

        while (node != modList) {
            auto entry = reinterpret_cast<LDR_DATA_TABLE_ENTRY *>
                CONTAINING_RECORD(
                    node,
                    LDR_DATA_TABLE_ENTRY,
                    InMemoryOrderLinks);

            if (!entry)
                continue;

            std::wstring moduleName(entry->BaseDllName.szBuffer);

            std::transform(
                moduleName.begin(), moduleName.end(),
                moduleName.begin(), std::tolower);

            if (hash::fnv1a(moduleName) == hash) {
                module = Module(entry->DllBase, entry->SizeOfImage);
                return true;
            }
        }

        return false;
    }

    bool findModule(Module &module, std::wstring name) {
        std::transform(name.begin(), name.end(), name.begin(), std::tolower);
        return findModule(module, hash::fnv1a(name));
    }

}