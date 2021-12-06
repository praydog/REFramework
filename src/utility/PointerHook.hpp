#pragma once

struct ProtectionOverride {
    ProtectionOverride(void* address, size_t size, uint32_t protection);
    virtual ~ProtectionOverride();

    void* m_address{};
    size_t m_size{};
    uint32_t m_old{};
};

// A class to replace a pointer with a new pointer.
class PointerHook {
public:
    PointerHook(void** old_ptr, void* new_ptr);
    virtual ~PointerHook();

    bool remove();
    bool restore();

    template<typename T>
    T get_original() const {
        return (T)m_original;
    }

private:
    void** m_replace_ptr{};
    void* m_original{};
    void* m_destination{};
};