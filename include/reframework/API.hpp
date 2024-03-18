#pragma once

#ifdef __cplusplus_cli
#pragma managed
#include <msclr/gcroot.h>
#include <msclr/marshal_cppstd.h>
//#include <cliext/vector>
#define REF_CONTAINER_NAMESPACE cliext
#else
#define REF_CONTAINER_NAMESPACE std
#endif

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

#ifdef __cplusplus_cli
public ref struct DotNetInvokeRet {
    property array<uint8_t>^ Bytes;
    property uint8_t Byte;
    property uint16_t Word;
    property uint32_t DWord;
    property float F;
    property uint64_t QWord;
    property double D;
    property System::Object^ Ptr;
    property bool ExceptionThrown;
};
#endif


#ifdef __cplusplus_cli
#define CPP_MEMBER_CONST
#define CPP_POINTER ^
#define REF_MAKE_POINTER(Tx, x) gcnew Tx(x)
#define CPP_CONTAINER_RET ^
#define CPP_VECTOR(Tx) System::Collections::Generic::List<Tx>^

// I had to resort to this because was having issues with template types causing compiler crashes
#define PUBLIC_POINTER_CONTAINER(X, Tx) ref struct X { public: X(Tx ptr) : m_impl{gcnew System::UIntPtr((void*)ptr)} { } operator Tx() { return (Tx)m_impl->ToPointer(); } void* ptr() { return m_impl->ToPointer(); } private: System::UIntPtr^ m_impl; public:


ref class API;

