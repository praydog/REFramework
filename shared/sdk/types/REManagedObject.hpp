#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "REObject.hpp"

// Forward declarations for member method signatures
class REType;
class REVariableList;
class VariableDescriptor;
class FunctionDescriptor;
namespace sdk { struct RETypeDefinition; }
namespace via::clr { enum class VMObjType : int64_t; }

#pragma pack(push, 1)
class REManagedObject : public REObject {
public:
    uint32_t get_ref_count() const { return m_ref_count; }
    void set_ref_count(uint32_t v) { m_ref_count = v; }
    uint32_t* ref_count_ptr() { return &m_ref_count; }

    // Runtime size of REManagedObject. Use instead of sizeof() for pointer
    // arithmetic — RE7 TDB49 has 0x20 (enlarged base), all others 0x10.
    static size_t runtime_size();  // implemented in REManagedObject.cpp

    // Type introspection
    REType* get_type() const;
    sdk::RETypeDefinition* get_type_definition() const;
    REType* safe_get_type() const;
    std::string get_type_name() const;
    via::clr::VMObjType get_vm_type() const;

    // Type checking
    bool is_a(std::string_view name) const;
    bool is_a(REType* cmp) const;
    static bool is_managed_object(void* address);

    // Ref counting
    void add_ref();
    void release();
    static void resolve_add_ref();
    static void resolve_release();

    // Size
    uint32_t get_size() const;

    // Field/method access
    static REVariableList* get_variables(::REType* t);
    REVariableList* get_variables() const;
    VariableDescriptor* get_field_desc(std::string_view field) const;
    FunctionDescriptor* get_method_desc(std::string_view name) const;

    template<typename T = void*> T* get_field_ptr() const;
    template<typename T> T get_reflection_property(VariableDescriptor* desc) const;
    template<typename T> T get_reflection_property(std::string_view field) const;

    // Serialization
    static std::vector<::REManagedObject*> deserialize(const uint8_t* data, size_t size, bool add_references);
    void deserialize_native(const uint8_t* data, size_t size, const std::vector<::REManagedObject*>& objects);

    // ParamWrapper + call_method declared in REManagedObject.hpp (needs ReClass.hpp)
    struct ParamWrapper;
    template<typename Arg> std::unique_ptr<ParamWrapper> call_method(FunctionDescriptor* desc, const Arg& arg);
    template<typename Arg> std::unique_ptr<ParamWrapper> call_method(std::string_view name, const Arg& arg);

private:
    uint32_t    m_ref_count;    // 0x08
    int16_t     _unk_000C;      // 0x0C
    char        _pad_000E[2];   // 0x0E
};
static_assert(sizeof(REManagedObject) == 0x10);
#pragma pack(pop)
