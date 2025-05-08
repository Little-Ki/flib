#pragma once
#include "lib_hash.hpp"
#include "lib_module.h"
#include <string>

namespace fl::pe {
    bool findModule(Module &module, std::wstring name);
    bool findModule(Module &module, const hash_t &hash);
}