public ref class API {
#else
#define CPP_MEMBER_CONST const
#define CPP_POINTER *
#define REF_MAKE_POINTER(Tx, x) ((Tx*)x)
#define CPP_CONTAINER_RET
#define CPP_VECTOR(Tx) std::vector<Tx>

class API;

class API {
#endif
private:
    const ::REFrameworkPluginInitializeParam* m_param;
    const REFrameworkSDKData* m_sdk;

#ifndef __cplusplus_cli
    std::recursive_mutex m_lua_mtx{};
#endif

#ifdef __cplusplus_cli
    static API^ s_instance{};
#else
    static inline std::unique_ptr<API> s_instance{};
#endif

public:
#ifdef __cplusplus_cli
    ref struct TDB;
    ref struct TypeDefinition;
    ref struct Method;
    ref struct Field;
    ref struct Property;
    ref struct ManagedObject;
    ref struct ResourceManager;
    ref struct Resource;
    ref struct TypeInfo;
    ref struct VMContext;
    ref struct ReflectionProperty;
    ref struct ReflectionMethod;
    ref struct REFramework_;
#else
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
    struct REFramework_;
#endif

#ifndef __cplusplus_cli
    struct LuaLock {
        LuaLock() {
            API::s_instance->lock_lua();
        }

        virtual ~LuaLock() {
            API::s_instance->unlock_lua();
        }
    };
#endif

public:
    // ALWAYS call initialize first in reframework_plugin_initialize
#ifndef __cplusplus_cli
    static inline std::unique_ptr<reframework::API>& initialize(const REFrameworkPluginInitializeParam* param);
#else
    static inline API^ initialize(uintptr_t param);
#endif

    // only call this AFTER calling initialize
#ifndef __cplusplus_cli
    static inline auto& get() {
#else
    static inline auto get() {
#endif
        if (s_instance == nullptr) {
            throw std::runtime_error("API not initialized");
        }

        return s_instance;
    }

public:
#ifdef __cplusplus_cli
    API(uintptr_t param)
        : m_param{reinterpret_cast<const REFrameworkPluginInitializeParam*>(param)},
        m_sdk{m_param->sdk}
    {
    }
#else
    API(const REFrameworkPluginInitializeParam* param)
        : m_param{param},
        m_sdk{param->sdk}
    {
    }
#endif

    virtual ~API() {

    }

    inline const REFrameworkPluginInitializeParam* param() CPP_MEMBER_CONST {
        return m_param;
    }

    inline const REFrameworkSDKData* sdk() CPP_MEMBER_CONST {
        return m_sdk;
    }

    inline TDB CPP_POINTER tdb() CPP_MEMBER_CONST {
        return REF_MAKE_POINTER(TDB, sdk()->functions->get_tdb());
    }

    inline ResourceManager CPP_POINTER resource_manager() CPP_MEMBER_CONST {
        return REF_MAKE_POINTER(ResourceManager, sdk()->functions->get_resource_manager());
    }

    inline REFramework_ CPP_POINTER reframework() CPP_MEMBER_CONST {
        return REF_MAKE_POINTER(REFramework_, param()->functions);
    }

#ifndef __cplusplus_cli
    void lock_lua() {
        m_lua_mtx.lock();
        m_param->functions->lock_lua();
    }

    void unlock_lua() {
        m_param->functions->unlock_lua();
        m_lua_mtx.unlock();
    }
#endif

    template <typename... Args> void log_error(const char* format, Args... args) { m_param->functions->log_error(format, args...); }
    template <typename... Args> void log_warn(const char* format, Args... args) { m_param->functions->log_warn(format, args...); }
    template <typename... Args> void log_info(const char* format, Args... args) { m_param->functions->log_info(format, args...); }

    API::VMContext CPP_POINTER get_vm_context() CPP_MEMBER_CONST {
        return REF_MAKE_POINTER(API::VMContext, sdk()->functions->get_vm_context());
    }

    API::ManagedObject CPP_POINTER typeof(const char* name) CPP_MEMBER_CONST {
        return REF_MAKE_POINTER(API::ManagedObject, sdk()->functions->typeof_(name));
    }

#ifdef __cplusplus_cli
    API::ManagedObject CPP_POINTER typeof(System::String^ name) CPP_MEMBER_CONST {
        return REF_MAKE_POINTER(API::ManagedObject, sdk()->functions->typeof_(msclr::interop::marshal_as<std::string>(name).c_str()));
    }
#endif


    API::ManagedObject CPP_POINTER get_managed_singleton(std::string_view name) CPP_MEMBER_CONST {
        return REF_MAKE_POINTER(API::ManagedObject, sdk()->functions->get_managed_singleton(name.data()));
    }

#ifdef __cplusplus_cli
    API::ManagedObject CPP_POINTER get_managed_singleton(System::String^ name) CPP_MEMBER_CONST {
        return REF_MAKE_POINTER(API::ManagedObject, sdk()->functions->get_managed_singleton(msclr::interop::marshal_as<std::string>(name).c_str()));
    }
#endif

    void* get_native_singleton(std::string_view name) CPP_MEMBER_CONST {
        return sdk()->functions->get_native_singleton(name.data());
    }

#ifdef __cplusplus_cli
    uintptr_t get_native_singleton(System::String^ name) {
        return uintptr_t(sdk()->functions->get_native_singleton(msclr::interop::marshal_as<std::string>(name).c_str()));
    }
#endif

#ifdef __cplusplus_cli
    ref struct DotNetManagedSingleton {
        property API::ManagedObject^ Instance;
        property API::TypeDefinition^ Type;
        property API::TypeInfo^ TypeInfo;
    };

    System::Collections::Generic::List<DotNetManagedSingleton^>^ get_managed_singletons() CPP_MEMBER_CONST {
#else
    std::vector<REFrameworkManagedSingleton> get_managed_singletons() CPP_MEMBER_CONST {
#endif
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

#ifdef __cplusplus_cli
        System::Collections::Generic::List<DotNetManagedSingleton^>^ out2 = gcnew System::Collections::Generic::List<DotNetManagedSingleton^>();

        for (auto& s : out) {
            DotNetManagedSingleton^ managed = gcnew DotNetManagedSingleton();
            managed->Instance = REF_MAKE_POINTER(API::ManagedObject, s.instance);
            managed->Type = REF_MAKE_POINTER(API::TypeDefinition, s.t);
            managed->TypeInfo = REF_MAKE_POINTER(API::TypeInfo, s.type_info);

            out2->Add(managed);
        }

        return out2;
#else
        return out;
#endif
    }
    
#ifdef __cplusplus_cli
    
#endif

    std::vector<REFrameworkNativeSingleton> get_native_singletons() CPP_MEMBER_CONST {
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
#ifdef __cplusplus_cli

    
    
#else
    template<typename T>
    struct PointerContainer {
    public:
        PointerContainer(T ptr) {};

        operator T() const  {
            return (T)this;
        }

        void* ptr() const {
            return (void*)this;
        }
    };

    #define POINTER_CONTAINER_CONSTRUCTOR(WrapperType, Tx) WrapperType(Tx ptr) : PointerContainer(ptr) {}
    #define PUBLIC_POINTER_CONTAINER(X, Tx) struct X : public PointerContainer<Tx> { \
    public:\
        X(Tx ptr) : PointerContainer(ptr) {}

#endif

    //#define PUBLIC_POINTER_CONTAINER(Tx) : public PointerContainer<Tx>
    //#define POINTER_CONTAINER_CONSTRUCTOR(WrapperType, Tx) WrapperType(Tx ptr) : PointerContainer(ptr) {}

    PUBLIC_POINTER_CONTAINER(TDB, ::REFrameworkTDBHandle)
        uint32_t get_num_types() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->tdb->get_num_types(*this);
        }

        uint32_t get_num_methods() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->tdb->get_num_methods(*this);
        }

        uint32_t get_num_fields() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->tdb->get_num_fields(*this);
        }

        uint32_t get_num_properties() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->tdb->get_num_properties(*this);
        }

        uint32_t get_strings_size() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->tdb->get_strings_size(*this);
        }

        uint32_t get_raw_data_size() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->tdb->get_raw_data_size(*this);
        }

        const char* get_string_database() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->tdb->get_string_database(*this);
        }

