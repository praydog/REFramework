#pragma once

#include <vector>
#include <utility/ScopeGuard.hpp>

// Safe callback vector that handles additions during iteration
// Prevents vector reallocation crashes by queuing new callbacks added during iteration
// and merging them when iteration completes.
template<typename T>
class SafeCallbackVector {
private:
    std::vector<T> m_callbacks;
    std::vector<T> m_pending;
    int m_use_count = 0;

public:
    // Create a scope guard that increments use count and merges pending when released
    auto acquire_iteration() {
        m_use_count++;
        return utility::ScopeGuard([this]() {
            m_use_count--;
            if (m_use_count == 0 && !m_pending.empty()) {
                m_callbacks.insert(m_callbacks.end(), m_pending.begin(), m_pending.end());
                m_pending.clear();
            }
        });
    }

    // Get the callback vector for iteration
    std::vector<T>& get() {
        return m_callbacks;
    }

    // Add a callback (queues if currently iterating)
    void add(const T& callback) {
        if (m_use_count > 0) {
            m_pending.push_back(callback);
        } else {
            m_callbacks.push_back(callback);
        }
    }

    // Clear all callbacks
    void clear() {
        m_callbacks.clear();
        m_pending.clear();
    }

    // Check if empty
    bool empty() const {
        return m_callbacks.empty() && m_pending.empty();
    }
};
