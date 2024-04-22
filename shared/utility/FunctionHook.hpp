#pragma once

#include <windows.h>
#include <cstdint>
#include <shared_mutex>

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
        if (m_init_finished) {
            return m_inline_hook->trampoline().address();
        }

        std::shared_lock _{ m_initialization_mutex };
        m_init_finished = true;
        return m_inline_hook->trampoline().address();
    }

    template <typename T>
    T* get_original() const {
        if (m_init_finished) {
            return m_inline_hook->original<T*>();
        }

        std::shared_lock _{ m_initialization_mutex };
        m_init_finished = true;
        return m_inline_hook->original<T*>();
    }

    auto is_valid() const {
        if (m_init_finished) {
            return is_valid_unsafe();
        }

        std::shared_lock _{ m_initialization_mutex };
        m_init_finished = true;
        return is_valid_unsafe();
    }

    FunctionHook& operator=(const FunctionHook& other) = delete;
    FunctionHook& operator=(FunctionHook&& other) = delete;

private:
    bool is_valid_unsafe() const {
        return m_inline_hook && m_inline_hook->operator bool();
    }

    std::expected<SafetyHookInline, SafetyHookInline::Error> m_inline_hook;
    mutable std::shared_mutex m_initialization_mutex{};
    mutable bool m_init_finished{ false };

    uintptr_t m_target{ 0 };
    uintptr_t m_destination{ 0 };
};