#ifdef __cplusplus_cli
        // Use Span<byte> instead
        System::Span<uint8_t> get_raw_data() {
            return System::Span<uint8_t>((uint8_t*)API::s_instance->sdk()->tdb->get_raw_database(*this), get_raw_data_size());
        }

        System::String^ get_string(uint32_t index) {
            if (index >= get_strings_size()) {
                return System::String::Empty;
            }

            return gcnew System::String(API::s_instance->sdk()->tdb->get_string_database(*this) + index);
        }
#endif

        uint8_t* get_raw_database() CPP_MEMBER_CONST {
            return (uint8_t*)API::s_instance->sdk()->tdb->get_raw_database(*this);
        }

        API::TypeDefinition CPP_POINTER get_type(uint32_t index) CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeDefinition, API::s_instance->sdk()->tdb->get_type(*this, index));
        }

        API::TypeDefinition CPP_POINTER find_type(std::string_view name) CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeDefinition, API::s_instance->sdk()->tdb->find_type(*this, name.data()));
        }

#ifdef __cplusplus_cli
        API::TypeDefinition CPP_POINTER find_type(System::String^ name) {
            return REF_MAKE_POINTER(API::TypeDefinition, API::s_instance->sdk()->tdb->find_type(*this, msclr::interop::marshal_as<std::string>(name).c_str()));
        }
#endif

        API::TypeDefinition CPP_POINTER find_type_by_fqn(uint32_t fqn) CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeDefinition, API::s_instance->sdk()->tdb->find_type_by_fqn(*this, fqn));
        }

        API::Method CPP_POINTER get_method(uint32_t index) CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::Method, API::s_instance->sdk()->tdb->get_method(*this, index));
        }

        API::Method CPP_POINTER find_method(std::string_view type_name, std::string_view name) CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::Method, API::s_instance->sdk()->tdb->find_method(*this, type_name.data(), name.data()));
        }

#ifdef __cplusplus_cli
        API::Method CPP_POINTER find_method(System::String^ type_name, System::String^ name) {
            return REF_MAKE_POINTER(API::Method, API::s_instance->sdk()->tdb->find_method(*this, msclr::interop::marshal_as<std::string>(type_name).c_str(), msclr::interop::marshal_as<std::string>(name).c_str()));
        }
#endif

        API::Field CPP_POINTER get_field(uint32_t index) CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::Field, API::s_instance->sdk()->tdb->get_field(*this, index));
        }

        API::Field CPP_POINTER find_field(std::string_view type_name, std::string_view name) CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::Field, API::s_instance->sdk()->tdb->find_field(*this, type_name.data(), name.data()));
        }

#ifdef __cplusplus_cli
        API::Field CPP_POINTER find_field(System::String^ type_name, System::String^ name) {
            return REF_MAKE_POINTER(API::Field, API::s_instance->sdk()->tdb->find_field(*this, msclr::interop::marshal_as<std::string>(type_name).c_str(), msclr::interop::marshal_as<std::string>(name).c_str()));
        }
