#pragma once

extern "C" {
    #include "API.h"
}

#include <mutex>
#include <array>
#include <vector>
#include <cassert>
#include <string_view>
#include <cstdint>
#include <memory>
#include <stdexcept>

namespace reframework {
#pragma pack(push, 1)
struct InvokeRet {
    union {
        std::array<uint8_t, 128> bytes{};
        uint8_t byte;
        uint16_t word;
        uint32_t dword;
        float f;
        uint64_t qword;
        double d;
        void* ptr;
    };

    bool exception_thrown{false};
};
#pragma pack(pop)

class API {
private:
    static inline std::unique_ptr<API> s_instance{};

public:
    struct TDB;
    struct TypeDefinition;
    struct Method;
    struct Field;
    struct Property;
    struct ManagedObject;
    struct ResourceManager;
    struct Resource;
    struct TypeInfo;
    struct VMContext;
    struct ReflectionProperty;
    struct ReflectionMethod;

    struct LuaLock {
        LuaLock() {
            API::s_instance->lock_lua();
        }

        virtual ~LuaLock() {
            API::s_instance->unlock_lua();
        }
    };

public:
    // ALWAYS call initialize first in reframework_plugin_initialize
    static auto& initialize(const REFrameworkPluginInitializeParam* param) {
        if (param == nullptr) {
            throw std::runtime_error("param is null");
        }

        if (s_instance != nullptr) {
            throw std::runtime_error("API already initialized");
        }

        s_instance = std::make_unique<API>(param);
        return s_instance;
    }

    // only call this AFTER calling initialize
    static auto& get() {
        if (s_instance == nullptr) {
            throw std::runtime_error("API not initialized");
        }

        return s_instance;
    }

public:
    API(const REFrameworkPluginInitializeParam* param) 
        : m_param{param},
        m_sdk{param->sdk}
    {
    }

    virtual ~API() {

    }

    inline const auto param() const {
        return m_param;
    }

    inline const auto sdk() const {
        return m_sdk;
    }

    inline const auto tdb() const { 
        return (TDB*)sdk()->functions->get_tdb(); 
    }

    inline const auto resource_manager() const {
        return (ResourceManager*)sdk()->functions->get_resource_manager();
    }

    inline const auto reframework() const {
        return (REFramework*)param()->functions;
    }

    void lock_lua() {
        m_lua_mtx.lock();
        m_param->functions->lock_lua();
    }

    void unlock_lua() {
        m_param->functions->unlock_lua();
        m_lua_mtx.unlock();
    }

    template <typename... Args> void log_error(const char* format, Args... args) { m_param->functions->log_error(format, args...); }
    template <typename... Args> void log_warn(const char* format, Args... args) { m_param->functions->log_warn(format, args...); }
    template <typename... Args> void log_info(const char* format, Args... args) { m_param->functions->log_info(format, args...); }

    API::VMContext* get_vm_context() const {
        return (API::VMContext*)sdk()->functions->get_vm_context();
    }

    API::ManagedObject* typeof(const char* name) const {
        return (API::ManagedObject*)sdk()->functions->typeof_(name);
    }

    API::ManagedObject* get_managed_singleton(std::string_view name) const {
        return (API::ManagedObject*)sdk()->functions->get_managed_singleton(name.data());
    }

    void* get_native_singleton(std::string_view name) const {
        return sdk()->functions->get_native_singleton(name.data());
    }

    std::vector<REFrameworkManagedSingleton> get_managed_singletons() const {
        std::vector<REFrameworkManagedSingleton> out{};
        out.resize(512);

        uint32_t count{};

        auto result = sdk()->functions->get_managed_singletons(&out[0], out.size() * sizeof(REFrameworkManagedSingleton), &count);

#ifdef REFRAMEWORK_API_EXCEPTIONS
        if (result != REFRAMEWORK_ERROR_NONE) {
            throw std::runtime_error("get_managed_singletons failed");
        }
#else
        if (result != REFRAMEWORK_ERROR_NONE) {
            return {};
        }
#endif

        out.resize(count);
        return out;
    }

