#include "lib_console.h"

#include <Windows.h>

namespace fl::console {
    void console::print(const char *format) {
        std::cout << format;
    }

    bool allocate(const std::string &name) {
        if (!AllocConsole()) {
            return false;
        }

        _iobuf *data;
        const errno_t res = freopen_s(&data, "CONOUT$", "w", stdout);
        if (res != 0) {
            return false;
        }

        if (!SetConsoleTitleA(name.c_str())) {
            return false;
        }

        return true;
    }

    void detach() {
        FreeConsole();
    }

    void setColor(ConsoleColor color) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<uint16_t>(color));
    }
}