#endif

        API::Property CPP_POINTER get_property(uint32_t index) CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::Property, API::s_instance->sdk()->tdb->get_property(*this, index));
        }
    };

    PUBLIC_POINTER_CONTAINER(REFramework_, const ::REFrameworkPluginFunctions*)
        bool is_drawing_ui() CPP_MEMBER_CONST {
            return API::s_instance->param()->functions->is_drawing_ui();
        }
    };

    PUBLIC_POINTER_CONTAINER(TypeDefinition, ::REFrameworkTypeDefinitionHandle)
        uint32_t get_index() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->get_index(*this);
        }

        uint32_t get_size() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->get_size(*this);
        }

        uint32_t get_valuetype_size() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->get_valuetype_size(*this);
        }

        uint32_t get_fqn() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->get_fqn(*this);
        }

#ifdef __cplusplus_cli
        System::String^ get_name() {
            return gcnew System::String(API::s_instance->sdk()->type_definition->get_name(*this));
        }

        System::String^ get_namespace() {
            return gcnew System::String(API::s_instance->sdk()->type_definition->get_namespace(*this));
        }
#else
        const char* get_name() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->get_name(*this);
        }

        const char* get_namespace() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->get_namespace(*this);
        }
#endif


#ifdef __cplusplus_cli
        System::String^ get_full_name() {
#else
        std::string get_full_name() CPP_MEMBER_CONST {
#endif
            std::string buffer{};
            buffer.resize(512);

            uint32_t real_size{0};

            const auto sdk = API::s_instance->sdk();
            auto result = sdk->type_definition->get_full_name(*this, &buffer[0], buffer.size(), &real_size);

            if (result != REFRAMEWORK_ERROR_NONE) {
                return "";
            }

            buffer.resize(real_size);

#ifdef __cplusplus_cli
            return gcnew System::String(buffer.c_str());
#else
            return buffer;
#endif
        }

        bool has_fieldptr_offset() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->has_fieldptr_offset(*this);
        }

        int32_t get_fieldptr_offset() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->get_fieldptr_offset(*this);
        }

        uint32_t get_num_methods() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->get_num_methods(*this);
        }

        uint32_t get_num_fields() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->get_num_fields(*this);
        }

        uint32_t get_num_properties() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->get_num_properties(*this);
        }

        bool is_derived_from(API::TypeDefinition CPP_POINTER other) {
            return API::s_instance->sdk()->type_definition->is_derived_from(*this, *other);
        }

        bool is_derived_from(std::string_view other) {
            return API::s_instance->sdk()->type_definition->is_derived_from_by_name(*this, other.data());
        }

#ifdef __cplusplus_cli
        bool is_derived_from(System::String^ other) {
            return API::s_instance->sdk()->type_definition->is_derived_from_by_name(*this, msclr::interop::marshal_as<std::string>(other).c_str());
        }
#endif

        bool is_valuetype() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->is_valuetype(*this);
        }

        bool is_enum() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->is_enum(*this);
        }

        bool is_by_ref() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->is_by_ref(*this);
        }

        bool is_pointer() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->is_pointer(*this);
        }

        bool is_primitive() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->is_primitive(*this);
        }

        ::REFrameworkVMObjType get_vm_obj_type() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->get_vm_obj_type(*this);
        }

        API::Method CPP_POINTER find_method(std::string_view name) CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::Method, API::s_instance->sdk()->type_definition->find_method(*this, name.data()));
        }

        API::Field CPP_POINTER find_field(std::string_view name) CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::Field, API::s_instance->sdk()->type_definition->find_field(*this, name.data()));
        }

        API::Property CPP_POINTER find_property(std::string_view name) CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::Property, API::s_instance->sdk()->type_definition->find_property(*this, name.data()));
        }

#ifdef __cplusplus_cli
        API::Method CPP_POINTER find_method(System::String^ name) {
            return REF_MAKE_POINTER(API::Method, API::s_instance->sdk()->type_definition->find_method(*this, msclr::interop::marshal_as<std::string>(name).c_str()));
        }

        API::Field CPP_POINTER find_field(System::String^ name) {
            return REF_MAKE_POINTER(API::Field, API::s_instance->sdk()->type_definition->find_field(*this, msclr::interop::marshal_as<std::string>(name).c_str()));
        }

        API::Property CPP_POINTER find_property(System::String^ name) {
            return REF_MAKE_POINTER(API::Property, API::s_instance->sdk()->type_definition->find_property(*this, msclr::interop::marshal_as<std::string>(name).c_str()));
        }
