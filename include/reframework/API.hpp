#pragma once

extern "C" {
    #include "API.h"
}

#include <span>
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

    inline const REFrameworkSDKData* sdk() const {
        return m_sdk;
    }

    inline const auto tdb() const { 
        static const auto fn = sdk()->functions->get_tdb;
        return (TDB*)fn(); 
    }

    inline const auto resource_manager() const {
        static const auto fn = sdk()->functions->get_resource_manager;
        return (ResourceManager*)fn();
    }

    inline const auto reframework() const {
        static const auto fn = param()->functions;
        return (REFramework*)fn;
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
        static const auto fn = sdk()->functions->get_vm_context;
        return (API::VMContext*)fn();
    }

    API::ManagedObject* typeof(const char* name) const {
        static const auto fn = sdk()->functions->typeof_;
        return (API::ManagedObject*)fn(name);
    }

    API::ManagedObject* get_managed_singleton(std::string_view name) const {
        static const auto fn = sdk()->functions->get_managed_singleton;
        return (API::ManagedObject*)fn(name.data());
    }

    void* get_native_singleton(std::string_view name) const {
        static const auto fn = sdk()->functions->get_native_singleton;
        return fn(name.data());
    }

    std::vector<REFrameworkManagedSingleton> get_managed_singletons() const {
        static const auto fn = sdk()->functions->get_managed_singletons;

        std::vector<REFrameworkManagedSingleton> out{};
        out.resize(512);

        uint32_t count{};

        auto result = fn(&out[0], out.size() * sizeof(REFrameworkManagedSingleton), &count);

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
        static const auto fn = sdk()->functions->get_native_singletons;

        std::vector<REFrameworkNativeSingleton> out{};
        out.resize(512);

        uint32_t count{};

        auto result = fn(&out[0], out.size() * sizeof(REFrameworkNativeSingleton), &count);

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
            static const auto fn = API::s_instance->sdk()->tdb->get_num_types;
            return fn(*this);
        }

        uint32_t get_num_methods() const {
            static const auto fn = API::s_instance->sdk()->tdb->get_num_methods;
            return fn(*this);
        }

        uint32_t get_num_fields() const {
            static const auto fn = API::s_instance->sdk()->tdb->get_num_fields;
            return fn(*this);
        }

        uint32_t get_num_properties() const {
            static const auto fn = API::s_instance->sdk()->tdb->get_num_properties;
            return fn(*this);
        }

        uint32_t get_strings_size() const {
            static const auto fn = API::s_instance->sdk()->tdb->get_strings_size;
            return fn(*this);
        }

        uint32_t get_raw_data_size() const {
            static const auto fn = API::s_instance->sdk()->tdb->get_raw_data_size;
            return fn(*this);
        }

        const char* get_string_database() const {
            static const auto fn = API::s_instance->sdk()->tdb->get_string_database;
            return fn(*this);
        }

        uint8_t* get_raw_database() const {
            static const auto fn = API::s_instance->sdk()->tdb->get_raw_database;
            return (uint8_t*)fn(*this);
        }

        API::TypeDefinition* get_type(uint32_t index) const {
            static const auto fn = API::s_instance->sdk()->tdb->get_type;
            return (API::TypeDefinition*)fn(*this, index);
        }

        API::TypeDefinition* find_type(std::string_view name) const {
            static const auto fn = API::s_instance->sdk()->tdb->find_type;
            return (API::TypeDefinition*)fn(*this, name.data());
        }

        API::TypeDefinition* find_type_by_fqn(uint32_t fqn) const {
            static const auto fn = API::s_instance->sdk()->tdb->find_type_by_fqn;
            return (API::TypeDefinition*)fn(*this, fqn);
        }

        API::Method* get_method(uint32_t index) const {
            static const auto fn = API::s_instance->sdk()->tdb->get_method;
            return (API::Method*)fn(*this, index);
        }

        API::Method* find_method(std::string_view type_name, std::string_view name) const {
            static const auto fn = API::s_instance->sdk()->tdb->find_method;
            return (API::Method*)fn(*this, type_name.data(), name.data());
        }

        API::Field* get_field(uint32_t index) const {
            static const auto fn = API::s_instance->sdk()->tdb->get_field;
            return (API::Field*)fn(*this, index);
        }

        API::Field* find_field(std::string_view type_name, std::string_view name) const {
            static const auto fn = API::s_instance->sdk()->tdb->find_field;
            return (API::Field*)fn(*this, type_name.data(), name.data());
        }

        API::Property* get_property(uint32_t index) const {
            static const auto fn = API::s_instance->sdk()->tdb->get_property;
            return (API::Property*)fn(*this, index);
        }
    };

    struct REFramework {
        operator ::REFrameworkHandle() {
            return (::REFrameworkHandle)this;
        }

        bool is_drawing_ui() const {
            static const auto fn = API::s_instance->param()->functions->is_drawing_ui;
            return fn();
        }
    };

    struct TypeDefinition {
        operator ::REFrameworkTypeDefinitionHandle() const {
            return (::REFrameworkTypeDefinitionHandle)this;
        }

        uint32_t get_index() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_index;
            return fn(*this);
        }

        uint32_t get_size() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_size;
            return fn(*this);
        }

        uint32_t get_valuetype_size() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_valuetype_size;
            return fn(*this);
        }

        uint32_t get_fqn() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_fqn;
            return fn(*this);
        }

        const char* get_name() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_name;
            return fn(*this);
        }

        const char* get_namespace() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_namespace;
            return fn(*this);
        }

        std::string get_full_name() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_full_name;

            std::string buffer{};
            buffer.resize(512);

            uint32_t real_size{0};
            auto result = fn(*this, &buffer[0], buffer.size(), &real_size);

            if (result != REFRAMEWORK_ERROR_NONE) {
                return "";
            }

            buffer.resize(real_size);
            return buffer;
        }

        bool has_fieldptr_offset() const {
            static const auto fn = API::s_instance->sdk()->type_definition->has_fieldptr_offset;
            return fn(*this);
        }

        int32_t get_fieldptr_offset() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_fieldptr_offset;
            return fn(*this);
        }

        uint32_t get_num_methods() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_num_methods;
            return fn(*this);
        }

        uint32_t get_num_fields() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_num_fields;
            return fn(*this);
        }

        uint32_t get_num_properties() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_num_properties;
            return fn(*this);
        }

        bool is_derived_from(API::TypeDefinition* other) {
            static const auto fn = API::s_instance->sdk()->type_definition->is_derived_from;
            return fn(*this, *other);
        }

        bool is_derived_from(std::string_view other) {
            static const auto fn = API::s_instance->sdk()->type_definition->is_derived_from_by_name;
            return fn(*this, other.data());
        }

        bool is_valuetype() const {
            static const auto fn = API::s_instance->sdk()->type_definition->is_valuetype;
            return fn(*this);
        }

        bool is_enum() const {
            static const auto fn = API::s_instance->sdk()->type_definition->is_enum;
            return fn(*this);
        }

        bool is_by_ref() const {
            static const auto fn = API::s_instance->sdk()->type_definition->is_by_ref;
            return fn(*this);
        }

        bool is_pointer() const {
            static const auto fn = API::s_instance->sdk()->type_definition->is_pointer;
            return fn(*this);
        }

        bool is_primitive() const {
            static const auto fn = API::s_instance->sdk()->type_definition->is_primitive;
            return fn(*this);
        }

        ::REFrameworkVMObjType get_vm_obj_type() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_vm_obj_type;
            return fn(*this);
        }

        API::Method* find_method(std::string_view name) const {
            static const auto fn = API::s_instance->sdk()->type_definition->find_method;
            return (API::Method*)fn(*this, name.data());
        }

        API::Field* find_field(std::string_view name) const {
            static const auto fn = API::s_instance->sdk()->type_definition->find_field;
            return (API::Field*)fn(*this, name.data());
        }

        API::Property* find_property(std::string_view name) const {
            static const auto fn = API::s_instance->sdk()->type_definition->find_property;
            return (API::Property*)fn(*this, name.data());
        }

        std::vector<API::Method*> get_methods() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_methods;

            std::vector<API::Method*> methods;
            methods.resize(get_num_methods());

            auto result = fn(*this, (REFrameworkMethodHandle*)&methods[0], methods.size() * sizeof(API::Method*), nullptr);

            if (result != REFRAMEWORK_ERROR_NONE) {
                return {};
            }

            return methods;
        }

        std::vector<API::Field*> get_fields() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_fields;

            std::vector<API::Field*> fields;
            fields.resize(get_num_fields());

            auto result = fn(*this, (REFrameworkFieldHandle*)&fields[0], fields.size() * sizeof(API::Field*), nullptr);

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
            static const auto fn = API::s_instance->sdk()->type_definition->get_instance;
            return fn(*this);
        }

        void* create_instance_deprecated() const {
            static const auto fn = API::s_instance->sdk()->type_definition->create_instance_deprecated;
            return fn(*this);
        }

        API::ManagedObject* create_instance(int flags = 0) const {
            static const auto fn = API::s_instance->sdk()->type_definition->create_instance;
            return (API::ManagedObject*)fn(*this, flags);
        }

        API::TypeDefinition* get_parent_type() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_parent_type;
            return (API::TypeDefinition*)fn(*this);
        }

        API::TypeDefinition* get_declaring_type() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_declaring_type;
            return (API::TypeDefinition*)fn(*this);
        }

        API::TypeDefinition* get_underlying_type() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_underlying_type;
            return (API::TypeDefinition*)fn(*this);
        }

        API::TypeInfo* get_type_info() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_type_info;
            return (API::TypeInfo*)fn(*this);
        }

        API::ManagedObject* get_runtime_type() const {
            static const auto fn = API::s_instance->sdk()->type_definition->get_runtime_type;
            return (API::ManagedObject*)fn(*this);
        }
    };

    struct Method {
        operator ::REFrameworkMethodHandle() const {
            return (::REFrameworkMethodHandle)this;
        }

        reframework::InvokeRet invoke(API::ManagedObject* obj, const std::vector<void*>& args) {
            static const auto fn = API::s_instance->sdk()->method->invoke;
            reframework::InvokeRet out{};

            auto result = fn(*this, obj, (void**)&args[0], args.size() * sizeof(void*), &out, sizeof(out));

#ifdef REFRAMEWORK_API_EXCEPTIONS
            if (result != REFRAMEWORK_ERROR_NONE) {
                throw std::runtime_error("Method invocation failed");
            }
#endif

            return out;
        }

        reframework::InvokeRet invoke(API::ManagedObject* obj, const std::span<void*>& args) {
            static const auto fn = API::s_instance->sdk()->method->invoke;
            reframework::InvokeRet out{};

            auto result = fn(*this, obj, args.data(), args.size() * sizeof(void*), &out, sizeof(out));

#ifdef REFRAMEWORK_API_EXCEPTIONS
            if (result != REFRAMEWORK_ERROR_NONE) {
                throw std::runtime_error("Method invocation failed");
            }
#endif

            return out;
        }

        template<typename T>
        T get_function() const {
            static const auto fn = API::s_instance->sdk()->method->get_function;
            return (T)fn(*this);
        }

        void* get_function_raw() const {
            static const auto fn = API::s_instance->sdk()->method->get_function;
            return fn(*this);
        }

        // e.g. call<void*>(sdk->get_vm_context(), obj, args...);
        template<typename Ret = void*, typename ...Args>
        Ret call(Args... args) const {
            return get_function<Ret (*)(Args...)>()(args...);
        }

        const char* get_name() const {
            static const auto fn = API::s_instance->sdk()->method->get_name;
            return API::s_instance->sdk()->method->get_name(*this);
        }

        API::TypeDefinition* get_declaring_type() const {
            static const auto fn = API::s_instance->sdk()->method->get_declaring_type;
            return (API::TypeDefinition*)fn(*this);
        }

        API::TypeDefinition* get_return_type() const {
            static const auto fn = API::s_instance->sdk()->method->get_return_type;
            return (API::TypeDefinition*)fn(*this);
        }

        uint32_t get_num_params() const {
            static const auto fn = API::s_instance->sdk()->method->get_num_params;
            return fn(*this);
        }

        std::vector<REFrameworkMethodParameter> get_params() const {
            static const auto fn = API::s_instance->sdk()->method->get_params;

            std::vector<REFrameworkMethodParameter> params;
            params.resize(get_num_params());

            auto result = fn(*this, (REFrameworkMethodParameter*)&params[0], params.size() * sizeof(REFrameworkMethodParameter), nullptr);

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
            static const auto fn = API::s_instance->sdk()->method->get_index;
            return fn(*this);
        }

        int get_virtual_index() const {
            static const auto fn = API::s_instance->sdk()->method->get_virtual_index;
            return fn(*this);
        }

        bool is_static() const {
            static const auto fn = API::s_instance->sdk()->method->is_static;
            return fn(*this);
        }

        uint16_t get_flags() const {
            static const auto fn = API::s_instance->sdk()->method->get_flags;
            return fn(*this);
        }

        uint16_t get_impl_flags() const {
            static const auto fn = API::s_instance->sdk()->method->get_impl_flags;
            return fn(*this);
        }

        uint32_t get_invoke_id() const {
            static const auto fn = API::s_instance->sdk()->method->get_invoke_id;
            return fn(*this);
        }

        unsigned int add_hook(REFPreHookFn pre_fn, REFPostHookFn post_fn, bool ignore_jmp) const {
            static const auto fn = API::s_instance->sdk()->functions->add_hook;
            return fn(*this, pre_fn, post_fn, ignore_jmp);
        }

        void remove_hook(unsigned int hook_id) const {

            static const auto fn = API::s_instance->sdk()->functions->remove_hook;
            fn(*this, hook_id);
        }
    };

    struct Field {
        operator ::REFrameworkFieldHandle() const {
            return (::REFrameworkFieldHandle)this;
        }

        const char* get_name() const {
            static const auto fn = API::s_instance->sdk()->field->get_name;
            return fn(*this);
        }

        API::TypeDefinition* get_declaring_type() const {
            static const auto fn = API::s_instance->sdk()->field->get_declaring_type;
            return (API::TypeDefinition*)fn(*this);
        }

        API::TypeDefinition* get_type() const {
            static const auto fn = API::s_instance->sdk()->field->get_type;
            return (API::TypeDefinition*)fn(*this);
        }

        uint32_t get_offset_from_base() const {
            static const auto fn = API::s_instance->sdk()->field->get_offset_from_base;
            return fn(*this);
        }

        uint32_t get_offset_from_fieldptr() const {
            static const auto fn = API::s_instance->sdk()->field->get_offset_from_fieldptr;
            return fn(*this);
        }

        uint32_t get_flags() const {
            static const auto fn = API::s_instance->sdk()->field->get_flags;
            return fn(*this);
        }

        bool is_static() const {
            static const auto fn = API::s_instance->sdk()->field->is_static;
            return fn(*this);
        }

        bool is_literal() const {
            static const auto fn = API::s_instance->sdk()->field->is_literal;
            return fn(*this);
        }

        void* get_init_data() const {
            static const auto fn = API::s_instance->sdk()->field->get_init_data;
            return fn(*this);
        }

        void* get_data_raw(void* obj, bool is_value_type = false) const {
            static const auto fn = API::s_instance->sdk()->field->get_data_raw;
            return fn(*this, obj, is_value_type);
        }

        uint32_t get_index() const {
            static const auto fn = API::s_instance->sdk()->field->get_index;
            return fn(*this);
        }

        template <typename T> T& get_data(void* object = nullptr, bool is_value_type = false) const { return *(T*)get_data_raw(object, is_value_type); }
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
            static const auto fn = API::s_instance->sdk()->managed_object->add_ref;
            fn(*this);
        }

        void release() {
            static const auto fn = API::s_instance->sdk()->managed_object->release;
            fn(*this);
        }

        API::TypeDefinition* get_type_definition() const {
            static const auto fn = API::s_instance->sdk()->managed_object->get_type_definition;
            return (API::TypeDefinition*)fn(*this);
        }

        bool is_managed_object() const {
            static const auto fn = API::s_instance->sdk()->managed_object->is_managed_object;
            return fn(*this);
        }

        uint32_t get_ref_count() const {
            static const auto fn = API::s_instance->sdk()->managed_object->get_ref_count;
            return fn(*this);
        }

        uint32_t get_vm_obj_type() const {
            static const auto fn = API::s_instance->sdk()->managed_object->get_vm_obj_type;
            return fn(*this);
        }

        API::TypeInfo* get_type_info() const {
            static const auto fn = API::s_instance->sdk()->managed_object->get_type_info;
            return (API::TypeInfo*)fn(*this);
        }

        void* get_reflection_properties() const {
            static const auto fn = API::s_instance->sdk()->managed_object->get_reflection_properties;
            return fn(*this);
        }

        API::ReflectionProperty* get_reflection_property_descriptor(std::string_view name) {
            static const auto fn = API::s_instance->sdk()->managed_object->get_reflection_property_descriptor;
            return (API::ReflectionProperty*)fn(*this, name.data());
        }

        API::ReflectionMethod* get_reflection_method_descriptor(std::string_view name) {
            static const auto fn = API::s_instance->sdk()->managed_object->get_reflection_method_descriptor;
            return (API::ReflectionMethod*)fn(*this, name.data());
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
            static const auto fn = API::s_instance->sdk()->resource_manager->create_resource;
            return (API::Resource*)fn(*this, type_name.data(), name.data());
        }
    };
    
    struct Resource {
        operator ::REFrameworkResourceHandle() const {
            return (::REFrameworkResourceHandle)this;
        }

        void add_ref() {
            static const auto fn = API::s_instance->sdk()->resource->add_ref;
            fn(*this);
        }

        void release() {
            static const auto fn = API::s_instance->sdk()->resource->release;
            fn(*this);
        }
    };

    struct TypeInfo {
        operator ::REFrameworkTypeInfoHandle() const {
            return (::REFrameworkTypeInfoHandle)this;
        }

        const char* get_name() const {
            static const auto fn = API::s_instance->sdk()->type_info->get_name;
            return fn(*this);
        }

        API::TypeDefinition* get_type_definition() const {
            static const auto fn = API::s_instance->sdk()->type_info->get_type_definition;
            return (API::TypeDefinition*)fn(*this);
        }

        bool is_clr_type() const {
            static const auto fn = API::s_instance->sdk()->type_info->is_clr_type;
            return fn(*this);
        }

        bool is_singleton() const {
            static const auto fn = API::s_instance->sdk()->type_info->is_singleton;
            return fn(*this);
        }

        void* get_singleton_instance() const {
            static const auto fn = API::s_instance->sdk()->type_info->get_singleton_instance;
            return fn(*this);
        }

        void* get_reflection_properties() const {
            static const auto fn = API::s_instance->sdk()->type_info->get_reflection_properties;
            return fn(*this);
        }

        API::ReflectionProperty* get_reflection_property_descriptor(std::string_view name) {
            static const auto fn = API::s_instance->sdk()->type_info->get_reflection_property_descriptor;
            return (API::ReflectionProperty*)API::s_instance->sdk()->type_info->get_reflection_property_descriptor(*this, name.data());
        }

        API::ReflectionMethod* get_reflection_method_descriptor(std::string_view name) {
            static const auto fn = API::s_instance->sdk()->type_info->get_reflection_method_descriptor;
            return (API::ReflectionMethod*)fn(*this, name.data());
        }

        void* get_deserializer_fn() const {
            static const auto fn = API::s_instance->sdk()->type_info->get_deserializer_fn;
            return fn(*this);
        }

        API::TypeInfo* get_parent() const {
            static const auto fn = API::s_instance->sdk()->type_info->get_parent;
            return (API::TypeInfo*)fn(*this);
        }

        uint32_t get_crc() const {
            static const auto fn = API::s_instance->sdk()->type_info->get_crc;
            return fn(*this);
        }
    };

    struct VMContext {
        operator ::REFrameworkVMContextHandle() const {
            return (::REFrameworkVMContextHandle)this;
        }

        bool has_exception() const {
            static const auto fn = API::s_instance->sdk()->vm_context->has_exception;
            return fn(*this);
        }

        void unhandled_exception() {
            static const auto fn = API::s_instance->sdk()->vm_context->unhandled_exception;
            fn(*this);
        }

        void local_frame_gc() {
            static const auto fn = API::s_instance->sdk()->vm_context->local_frame_gc;
            fn(*this);
        }

        void cleanup_after_exception(int32_t old_ref_count) {
            static const auto fn = API::s_instance->sdk()->vm_context->cleanup_after_exception;
            fn(*this, old_ref_count);
        }
    };

    struct ReflectionMethod {
        operator ::REFrameworkReflectionMethodHandle() const {
            return (::REFrameworkReflectionMethodHandle)this;
        }

        ::REFrameworkInvokeMethod get_function() const {
            static const auto fn = API::s_instance->sdk()->reflection_method->get_function;
            return fn(*this);
        }
    };

    struct ReflectionProperty {
        operator ::REFrameworkReflectionPropertyHandle() const {
            return (::REFrameworkReflectionPropertyHandle)this;
        }

        ::REFrameworkReflectionPropertyMethod get_getter() const {
            static const auto fn = API::s_instance->sdk()->reflection_property->get_getter;
            return fn(*this);
        }

        bool is_static() const {
            static const auto fn = API::s_instance->sdk()->reflection_property->is_static;
            return fn(*this);
        }

        uint32_t get_size() const {
            static const auto fn = API::s_instance->sdk()->reflection_property->get_size;
            return fn(*this);
        }
    };

private:
    const REFrameworkPluginInitializeParam* m_param;
    const REFrameworkSDKData* m_sdk;
    std::recursive_mutex m_lua_mtx{};
};
}
