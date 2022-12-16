#include <spdlog/spdlog.h>

#include "REManagedObject.hpp"

#include "REVTableHook.hpp"

namespace sdk {
REVTableHook::REVTableHook(::REManagedObject* target)
    : m_object{target}
{
    hook();
};

REVTableHook::~REVTableHook() {
    unhook();
}

void REVTableHook::unhook() {
    if (!m_hooked) {
        return;
    }

    spdlog::info("[REVTableHook] Attempting to unhook vtable for {:x}...", (uintptr_t)m_object);

    try {
        if (*(void**)m_object == m_new_object_info) {
            *(void**)m_object = m_original_object_info;
        } else {
            spdlog::info("[REVTableHook] Object {:x}'s memory was not what we expected, not unhooking...", (uintptr_t)m_object);
        }
    } catch(...) {
        spdlog::error("[REVTableHook] Could not unhook vtable, object may have been deallocated");
    }

    m_hooked = false;
}

uint32_t REVTableHook::calculate_vtable_size(void** vtable) const {
    if (vtable == nullptr) {
        return 0;
    }

    uint32_t size{0};

    for (auto i = 0; i < 512; ++i) {
        if (IsBadReadPtr(vtable + i, sizeof(void*))) {
            return size;
        }

        ++size;
    }

    return size;
}

bool REVTableHook::hook() {
    spdlog::info("[REVTableHook] Attempting to hook {:x}", (uintptr_t)m_object);

    if (m_object == nullptr || !utility::re_managed_object::is_managed_object(m_object)) {
        spdlog::error("[REVTableHook] Cannot hook invalid object {:x}", (uintptr_t)m_object);
        return false;
    }

    m_original_object_info = *(void**)m_object;

    if (m_original_object_info == nullptr) {
        return false;
    }
    
    m_original_vtable = *(void***)((uintptr_t)m_original_object_info - 0x10);

    if (m_original_vtable == nullptr) {
        spdlog::error("[REVtableHook] {:x} has no vtable, cannot hook.", (uintptr_t)m_object);
        return false;
    }

    spdlog::info("[REVTableHook] {:x} has vtable {:x}", (uintptr_t)m_object, (uintptr_t)m_original_vtable);
    spdlog::info("[REVTableHook] Calculating backwards size...");

    for (uint32_t i = 0; i < 0x1000; i += sizeof(void*)) {
        const auto ptr = (void**)((uintptr_t)m_original_object_info - i);

        if (IsBadReadPtr(ptr, sizeof(void*))) {
            break;
        }

        m_offset_from_object_info_base = i;
    }

    spdlog::info("[REVTableHook] Offset from object info base: {:x}", m_offset_from_object_info_base);
    spdlog::info("[REVTableHook] Calculating forwards size...");

    // Do the same thing but forward now to determine the rest of the size.
    uint32_t forward_size = 0;

    for (uint32_t i = 0; i < 0x1000; i += sizeof(void*)) {
        const auto ptr = (void**)((uintptr_t)m_original_object_info + i);

        if (IsBadReadPtr(ptr, sizeof(void*))) {
            break;
        }

        forward_size = i + sizeof(void*);
    }

    spdlog::info("[REVTableHook] Forward size: {:x}, {:x} total", forward_size, m_offset_from_object_info_base + forward_size);

    // Copy over the original object info.
    spdlog::info("[REVTableHook] Copying original object info...");
    m_new_data.resize(m_offset_from_object_info_base + forward_size);
    memcpy(m_new_data.data(), (uint8_t*)((uintptr_t)m_original_object_info - m_offset_from_object_info_base), m_new_data.size());

    // Calculate vtable size.
    spdlog::info("[REVTableHook] Calculating vtable size...");
    m_new_vtable.resize(calculate_vtable_size(m_original_vtable));

    // Copy over the original vtable.
    spdlog::info("[REVTableHook] Copying original vtable of size {}...", m_new_vtable.size());
    std::copy(m_original_vtable, m_original_vtable + m_new_vtable.size(), m_new_vtable.begin());

    // Replace the vtable pointer in the new data with ours.
    spdlog::info("[REVTableHook] Replacing vtable pointer in new data...");
    *(void***)(((uintptr_t)m_new_data.data() + m_offset_from_object_info_base) - 0x10) = m_new_vtable.data();

    // Now replace the pointer.
    spdlog::info("[REVTableHook] Replacing vtable pointer in original object info...");
    m_new_object_info = &m_new_data[m_offset_from_object_info_base];
    *(void**)m_object = m_new_object_info;

    spdlog::info("[REVTableHook] Hooked {:x}", (uintptr_t)m_object);

    m_hooked = true;
    return true;
}

bool REVTableHook::hook_method(uint32_t index, void* destination) {
    if (!m_hooked) {
        spdlog::error("[REVTableHook] Hook not initialized, cannot hook method.");
        return false;
    }

    if (index >= m_new_vtable.size()) {
        spdlog::error("[REVTableHook] Index {} is >= than maximum of {}", index, m_new_vtable.size());
        return false;
    }

    if (destination == nullptr) {
        spdlog::error("[REVTableHook] Cannot hook method to nullptr.");
        return false;
    }

    spdlog::info("[REVTableHook] Hooking method {} (currently {:x}) to {:x}", index, (uintptr_t)m_new_vtable[index], (uintptr_t)destination);

    m_new_vtable[index] = destination;
    return true;
}
}