#endif

        CPP_VECTOR(API::Method CPP_POINTER) get_methods() CPP_MEMBER_CONST {
            std::vector<void*> methods;
            methods.resize(get_num_methods());

            auto result = API::s_instance->sdk()->type_definition->get_methods(*this, (::REFrameworkMethodHandle*)methods.data(), methods.size() * sizeof(void*), nullptr);

            if (result != REFRAMEWORK_ERROR_NONE) {
                return {};
            }

            #ifdef __cplusplus_cli
                //cliext::vector<API::Method ^> out = gcnew cliext::vector<API::Method ^>();
                System::Collections::Generic::List<API::Method ^>^ out = gcnew System::Collections::Generic::List<API::Method ^>();

                for (auto m : methods) {
                    //out.push_back(REF_MAKE_POINTER(API::Method, m));
                    out->Add(REF_MAKE_POINTER(API::Method, (::REFrameworkMethodHandle)m));
                }

                return out;
            #else
                return *(std::vector<API::Method*>*)&methods;
            #endif
        }

        CPP_VECTOR(API::Field CPP_POINTER)  get_fields() CPP_MEMBER_CONST {
            std::vector<void*> fields;
            fields.resize(get_num_fields());

            auto result = API::s_instance->sdk()->type_definition->get_fields(*this, (REFrameworkFieldHandle*)&fields[0], fields.size() * sizeof(void*), nullptr);

            if (result != REFRAMEWORK_ERROR_NONE) {
                return {};
            }

            #ifdef __cplusplus_cli
                //cliext::::vector<API::Field ^> out = gcnew cliext::vector<API::Field ^>();
                System::Collections::Generic::List<API::Field ^>^ out = gcnew System::Collections::Generic::List<API::Field ^>();

                for (auto f : fields) {
                    out->Add(REF_MAKE_POINTER(API::Field, (::REFrameworkFieldHandle)f));
                }

                return out;
            #else
                return *(std::vector<API::Field*>*)&fields;
            #endif
        }

        CPP_VECTOR(API::Property CPP_POINTER) get_properties() CPP_MEMBER_CONST {
            throw std::runtime_error("Not implemented");
#ifdef __cplusplus_cli
            return nullptr;
#else
            return {};
#endif
        }

        void* get_instance() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->get_instance(*this);
        }

        void* create_instance_deprecated() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_definition->create_instance_deprecated(*this);
        }

        API::ManagedObject CPP_POINTER create_instance(int flags) CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::ManagedObject, API::s_instance->sdk()->type_definition->create_instance(*this, flags));
        }

        API::TypeDefinition CPP_POINTER get_parent_type() CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeDefinition, API::s_instance->sdk()->type_definition->get_parent_type(*this));
        }

        API::TypeDefinition CPP_POINTER get_declaring_type() CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeDefinition, API::s_instance->sdk()->type_definition->get_declaring_type(*this));
        }

        API::TypeDefinition CPP_POINTER get_underlying_type() CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeDefinition, API::s_instance->sdk()->type_definition->get_underlying_type(*this));
        }

        API::TypeInfo CPP_POINTER get_type_info() CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeInfo, API::s_instance->sdk()->type_definition->get_type_info(*this));
        }

        API::ManagedObject CPP_POINTER get_runtime_type() CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::ManagedObject, API::s_instance->sdk()->type_definition->get_runtime_type(*this));
        }
    };

    PUBLIC_POINTER_CONTAINER(Method, ::REFrameworkMethodHandle)
        // .NET version
