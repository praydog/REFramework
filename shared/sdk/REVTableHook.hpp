// REVTableHook is a vtable hook class that is specific to the RE Engine
// The concepts behind vtable hooking remain the same,
// but we need to copy more memory than the usual MSVC style hooking.
// MSVC vtable hooking requires the preceding 8 bytes before the vtable to be copied for RTTI.
// Since REEngine managed objects are not the same as MSVC objects,
// REVTableHook will need to copy a bit more than 8 bytes, because the layout looks something like this
// REManagedObject
//  + 0: Object info*
//       - 0x10: Native vtable, what we want to hook, probably more past -0x10 as well.
//       - 8: Field ptr
//       + 0: sdk::RETypeDefinition*
//       + 8 and onwards: some kind of vtable, not the one we want
// We will be replacing the object info pointer, and copying the preceding bytes (-8 and onwards), as well as +0 and onwards.

#pragma once

#include <vector>
#include <cstdint>

class REManagedObject;

namespace sdk {
class REVTableHook {
public:
    REVTableHook(::REManagedObject* target);
    virtual ~REVTableHook();

    bool is_hooked() const {
        return m_hooked;
    }

    bool hook_method(uint32_t index, void* destination);

    template<typename T>
    T get_original(uint32_t index) {
        if (index >= m_new_vtable.size()) {
            return (T)0;
        }

        return (T)m_original_vtable[index];
    }

private:
    bool hook();
    void unhook();
    uint32_t calculate_vtable_size(void** vtable) const;

    bool m_hooked{false};

    ::REManagedObject* m_object{nullptr};
    void* m_original_object_info{nullptr};
    void** m_original_vtable{nullptr};

    std::vector<void*> m_new_vtable{};
    std::vector<uint8_t> m_new_data{};
    void* m_new_object_info{nullptr};
    uint8_t m_offset_from_object_info_base{0}; // offset that m_original_object_info starts from behind the object info pointer
};
}