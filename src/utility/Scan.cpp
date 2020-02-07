#include "Pattern.hpp"
#include "String.hpp"
#include "Module.hpp"
#include "Scan.hpp"

using namespace std;

namespace utility {
    optional<uintptr_t> scan(const string& module, const string& pattern) {
        return scan(GetModuleHandle(module.c_str()), pattern);
    }

    optional<uintptr_t> scan(const string& module, uintptr_t start, const string& pattern) {
        HMODULE mod = GetModuleHandle(module.c_str());
        return scan(start, (get_module_size(mod).value_or(0) - start + (uintptr_t)mod), pattern);
    }

    optional<uintptr_t> scan(HMODULE module, const string& pattern) {
        return scan((uintptr_t)module, get_module_size(module).value_or(0), pattern);
    }

    optional<uintptr_t> scan(uintptr_t start, size_t length, const string& pattern) {
        if (start == 0 || length == 0) {
            return {};
        }

        Pattern p{ pattern };

        return p.find(start, length);
    }

    uintptr_t calculate_absolute(uintptr_t address, uint8_t customOffset /*= 4*/) {
        auto offset = *(int32_t*)address;

        return address + customOffset + offset;
    }
}
