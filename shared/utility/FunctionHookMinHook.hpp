#pragma once

#include <windows.h>
#include <cstdint>

#include <utility/Address.hpp>

class FunctionHookMinHook {
public:
    FunctionHookMinHook() = delete;
    FunctionHookMinHook(const FunctionHookMinHook& other) = delete;
    FunctionHookMinHook(FunctionHookMinHook&& other) = delete;
    FunctionHookMinHook(Address target, Address destination);
    virtual ~FunctionHookMinHook();

    bool create();

    // Called automatically by the destructor, but you can call it explicitly
    // if you need to remove the hook.
    bool remove();

    auto get_original() const {
        return m_original;
    }

    template <typename T>
    T* get_original() const {
        return (T*)m_original;
    }

    auto is_valid() const {
        return m_original != 0;
    }

    FunctionHookMinHook& operator=(const FunctionHookMinHook& other) = delete;
    FunctionHookMinHook& operator=(FunctionHookMinHook&& other) = delete;

private:
    uintptr_t m_target{ 0 };
    uintptr_t m_destination{ 0 };
    uintptr_t m_original{ 0 };
};