#pragma once

namespace sdk {
template <typename T>
class intrusive_ptr {
public:
    intrusive_ptr() = default;
    intrusive_ptr(T* ptr) : m_ptr(ptr) {
        if (m_ptr != nullptr) {
            m_ptr->add_ref();
        }
    }

    intrusive_ptr(const intrusive_ptr& other) : m_ptr(other.m_ptr) {
        if (m_ptr != nullptr) {
            m_ptr->add_ref();
        }
    }

    intrusive_ptr(intrusive_ptr&& other) noexcept : m_ptr(other.m_ptr) {
        other.m_ptr = nullptr;
    }

    ~intrusive_ptr() {
        if (m_ptr != nullptr) {
            m_ptr->release();
        }
    }

    T* operator->() const {
        return m_ptr;
    }

    intrusive_ptr& operator=(T* ptr) {
        if (m_ptr != nullptr) {
            m_ptr->release();
        }

        m_ptr = ptr;

        if (m_ptr != nullptr) {
            m_ptr->add_ref();
        }

        return *this;
    }

    T* get() const {
        return m_ptr;
    }

    operator T*() const {
        return m_ptr;
    }

    T* operator*() const {
        return m_ptr;
    }

    bool has_value() const {
        return m_ptr != nullptr;
    }

    void reset() {
        if (m_ptr != nullptr) {
            m_ptr->release();
            m_ptr = nullptr;
        }
    }

private:
    T* m_ptr{nullptr};
};
}