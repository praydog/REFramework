#include <msclr/marshal_cppstd.h>

#include "TypeInfo.hpp"
#include "Method.hpp"
#include "Field.hpp"
#include "Property.hpp"
#include "ManagedObject.hpp"
#include "NativeObject.hpp"
#include "Proxy.hpp"
#include "Utility.hpp"

#include "TypeDefinition.hpp"

namespace REFrameworkNET {
    NativeObject^ TypeDefinition::Statics::get() {
        return gcnew NativeObject(this);
    }

    size_t TypeDefinition::GetFNV64Hash() {
        if (m_cachedFNV64Hash == 0) {
            m_lock->EnterWriteLock();
            try {
                if (m_cachedFNV64Hash == 0) {
                    m_cachedFNV64Hash = REFrameworkNET::hash(m_type->get_full_name().c_str());
                }
            } finally {
                m_lock->ExitWriteLock();
            }
        }

        return m_cachedFNV64Hash;
    }

    REFrameworkNET::Method^ TypeDefinition::FindMethod(System::String^ name)
    {
        auto result = m_type->find_method(msclr::interop::marshal_as<std::string>(name));

        if (result == nullptr) {
            return nullptr;
        }

        return REFrameworkNET::Method::GetInstance(result);
    }

    REFrameworkNET::Field^ TypeDefinition::FindField(System::String^ name)
    {
        auto result = m_type->find_field(msclr::interop::marshal_as<std::string>(name));

        if (result == nullptr) {
            return nullptr;
        }

        return REFrameworkNET::Field::GetInstance(result);
    }

    REFrameworkNET::Property^ TypeDefinition::FindProperty(System::String^ name)
    {
        auto result = m_type->find_property(msclr::interop::marshal_as<std::string>(name));

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew Property(result);
    }

    System::Collections::Generic::List<REFrameworkNET::Method^>^ TypeDefinition::GetMethods()
    {
        if (m_methods == nullptr) {
            m_lock->EnterWriteLock();
            try {
                if (m_methods == nullptr) {
                    auto methods = m_type->get_methods();
                    m_methods = gcnew System::Collections::Generic::List<Method^>();

                    for (auto& method : methods) {
                        if (method == nullptr) {
                            continue;
                        }

                        m_methods->Add(Method::GetInstance(method));
                    }
                }
            } finally {
                m_lock->ExitWriteLock();
            }
        }

        return m_methods;
    }

    System::Collections::Generic::List<REFrameworkNET::Field^>^ TypeDefinition::GetFields()
    {
        if (m_fields == nullptr) {
            m_lock->EnterWriteLock();

            try {
                if (m_fields == nullptr ){
                    auto fields = m_type->get_fields();
                    m_fields = gcnew System::Collections::Generic::List<Field^>();

                    for (auto& field : fields) {
                        if (field == nullptr) {
                            continue;
                        }

                        m_fields->Add(Field::GetInstance(field));
                    }
                }
            } finally {
                m_lock->ExitWriteLock();
            }
        }
        
        return m_fields;
    }

    System::Collections::Generic::List<REFrameworkNET::Property^>^ TypeDefinition::GetProperties()
    {
        if (m_properties == nullptr) {
            m_lock->EnterWriteLock();

            try {
                auto properties = m_type->get_properties();
                m_properties = gcnew System::Collections::Generic::List<Property^>();

                for (auto& property : properties) {
                    if (property == nullptr) {
                        continue;
                    }

                    m_properties->Add(gcnew Property(property));
                }
            } finally {
                m_lock->ExitWriteLock();
            }
        }

        return m_properties;
    }

    ManagedObject^ TypeDefinition::CreateInstance(int32_t flags)
    {
        auto result = m_type->create_instance(flags);

        if (result == nullptr) {
            return nullptr;
        }

        return ManagedObject::Get(result);
    }

    REFrameworkNET::TypeInfo^ TypeDefinition::GetTypeInfo()
    {
        auto result = m_type->get_type_info();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew REFrameworkNET::TypeInfo(result);
    }