#ifdef __cplusplus_cli
        DotNetInvokeRet^ invoke(System::Object^ obj, array<System::Object^>^ args) {
            std::vector<void*> args2{};
            args2.resize(args->Length);

            for (int i = 0; i < args->Length; ++i) {
                //args2[i] = args[i]->ptr();
                const auto t = args[i]->GetType();

                if (t == System::Byte::typeid) {
                    uint8_t v = System::Convert::ToByte(args[i]);
                    args2[i] = (void*)(uint64_t)v;
                } else if (t == System::UInt16::typeid) {
                    uint16_t v = System::Convert::ToUInt16(args[i]);
                    args2[i] = (void*)(uint64_t)v;
                } else if (t == System::UInt32::typeid) {
                    uint32_t v = System::Convert::ToUInt32(args[i]);
                    args2[i] = (void*)(uint64_t)v;
                } else if (t == System::Single::typeid) {
                    float v = System::Convert::ToSingle(args[i]);
                    args2[i] = (void*)(uint64_t)v;
                } else if (t == System::UInt64::typeid) {
                    uint64_t v = System::Convert::ToUInt64(args[i]);
                    args2[i] = (void*)(uint64_t)v;
                } else if (t == System::Double::typeid) {
                    double v = System::Convert::ToDouble(args[i]);
                    args2[i] = (void*)(uint64_t)v;
                } else if (t == System::IntPtr::typeid) {
                    args2[i] = (void*)(uint64_t)System::Convert::ToInt64(args[i]);
                } else {
                    //args2[i] = args[i]->ptr();
                }
            }

            void* obj_ptr = nullptr;

            if (obj != nullptr) {
                const auto obj_t = obj->GetType();
                if (obj_t == System::IntPtr::typeid || obj_t == System::UIntPtr::typeid) {
                    obj_ptr = (void*)(uint64_t)System::Convert::ToInt64(obj);
                } else if (obj_t == API::ManagedObject::typeid) {
                    obj_ptr = ((API::ManagedObject^)obj)->ptr();
                } else {
                    //obj_ptr = obj->ptr();
                }
            }

            reframework::InvokeRet result;
            auto succeed = API::s_instance->sdk()->method->invoke(*this, obj_ptr, (void**)&args2[0], args2.size() * sizeof(void*), &result, sizeof(result));

            DotNetInvokeRet^ out = gcnew DotNetInvokeRet();
            out->ExceptionThrown = result.exception_thrown;

            if (result.exception_thrown) {
                return out;
            }

            out->Bytes = gcnew array<uint8_t>(result.bytes.size());
            System::Runtime::InteropServices::Marshal::Copy(System::IntPtr((void*)result.bytes.data()), out->Bytes, 0, result.bytes.size());

            out->Byte = result.byte;
            out->Word = result.word;
            out->DWord = result.dword;
            out->F = result.f;
            out->QWord = result.qword;
            out->D = result.d;
            out->Ptr = gcnew System::IntPtr(result.ptr);

            return out;
        }
#else
        reframework::InvokeRet invoke(API::ManagedObject CPP_POINTER obj, const std::vector<void*>& args) {
            reframework::InvokeRet out{};

            auto result = API::s_instance->sdk()->method->invoke(*this, obj->ptr(), (void**)&args[0], args.size() * sizeof(void*), &out, sizeof(out));

#ifdef REFRAMEWORK_API_EXCEPTIONS
            if (result != REFRAMEWORK_ERROR_NONE) {
                throw std::runtime_error("Method invocation failed");
            }
#endif

            return out;
        }
#endif

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
        T get_function() CPP_MEMBER_CONST {
            return (T)API::s_instance->sdk()->method->get_function(*this);
        }

        void* get_function_raw() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->method->get_function(*this);
        }

        // e.g. call<void*>(sdk->get_vm_context(), obj, args...);
        template<typename Ret = void*, typename ...Args>
        Ret call(Args... args) CPP_MEMBER_CONST {
            return get_function<Ret (*)(Args...)>()(args...);
        }

#ifdef __cplusplus_cli
        System::String^ get_name() {
            return gcnew System::String(API::s_instance->sdk()->method->get_name(*this));
        }
#else
        const char* get_name() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->method->get_name(*this);
        }
#endif

        API::TypeDefinition CPP_POINTER get_declaring_type() CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeDefinition, API::s_instance->sdk()->method->get_declaring_type(*this));
        }

        API::TypeDefinition CPP_POINTER get_return_type() CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeDefinition, API::s_instance->sdk()->method->get_return_type(*this));
        }

        uint32_t get_num_params() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->method->get_num_params(*this);
        }

#ifdef __cplusplus_cli
        ref struct DotNetMethodParameter {
            DotNetMethodParameter(const REFrameworkMethodParameter& p) { 
                Name = gcnew System::String(p.name);
                Type = gcnew API::TypeDefinition(p.t);  
            }

            property System::String^ Name;
            property API::TypeDefinition^ Type;
        };
#endif