    std::vector<REFrameworkNativeSingleton> get_native_singletons() const {
        std::vector<REFrameworkNativeSingleton> out{};
        out.resize(512);

        uint32_t count{};

        auto result = sdk()->functions->get_native_singletons(&out[0], out.size() * sizeof(REFrameworkNativeSingleton), &count);

#ifdef REFRAMEWORK_API_EXCEPTIONS
        if (result != REFRAMEWORK_ERROR_NONE) {
            throw std::runtime_error("get_native_singletons failed");
        }
#else
        if (result != REFRAMEWORK_ERROR_NONE) {
            return {};
        }
#endif

        out.resize(count);
        return out;
    }

public:
    struct TDB {
        operator ::REFrameworkTDBHandle() const {
            return (::REFrameworkTDBHandle)this;
        }

        uint32_t get_num_types() const {
            return API::s_instance->sdk()->tdb->get_num_types(*this);
        }

        uint32_t get_num_methods() const {
            return API::s_instance->sdk()->tdb->get_num_methods(*this);
        }

        uint32_t get_num_fields() const {
            return API::s_instance->sdk()->tdb->get_num_fields(*this);
        }

        uint32_t get_num_properties() const {
            return API::s_instance->sdk()->tdb->get_num_properties(*this);
        }

        uint32_t get_strings_size() const {
            return API::s_instance->sdk()->tdb->get_strings_size(*this);
        }

        uint32_t get_raw_data_size() const {
            return API::s_instance->sdk()->tdb->get_raw_data_size(*this);
        }

        const char* get_string_database() const {
            return API::s_instance->sdk()->tdb->get_string_database(*this);
        }

        uint8_t* get_raw_database() const {
            return (uint8_t*)API::s_instance->sdk()->tdb->get_raw_database(*this);
        }

        API::TypeDefinition* get_type(uint32_t index) const {
            return (API::TypeDefinition*)API::s_instance->sdk()->tdb->get_type(*this, index);
        }

        API::TypeDefinition* find_type(std::string_view name) const {
            return (API::TypeDefinition*)API::s_instance->sdk()->tdb->find_type(*this, name.data());
        }

        API::TypeDefinition* find_type_by_fqn(uint32_t fqn) const {
            return (API::TypeDefinition*)API::s_instance->sdk()->tdb->find_type_by_fqn(*this, fqn);
        }

        API::Method* get_method(uint32_t index) const {
            return (API::Method*)API::s_instance->sdk()->tdb->get_method(*this, index);
        }

        API::Method* find_method(std::string_view type_name, std::string_view name) const {
            return (API::Method*)API::s_instance->sdk()->tdb->find_method(*this, type_name.data(), name.data());
        }

        API::Field* get_field(uint32_t index) const {
            return (API::Field*)API::s_instance->sdk()->tdb->get_field(*this, index);
        }

        API::Field* find_field(std::string_view type_name, std::string_view name) const {
            return (API::Field*)API::s_instance->sdk()->tdb->find_field(*this, type_name.data(), name.data());
        }

        API::Property* get_property(uint32_t index) const {
            return (API::Property*)API::s_instance->sdk()->tdb->get_property(*this, index);
        }
    };

    struct REFramework {
        operator ::REFrameworkHandle() {
            return (::REFrameworkHandle)this;
        }

        bool is_drawing_ui() const {
            return API::s_instance->param()->functions->is_drawing_ui();
        }
    };

    struct TypeDefinition {
        operator ::REFrameworkTypeDefinitionHandle() const {
            return (::REFrameworkTypeDefinitionHandle)this;
        }

        uint32_t get_index() const {
            return API::s_instance->sdk()->type_definition->get_index(*this);
        }

        uint32_t get_size() const {
            return API::s_instance->sdk()->type_definition->get_size(*this);
        }

        uint32_t get_valuetype_size() const {
            return API::s_instance->sdk()->type_definition->get_valuetype_size(*this);
        }

        uint32_t get_fqn() const {
            return API::s_instance->sdk()->type_definition->get_fqn(*this);
        }

        const char* get_name() const {
            return API::s_instance->sdk()->type_definition->get_name(*this);
        }

        const char* get_namespace() const {
            return API::s_instance->sdk()->type_definition->get_namespace(*this);
        }