    ManagedObject^ TypeDefinition::GetRuntimeType()
    {
        if (m_runtimeType == nullptr) {
            m_lock->EnterWriteLock();

            try {
                if (m_runtimeType == nullptr) {
                    auto result = m_type->get_runtime_type();

                    if (result != nullptr) {
                        m_runtimeType = ManagedObject::Get(result);
                    }
                }
            } finally {
                m_lock->ExitWriteLock();
            }
        }

        return m_runtimeType;
    }

    REFrameworkNET::InvokeRet^ TypeDefinition::Invoke(System::String^ methodName, array<System::Object^>^ args) {
        // Forward this onto NativeObject.Invoke (for static methods)
        auto native = gcnew NativeObject(this);
        return native->Invoke(methodName, args);
    }

    bool TypeDefinition::HandleInvokeMember_Internal(System::String^ methodName, array<System::Object^>^ args, System::Object^% result) {
        // Forward this onto NativeObject.HandleInvokeMember_Internal (for static methods)
        auto native = gcnew NativeObject(this);
        return native->HandleInvokeMember_Internal(methodName, args, result);
    }

    bool TypeDefinition::TryInvokeMember(System::Dynamic::InvokeMemberBinder^ binder, array<System::Object^>^ args, System::Object^% result) {
        // Forward this onto NativeObject.TryInvokeMember (for static methods)
        auto native = gcnew NativeObject(this);
        return native->TryInvokeMember(binder, args, result);
    }

    generic <typename T>
    T TypeDefinition::As() {
        return NativeProxy<T>::Create(gcnew NativeObject(this));
    }

    bool TypeDefinition::HasAttribute(REFrameworkNET::ManagedObject^ runtimeAttribute, bool inherit) {
        if (runtimeAttribute == nullptr) {
            return false;
        }

        auto runtimeType = this->GetRuntimeType();

        if (runtimeType == nullptr) {
            return false;
        }

        auto attributes = (ManagedObject^)runtimeType->Call("GetCustomAttributes(System.Type, System.Boolean)", runtimeAttribute, inherit);

        if (attributes == nullptr) {
            return false;
        }

        return (int)attributes->Call("GetLength", gcnew System::Int32(0)) > 0;
    }

    bool TypeDefinition::IsGenericTypeDefinition() {
        auto runtimeType = this->GetRuntimeType();

        if (runtimeType == nullptr) {
            return false;
        }

        return (bool)runtimeType->Call("get_IsGenericTypeDefinition");
    }

    bool TypeDefinition::IsGenericType() {
        auto runtimeType = this->GetRuntimeType();

        if (runtimeType == nullptr) {
            return false;
        }

        return (bool)runtimeType->Call("get_IsGenericType");
    }

    array<TypeDefinition^>^ TypeDefinition::GetGenericArguments() {
        auto runtimeType = this->GetRuntimeType();

        if (runtimeType == nullptr) {
            return nullptr;
        }

        auto arguments = (ManagedObject^)runtimeType->Call("GetGenericArguments");

        if (arguments == nullptr) {
            return nullptr;
        }

        auto result = gcnew array<TypeDefinition^>((int)arguments->Call("get_Length", gcnew System::Int32(0)));

        for (int i = 0; i < result->Length; i++) {
            auto runtimeType = (ManagedObject^)arguments->Call("get_Item", gcnew System::Int32(i));
            result[i] = (TypeDefinition^)runtimeType->Call("get_TypeHandle");
        }

        return result;
    }

    TypeDefinition^ TypeDefinition::GetElementType() {
        if (m_elementType == nullptr) {
            m_lock->EnterWriteLock();

            try {
                auto runtimeType = this->GetRuntimeType();

                if (runtimeType == nullptr) {
                    return nullptr;
                }

                auto elementType = (ManagedObject^)runtimeType->Call("GetElementType");

                if (elementType == nullptr) {
                    return nullptr;
                }

                return (TypeDefinition^)elementType->Call("get_TypeHandle");
            } finally {
                m_lock->ExitWriteLock();
            }
        }

        return m_elementType;
    }
}