#include <optional>
#include <vector>
#include <array>

#include <Windows.h>

#include "String.hpp"
#include "Memory.hpp"

using namespace std;

namespace utility {
    thread_local vector<MEMORY_BASIC_INFORMATION> g_pages{};

    optional<MEMORY_BASIC_INFORMATION> findInCache(uintptr_t ptr, size_t len) {
        for (const auto& mbi : g_pages) {
            auto start = (uintptr_t)mbi.BaseAddress;
            auto end = start + mbi.RegionSize;

            if (start <= ptr && ptr + len < end) {
                return mbi;
            }
        }
        
        return {};
    }

    bool memoryHasAccess(const MEMORY_BASIC_INFORMATION& mbi, DWORD protect) {
        // Pages are commited, not guarded or no access, and same protect.
        return (mbi.State & MEM_COMMIT && 
                !(mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) && 
                mbi.Protect & protect);
    }

    bool isGoodPtr(uintptr_t ptr, size_t len, uint32_t access) {
        // See if the address is in our cache.
        auto cachedMBI = findInCache(ptr, len);

        if (cachedMBI) {
            return memoryHasAccess(*cachedMBI, access);
        }
        
        MEMORY_BASIC_INFORMATION mbi{};

        if (VirtualQuery((LPCVOID)ptr, &mbi, sizeof(mbi)) == 0) {
            return false;
        }

        // Add it to our cache if its not there.
        if (!findInCache((uintptr_t)mbi.BaseAddress, 0)) {
            g_pages.push_back(mbi);
        }

        return memoryHasAccess(mbi, access);
    }

    bool isGoodReadPtr(uintptr_t ptr, size_t len) {
        return isGoodPtr(ptr, len, PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
    }

    bool isGoodWritePtr(uintptr_t ptr, size_t len) {
        return isGoodPtr(ptr, len, PAGE_READWRITE | PAGE_EXECUTE_READWRITE);
    }

    bool isGoodCodePtr(uintptr_t ptr, size_t len) {
        return isGoodPtr(ptr, len, PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE);
    }

    bool is_stub_code(uint8_t* code) {
        if (code == nullptr) {
            return false;
        }

        static const std::vector<std::vector<uint8_t>> stub_codes{
            { 0x48, 0x31, 0xc0, 0xc3 }, // xor rax, rax; ret
            { 0x33, 0xc0, 0xc3 }, // xor eax, eax; ret
            { 0x30, 0xc0, 0xc3 }, // xor al, al; ret
            { 0x31, 0xc0, 0xc3 }, // xor al, al; ret
            { 0x32, 0xc0, 0xc3 }, // xor al, al; ret
            { 0xb0, 0x01, 0xc3 }, // mov al, 1; ret
            { 0xb0, 0x00, 0xc3 }, // mov al, 0; ret
            { 0xc2, 0x00, 0x00 }, // ret 0
            { 0xc3 } // ret
        };

        // check whether the code matches any of the stub codes
        for (const auto& stub_code : stub_codes) {
            if (memcmp(code, stub_code.data(), stub_code.size()) == 0) {
                return true;
            }
        }

        return false;
    }
}