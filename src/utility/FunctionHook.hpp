#pragma once

#include <windows.h>
#include <cstdint>

#include "Address.hpp"

class FunctionHook {
public:
    FunctionHook() = delete;
    FunctionHook(const FunctionHook& other) = delete;
    FunctionHook(FunctionHook&& other) = delete;
    FunctionHook(Address target, Address destination);
    virtual ~FunctionHook();

    bool create();

    // Called automatically by the destructor, but you can call it explicitly
    // if you need to remove the hook.
    bool remove();

    auto getOriginal() const {
        return m_original;
    }

    template <typename T>
    T* getOriginal() const {
        return (T*)m_original;
    }

    auto isValid() const {
        return m_original != 0;
    }

    FunctionHook& operator=(const FunctionHook& other) = delete;
    FunctionHook& operator=(FunctionHook&& other) = delete;

private:
    uintptr_t m_target{ 0 };
    uintptr_t m_destination{ 0 };
    uintptr_t m_original{ 0 };
};