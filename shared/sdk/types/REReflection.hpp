#pragma once
#include <cstdint>

class REVariableList;
class MethodParamInfo;
class StaticVariableDescriptor;

#pragma pack(push, 1)

// Holds a function descriptor + padding.
class FunctionHolder {
public:
    class FunctionDescriptor* descriptor;   // 0x00
    char _pad_0008[24];                     // 0x08
};
static_assert(sizeof(FunctionHolder) == 0x20);

// Linked list of type fields (methods + variables + deserializer).
class REFieldList {
public:
    REFieldList* get_next() const { return m_next; }
    auto get_methods() const { return m_methods; }
    int32_t get_num() const { return m_num; }
    int32_t get_maxItems() const { return m_maxItems; }
    REVariableList* get_variables() const { return m_variables; }
    void* get_deserializer() const { return m_deserializer; }

private:
    uint32_t        _unk_0000;              // 0x00
    char            _pad_0004[4];           // 0x04
    REFieldList*    m_next;                 // 0x08
    FunctionHolder**(*m_methods)[4000];     // 0x10
    int32_t         m_num;                  // 0x18
    int32_t         m_maxItems;             // 0x1C
    REVariableList* m_variables;            // 0x20
    void*           m_deserializer;         // 0x28
    uint32_t        _unk_0030;              // 0x30
};
static_assert(sizeof(REFieldList) == 0x34);

// Native function descriptor (reflection method).
class FunctionDescriptor {
public:
    const char* get_name() const { return m_name; }
    void* get_functionPtr() const { return m_function_ptr; }
    int32_t get_numParams() const { return m_num_params; }
    auto get_params() const { return m_params; }
    uint32_t get_typeIndex() const { return m_type_index; }
    const char* get_returnTypeName() const { return m_return_type_name; }

private:
    char*                       m_name;             // 0x00
    MethodParamInfo(*m_params)[256];                // 0x08
    char                        _pad_0010[4];       // 0x10
    int32_t                     m_num_params;       // 0x14
    void*                       m_function_ptr;     // 0x18
    uint32_t                    m_return_type_flag; // 0x20 -- AND 0x1F = via::reflection::TypeKind
    uint32_t                    m_type_index;       // 0x24
    char*                       m_return_type_name; // 0x28
    char                        _pad_0030[8];       // 0x30
};
static_assert(sizeof(FunctionDescriptor) == 0x38);

// Native variable descriptor (reflection property).
class VariableDescriptor {
public:
    const char* get_name() const { return m_name; }
    void* get_function() const { return m_function; }
    int32_t get_flags() const { return m_flags; }
    uint32_t get_typeFqn() const { return m_type_fqn; }
    const char* get_typeName() const { return m_type_name; }
    uint32_t get_variableType() const { return m_variable_type; }
    StaticVariableDescriptor* get_staticVariableData() const { return m_static_data; }
    int32_t get_attributes() const { return m_attributes; }

    static constexpr uintptr_t offset_of_flags()      { return 0x18; }
    static constexpr uintptr_t offset_of_attributes() { return 0x3C; }

private:
    char*                       m_name;             // 0x00
    uint32_t                    m_name_hash;        // 0x08
    uint16_t                    m_flags1;           // 0x0C
    uint16_t                    _unk_000E;          // 0x0E
    void*                       m_function;         // 0x10
    int32_t                     m_flags;            // 0x18 -- (flags & 0x1F) = via::clr::reflection::TypeKind
    uint32_t                    m_type_fqn;         // 0x1C
    char*                       m_type_name;        // 0x20
    int32_t                     m_getter;           // 0x28
    union {                                         // 0x2C
        uint32_t                m_variable_type;    // 1 == pointer? 3 == builtin?
        uint32_t                m_destructor;
    };
    StaticVariableDescriptor*   m_static_data;      // 0x30
    int32_t                     m_setter;           // 0x38
    int32_t                     m_attributes;       // 0x3C
    char                        _pad_0040[8];       // 0x40
};
static_assert(sizeof(VariableDescriptor) == 0x48);

#pragma pack(pop)
