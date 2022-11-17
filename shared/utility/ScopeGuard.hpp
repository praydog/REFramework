#pragma once

#include <functional>

class ScopeGuard {
public:
    ScopeGuard() = delete;
    ScopeGuard(std::function<void()> on_exit)
        : m_on_exit{on_exit}
    {
    }

    ~ScopeGuard() {
        if (m_on_exit) {
            m_on_exit();
        }
    }

private:
    std::function<void()> m_on_exit;
};