        std::string get_full_name() const {
            std::string buffer{};
            buffer.resize(512);

            uint32_t real_size{0};

            const auto sdk = API::s_instance->sdk();
            auto result = sdk->type_definition->get_full_name(*this, &buffer[0], buffer.size(), &real_size);

            if (result != REFRAMEWORK_ERROR_NONE) {
                return "";
            }

            buffer.resize(real_size);
            return buffer;
        }

        bool has_fieldptr_offset() const {
            return API::s_instance->sdk()->type_definition->has_fieldptr_offset(*this);
        }

        int32_t get_fieldptr_offset() const {
            return API::s_instance->sdk()->type_definition->get_fieldptr_offset(*this);
        }

        uint32_t get_num_methods() const {
            return API::s_instance->sdk()->type_definition->get_num_methods(*this);
        }

        uint32_t get_num_fields() const {
            return API::s_instance->sdk()->type_definition->get_num_fields(*this);
        }

        uint32_t get_num_properties() const {
            return API::s_instance->sdk()->type_definition->get_num_properties(*this);
        }

        bool is_derived_from(API::TypeDefinition* other) {
            return API::s_instance->sdk()->type_definition->is_derived_from(*this, *other);
        }

        bool is_derived_from(std::string_view other) {
            return API::s_instance->sdk()->type_definition->is_derived_from_by_name(*this, other.data());
        }

        bool is_valuetype() const {
            return API::s_instance->sdk()->type_definition->is_valuetype(*this);
        }

        bool is_enum() const {
            return API::s_instance->sdk()->type_definition->is_enum(*this);
        }

        bool is_by_ref() const {
            return API::s_instance->sdk()->type_definition->is_by_ref(*this);
        }

        bool is_pointer() const {
            return API::s_instance->sdk()->type_definition->is_pointer(*this);
        }

        bool is_primitive() const {
            return API::s_instance->sdk()->type_definition->is_primitive(*this);
        }

        ::REFrameworkVMObjType get_vm_obj_type() const {
            return API::s_instance->sdk()->type_definition->get_vm_obj_type(*this);
        }

        API::Method* find_method(std::string_view name) const {
            return (API::Method*)API::s_instance->sdk()->type_definition->find_method(*this, name.data());
        }

        API::Field* find_field(std::string_view name) const {
            return (API::Field*)API::s_instance->sdk()->type_definition->find_field(*this, name.data());
        }

        API::Property* find_property(std::string_view name) const {
            return (API::Property*)API::s_instance->sdk()->type_definition->find_property(*this, name.data());
        }

        std::vector<API::Method*> get_methods() const {
            std::vector<API::Method*> methods;
            methods.resize(get_num_methods());

            auto result = API::s_instance->sdk()->type_definition->get_methods(*this, (REFrameworkMethodHandle*)&methods[0], methods.size() * sizeof(API::Method*), nullptr);

            if (result != REFRAMEWORK_ERROR_NONE) {
                return {};
            }

            return methods;
        }

        std::vector<API::Field*> get_fields() const {
            std::vector<API::Field*> fields;
            fields.resize(get_num_fields());

            auto result = API::s_instance->sdk()->type_definition->get_fields(*this, (REFrameworkFieldHandle*)&fields[0], fields.size() * sizeof(API::Field*), nullptr);

            if (result != REFRAMEWORK_ERROR_NONE) {
                return {};
            }

            return fields;
        }

        std::vector<API::Property*> get_properties() const {
            throw std::runtime_error("Not implemented");
            return {};
        }

        void* get_instance() const {
            return API::s_instance->sdk()->type_definition->get_instance(*this);
        }

        void* create_instance_deprecated() const {
            return API::s_instance->sdk()->type_definition->create_instance_deprecated(*this);
        }

        API::ManagedObject* create_instance(int flags = 0) const {
            return (API::ManagedObject*)API::s_instance->sdk()->type_definition->create_instance(*this, flags);
        }

        API::TypeDefinition* get_parent_type() const {
            return (API::TypeDefinition*)API::s_instance->sdk()->type_definition->get_parent_type(*this);
        }

