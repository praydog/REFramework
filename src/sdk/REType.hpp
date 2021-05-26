#pragma once

class REType;
class REVariableList;
class VariableDescriptor;
class FunctionDescriptor;

// Flags
namespace via::dti {
enum class decl : uint16_t {
    None = 0,
    Abstract = (1 << 0),
    Concrete = (1 << 1),
    Script = (1 << 5),
    Function = (1 << 6),
    Singleton = (1 << 7),
    Interface = (1 << 8),
    Root = 16384,
};
}

namespace utility::re_type {
uint32_t get_vm_type(::REType* t);
uint32_t get_value_type_size(::REType* t);
bool is_singleton(::REType* t);
void* get_singleton_instance(::REType* t);
REVariableList* get_variables(::REType* t);
VariableDescriptor* get_field_desc(::REType* t, std::string_view field);
FunctionDescriptor* get_method_desc(::REType* t, std::string_view name);
} // namespace utility::re_type