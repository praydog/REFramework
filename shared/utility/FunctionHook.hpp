#pragma once

#include <windows.h>
#include <cstdint>

#include <utility/Address.hpp>

#include <safetyhook.hpp>

class FunctionHook {
public:
    FunctionHook() = delete;
    FunctionHook(const FunctionHook& other) = delete;
    FunctionHook(FunctionHook&& other) = delete;
    FunctionHook(Address target, Address destination);
    virtual ~FunctionHook();

    bool create();

    auto get_original() const {
        return m_inline_hook->trampoline().address();
    }

    template <typename T>
    T* get_original() const {
        return m_inline_hook->original<T*>();
    }

    auto is_valid() const {
        return m_inline_hook && m_inline_hook->operator bool();
    }

    FunctionHook& operator=(const FunctionHook& other) = delete;
    FunctionHook& operator=(FunctionHook&& other) = delete;

private:
    std::expected<SafetyHookInline, SafetyHookInline::Error> m_inline_hook;

    uintptr_t m_target{ 0 };
    uintptr_t m_destination{ 0 };
};