        API::TypeDefinition* get_declaring_type() const {
            return (API::TypeDefinition*)API::s_instance->sdk()->type_definition->get_declaring_type(*this);
        }

        API::TypeDefinition* get_underlying_type() const {
            return (API::TypeDefinition*)API::s_instance->sdk()->type_definition->get_underlying_type(*this);
        }

        API::TypeInfo* get_type_info() const {
            return (API::TypeInfo*)API::s_instance->sdk()->type_definition->get_type_info(*this);
        }

        API::ManagedObject* get_runtime_type() const {
            return (API::ManagedObject*)API::s_instance->sdk()->type_definition->get_runtime_type(*this);
        }
    };

    struct Method {
        operator ::REFrameworkMethodHandle() const {
            return (::REFrameworkMethodHandle)this;
        }

        reframework::InvokeRet invoke(API::ManagedObject* obj, const std::vector<void*>& args) {
            reframework::InvokeRet out{};

            auto result = API::s_instance->sdk()->method->invoke(*this, obj, (void**)&args[0], args.size() * sizeof(void*), &out, sizeof(out));

#ifdef REFRAMEWORK_API_EXCEPTIONS
            if (result != REFRAMEWORK_ERROR_NONE) {
                throw std::runtime_error("Method invocation failed");
            }
#endif

            return out;
        }

        template<typename T>
        T get_function() const {
            return (T)API::s_instance->sdk()->method->get_function(*this);
        }

        void* get_function_raw() const {
            return API::s_instance->sdk()->method->get_function(*this);
        }

        // e.g. call<void*>(sdk->get_vm_context(), obj, args...);
        template<typename Ret = void*, typename ...Args>
        Ret call(Args... args) const {
            return get_function<Ret (*)(Args...)>()(args...);
        }

        const char* get_name() const {
            return API::s_instance->sdk()->method->get_name(*this);
        }

        API::TypeDefinition* get_declaring_type() const {
            return (API::TypeDefinition*)API::s_instance->sdk()->method->get_declaring_type(*this);
        }

        API::TypeDefinition* get_return_type() const {
            return (API::TypeDefinition*)API::s_instance->sdk()->method->get_return_type(*this);
        }

        uint32_t get_num_params() const {
            return API::s_instance->sdk()->method->get_num_params(*this);
        }

        std::vector<REFrameworkMethodParameter> get_params() const {
            std::vector<REFrameworkMethodParameter> params;
            params.resize(get_num_params());

            auto result = API::s_instance->sdk()->method->get_params(*this, (REFrameworkMethodParameter*)&params[0], params.size() * sizeof(REFrameworkMethodParameter), nullptr);

#ifdef REFRAMEWORK_API_EXCEPTIONS
            if (result != REFRAMEWORK_ERROR_NONE) {
                throw std::runtime_error("Method parameter retrieval failed");
            }
#else
            if (result != REFRAMEWORK_ERROR_NONE) {
                return {};
            }
#endif

            return params;
        }

        uint32_t get_index() const {
            return API::s_instance->sdk()->method->get_index(*this);
        }

        int get_virtual_index() const {
            return API::s_instance->sdk()->method->get_virtual_index(*this);
        }

        bool is_static() const {
            return API::s_instance->sdk()->method->is_static(*this);
        }

        uint16_t get_flags() const {
            return API::s_instance->sdk()->method->get_flags(*this);
        }

        uint16_t get_impl_flags() const {
            return API::s_instance->sdk()->method->get_impl_flags(*this);
        }

        uint32_t get_invoke_id() const {
            return API::s_instance->sdk()->method->get_invoke_id(*this);
        }

        unsigned int add_hook(REFPreHookFn pre_fn, REFPostHookFn post_fn, bool ignore_jmp) const {
            return API::s_instance->sdk()->functions->add_hook(*this, pre_fn, post_fn, ignore_jmp);
        }

        void remove_hook(unsigned int hook_id) const {
            API::s_instance->sdk()->functions->remove_hook(*this, hook_id);
        }
    };

    struct Field {
        operator ::REFrameworkFieldHandle() const {
            return (::REFrameworkFieldHandle)this;
        }