#ifdef __cplusplus_cli
        System::Collections::Generic::List<DotNetMethodParameter^>^ get_params() {
#else
        std::vector<REFrameworkMethodParameter> get_params() CPP_MEMBER_CONST {
#endif
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

#ifdef __cplusplus_cli
            System::Collections::Generic::List<DotNetMethodParameter^>^ out = gcnew System::Collections::Generic::List<DotNetMethodParameter^>();

            for (auto& p : params) {
                out->Add(gcnew DotNetMethodParameter(p));
            }

            return out;
#else
            return params;
#endif
        }

        uint32_t get_index() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->method->get_index(*this);
        }

        int get_virtual_index() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->method->get_virtual_index(*this);
        }

        bool is_static() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->method->is_static(*this);
        }

        uint16_t get_flags() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->method->get_flags(*this);
        }

        uint16_t get_impl_flags() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->method->get_impl_flags(*this);
        }

        uint32_t get_invoke_id() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->method->get_invoke_id(*this);
        }

        unsigned int add_hook(REFPreHookFn pre_fn, REFPostHookFn post_fn, bool ignore_jmp) CPP_MEMBER_CONST {
            return API::s_instance->sdk()->functions->add_hook(*this, pre_fn, post_fn, ignore_jmp);
        }

        void remove_hook(unsigned int hook_id) CPP_MEMBER_CONST {
            API::s_instance->sdk()->functions->remove_hook(*this, hook_id);
        }
    };

    PUBLIC_POINTER_CONTAINER(Field, ::REFrameworkFieldHandle)
    #ifdef __cplusplus_cli
        System::String^ get_name() {
            return gcnew System::String(API::s_instance->sdk()->field->get_name(*this));
        }
    #else
        const char* get_name() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->field->get_name(*this);
        }
    #endif

        API::TypeDefinition CPP_POINTER get_declaring_type() CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeDefinition, API::s_instance->sdk()->field->get_declaring_type(*this));
        }

        API::TypeDefinition CPP_POINTER get_type() CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeDefinition, API::s_instance->sdk()->field->get_type(*this));
        }

        uint32_t get_offset_from_base() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->field->get_offset_from_base(*this);
        }

        uint32_t get_offset_from_fieldptr() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->field->get_offset_from_fieldptr(*this);
        }

        uint32_t get_flags() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->field->get_flags(*this);
        }

        bool is_static() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->field->is_static(*this);
        }

        bool is_literal() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->field->is_literal(*this);
        }

        void* get_init_data() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->field->get_init_data(*this);
        }

        void* get_data_raw(void* obj, bool is_value_type) CPP_MEMBER_CONST {
            return API::s_instance->sdk()->field->get_data_raw(*this, obj, is_value_type);
        }

        template <typename T> T& get_data(void* object, bool is_value_type) CPP_MEMBER_CONST { return *(T*)get_data_raw(object); }
    };

    PUBLIC_POINTER_CONTAINER(Property, ::REFrameworkPropertyHandle)
        // TODO: Implement
    };

    PUBLIC_POINTER_CONTAINER(ManagedObject, ::REFrameworkManagedObjectHandle)
        void add_ref() {
            API::s_instance->sdk()->managed_object->add_ref(*this);
        }

        void release() {
            API::s_instance->sdk()->managed_object->release(*this);
        }

        API::TypeDefinition CPP_POINTER get_type_definition() CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeDefinition, API::s_instance->sdk()->managed_object->get_type_definition(*this));
        }

        bool is_managed_object() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->managed_object->is_managed_object(this->ptr());
        }

        uint32_t get_ref_count() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->managed_object->get_ref_count(*this);
        }

        uint32_t get_vm_obj_type() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->managed_object->get_vm_obj_type(*this);
        }

        API::TypeInfo CPP_POINTER get_type_info() CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeInfo, API::s_instance->sdk()->managed_object->get_type_info(*this));
        }

        void* get_reflection_properties() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->managed_object->get_reflection_properties(*this);
        }

        API::ReflectionProperty CPP_POINTER get_reflection_property_descriptor(std::string_view name) {
            return REF_MAKE_POINTER(API::ReflectionProperty, API::s_instance->sdk()->managed_object->get_reflection_property_descriptor(*this, name.data()));
        }

        API::ReflectionMethod CPP_POINTER get_reflection_method_descriptor(std::string_view name) {
            return REF_MAKE_POINTER(API::ReflectionMethod, API::s_instance->sdk()->managed_object->get_reflection_method_descriptor(*this, name.data()));
        }

        template<typename Ret = void*, typename ...Args>
        Ret call(std::string_view method_name, Args... args) CPP_MEMBER_CONST {
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

#ifdef __cplusplus_cli
        // .NET version
        DotNetInvokeRet^ invoke(System::String^ method_name, array<System::Object^>^ args) {
            auto t = get_type_definition();

            if (t == nullptr) {
                return nullptr;
            }

            auto m = t->find_method(method_name);

            if (m == nullptr) {
                return nullptr;
            }

            return m->invoke(this, args);
        }
#else
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
#endif

        template<typename T>
        T* get_field(std::string_view name, bool is_value_type) CPP_MEMBER_CONST {
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

    PUBLIC_POINTER_CONTAINER(ResourceManager, REFrameworkResourceManagerHandle)
        API::Resource CPP_POINTER create_resource(std::string_view type_name, std::string_view name) {
            return REF_MAKE_POINTER(API::Resource, API::s_instance->sdk()->resource_manager->create_resource(*this, type_name.data(), name.data()));
        }
    };
    
    PUBLIC_POINTER_CONTAINER(Resource, ::REFrameworkResourceHandle)
        void add_ref() {
            API::s_instance->sdk()->resource->add_ref(*this);
        }

        void release() {
            API::s_instance->sdk()->resource->release(*this);
        }
    };

    PUBLIC_POINTER_CONTAINER(TypeInfo, ::REFrameworkTypeInfoHandle)
    #ifdef __cplusplus_cli
        System::String^ get_name() {
            return gcnew System::String(API::s_instance->sdk()->type_info->get_name(*this));
        }
    #else
        const char* get_name() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_info->get_name(*this);
        }
    #endif

        API::TypeDefinition CPP_POINTER get_type_definition() CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeDefinition, API::s_instance->sdk()->type_info->get_type_definition(*this));
        }

        bool is_clr_type() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_info->is_clr_type(*this);
        }

        bool is_singleton() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_info->is_singleton(*this);
        }

        void* get_singleton_instance() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_info->get_singleton_instance(*this);
        }

        void* get_reflection_properties() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_info->get_reflection_properties(*this);
        }

        API::ReflectionProperty CPP_POINTER get_reflection_property_descriptor(std::string_view name) {
            return REF_MAKE_POINTER(API::ReflectionProperty, API::s_instance->sdk()->type_info->get_reflection_property_descriptor(*this, name.data()));
        }

        API::ReflectionMethod CPP_POINTER get_reflection_method_descriptor(std::string_view name) {
            return REF_MAKE_POINTER(API::ReflectionMethod, API::s_instance->sdk()->type_info->get_reflection_method_descriptor(*this, name.data()));
        }

        void* get_deserializer_fn() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_info->get_deserializer_fn(*this);
        }

        API::TypeInfo CPP_POINTER get_parent() CPP_MEMBER_CONST {
            return REF_MAKE_POINTER(API::TypeInfo, API::s_instance->sdk()->type_info->get_parent(*this));
        }

        uint32_t get_crc() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->type_info->get_crc(*this);
        }
    };

    PUBLIC_POINTER_CONTAINER(VMContext, ::REFrameworkVMContextHandle)
        bool has_exception() CPP_MEMBER_CONST {
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

    PUBLIC_POINTER_CONTAINER(ReflectionMethod, ::REFrameworkReflectionMethodHandle)
        ::REFrameworkInvokeMethod get_function() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->reflection_method->get_function(*this);
        }
    };

    PUBLIC_POINTER_CONTAINER(ReflectionProperty, ::REFrameworkReflectionPropertyHandle)
        ::REFrameworkReflectionPropertyMethod get_getter() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->reflection_property->get_getter(*this);
        }

        bool is_static() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->reflection_property->is_static(*this);
        }

        uint32_t get_size() CPP_MEMBER_CONST {
            return API::s_instance->sdk()->reflection_property->get_size(*this);
        }
    };

private:
};
}

// ALWAYS call initialize first in reframework_plugin_initialize
#ifndef __cplusplus_cli
inline std::unique_ptr<reframework::API>& reframework::API::initialize(const REFrameworkPluginInitializeParam* param) {
#else
inline reframework::API^ reframework::API::initialize(uintptr_t param) {
#endif
#ifndef __cplusplus_cli
    if (param == nullptr) {
        throw std::runtime_error("param is null");
    }

    if (s_instance != nullptr) {
        return s_instance;
    }

    s_instance = std::make_unique<API>(param);
#else
    if (param == 0) {
        throw std::runtime_error("param is null");
    }

    if (s_instance != nullptr) {
        return s_instance;
    }

    s_instance = gcnew reframework::API(param);
#endif
    return s_instance;
}