        const char* get_name() const {
            return API::s_instance->sdk()->field->get_name(*this);
        }

        API::TypeDefinition* get_declaring_type() const {
            return (API::TypeDefinition*)API::s_instance->sdk()->field->get_declaring_type(*this);
        }

        API::TypeDefinition* get_type() const {
            return (API::TypeDefinition*)API::s_instance->sdk()->field->get_type(*this);
        }

        uint32_t get_offset_from_base() const {
            return API::s_instance->sdk()->field->get_offset_from_base(*this);
        }

        uint32_t get_offset_from_fieldptr() const {
            return API::s_instance->sdk()->field->get_offset_from_fieldptr(*this);
        }

        uint32_t get_flags() const {
            return API::s_instance->sdk()->field->get_flags(*this);
        }

        bool is_static() const {
            return API::s_instance->sdk()->field->is_static(*this);
        }

        bool is_literal() const {
            return API::s_instance->sdk()->field->is_literal(*this);
        }

        void* get_init_data() const {
            return API::s_instance->sdk()->field->get_init_data(*this);
        }

        void* get_data_raw(void* obj, bool is_value_type = false) const {
            return API::s_instance->sdk()->field->get_data_raw(*this, obj, is_value_type);
        }

        template <typename T> T& get_data(void* object = nullptr, bool is_value_type = false) const { return *(T*)get_data_raw(object); }
    };

    struct Property {
        operator ::REFrameworkPropertyHandle() const {
            return (::REFrameworkPropertyHandle)this;
        }

        // TODO: Implement
    };

    struct ManagedObject {
        operator ::REFrameworkManagedObjectHandle() const {
            return (::REFrameworkManagedObjectHandle)this;
        }

        void add_ref() {
            API::s_instance->sdk()->managed_object->add_ref(*this);
        }

        void release() {
            API::s_instance->sdk()->managed_object->release(*this);
        }

        API::TypeDefinition* get_type_definition() const {
            return (API::TypeDefinition*)API::s_instance->sdk()->managed_object->get_type_definition(*this);
        }

        bool is_managed_object() const {
            return API::s_instance->sdk()->managed_object->is_managed_object(*this);
        }

        uint32_t get_ref_count() const {
            return API::s_instance->sdk()->managed_object->get_ref_count(*this);
        }

        uint32_t get_vm_obj_type() const {
            return API::s_instance->sdk()->managed_object->get_vm_obj_type(*this);
        }

        API::TypeInfo* get_type_info() const {
            return (API::TypeInfo*)API::s_instance->sdk()->managed_object->get_type_info(*this);
        }

        void* get_reflection_properties() const {
            return API::s_instance->sdk()->managed_object->get_reflection_properties(*this);
        }

        API::ReflectionProperty* get_reflection_property_descriptor(std::string_view name) {
            return (API::ReflectionProperty*)API::s_instance->sdk()->managed_object->get_reflection_property_descriptor(*this, name.data());
        }

        API::ReflectionMethod* get_reflection_method_descriptor(std::string_view name) {
            return (API::ReflectionMethod*)API::s_instance->sdk()->managed_object->get_reflection_method_descriptor(*this, name.data());
        }

        template<typename Ret = void*, typename ...Args>
        Ret call(std::string_view method_name, Args... args) const {
            auto t = get_type_definition();

            if (t == nullptr) {
                return Ret{};
            }

            auto m = t->find_method(method_name);

            if (m == nullptr) {
                return Ret{};
            }

            return m->get_function<Ret (*)(Args...)>()(args...);
        }

        reframework::InvokeRet invoke(std::string_view method_name, const std::vector<void*>& args) {
            auto t = get_type_definition();

            if (t == nullptr) {
                return {};
            }

            auto m = t->find_method(method_name);

            if (m == nullptr) {
                return {};
            }

            return m->invoke(this, args);
        }

        template<typename T>
        T* get_field(std::string_view name, bool is_value_type = false) const {
            auto t = get_type_definition();

            if (t == nullptr) {
                return nullptr;
            }

            auto f = t->find_field(name);

            if (f == nullptr) {
                return nullptr;
            }

            return (T*)f->get_data_raw((void*)this, is_value_type);
        }
    };

    struct ResourceManager {
        operator ::REFrameworkResourceManagerHandle() const {
            return (::REFrameworkResourceManagerHandle)this;
        }

        API::Resource* create_resource(std::string_view type_name, std::string_view name) {
            return (API::Resource*)API::s_instance->sdk()->resource_manager->create_resource(*this, type_name.data(), name.data());
        }
    };
    
    struct Resource {
        operator ::REFrameworkResourceHandle() const {
            return (::REFrameworkResourceHandle)this;
        }

        void add_ref() {
            API::s_instance->sdk()->resource->add_ref(*this);
        }

        void release() {
            API::s_instance->sdk()->resource->release(*this);
        }
    };

    struct TypeInfo {
        operator ::REFrameworkTypeInfoHandle() const {
            return (::REFrameworkTypeInfoHandle)this;
        }

        const char* get_name() const {
            return API::s_instance->sdk()->type_info->get_name(*this);
        }

        API::TypeDefinition* get_type_definition() const {
            return (API::TypeDefinition*)API::s_instance->sdk()->type_info->get_type_definition(*this);
        }

        bool is_clr_type() const {
            return API::s_instance->sdk()->type_info->is_clr_type(*this);
        }

        bool is_singleton() const {
            return API::s_instance->sdk()->type_info->is_singleton(*this);
        }

        void* get_singleton_instance() const {
            return API::s_instance->sdk()->type_info->get_singleton_instance(*this);
        }

        void* get_reflection_properties() const {
            return API::s_instance->sdk()->type_info->get_reflection_properties(*this);
        }

        API::ReflectionProperty* get_reflection_property_descriptor(std::string_view name) {
            return (API::ReflectionProperty*)API::s_instance->sdk()->type_info->get_reflection_property_descriptor(*this, name.data());
        }

        API::ReflectionMethod* get_reflection_method_descriptor(std::string_view name) {
            return (API::ReflectionMethod*)API::s_instance->sdk()->type_info->get_reflection_method_descriptor(*this, name.data());
        }

        void* get_deserializer_fn() const {
            return API::s_instance->sdk()->type_info->get_deserializer_fn(*this);
        }

        API::TypeInfo* get_parent() const {
            return (API::TypeInfo*)API::s_instance->sdk()->type_info->get_parent(*this);
        }

        uint32_t get_crc() const {
            return API::s_instance->sdk()->type_info->get_crc(*this);
        }
    };

    struct VMContext {
        operator ::REFrameworkVMContextHandle() const {
            return (::REFrameworkVMContextHandle)this;
        }

        bool has_exception() const {
            return API::s_instance->sdk()->vm_context->has_exception(*this);
        }

        void unhandled_exception() {
            API::s_instance->sdk()->vm_context->unhandled_exception(*this);
        }

        void local_frame_gc() {
            API::s_instance->sdk()->vm_context->local_frame_gc(*this);
        }

        void cleanup_after_exception(int32_t old_ref_count) {
            API::s_instance->sdk()->vm_context->cleanup_after_exception(*this, old_ref_count);
        }
    };

    struct ReflectionMethod {
        operator ::REFrameworkReflectionMethodHandle() const {
            return (::REFrameworkReflectionMethodHandle)this;
        }

        ::REFrameworkInvokeMethod get_function() const {
            return API::s_instance->sdk()->reflection_method->get_function(*this);
        }
    };

    struct ReflectionProperty {
        operator ::REFrameworkReflectionPropertyHandle() const {
            return (::REFrameworkReflectionPropertyHandle)this;
        }

        ::REFrameworkReflectionPropertyMethod get_getter() const {
            return API::s_instance->sdk()->reflection_property->get_getter(*this);
        }

        bool is_static() const {
            return API::s_instance->sdk()->reflection_property->is_static(*this);
        }

        uint32_t get_size() const {
            return API::s_instance->sdk()->reflection_property->get_size(*this);
        }
    };

private:
    const REFrameworkPluginInitializeParam* m_param;
    const REFrameworkSDKData* m_sdk;
    std::recursive_mutex m_lua_mtx{};
};
}