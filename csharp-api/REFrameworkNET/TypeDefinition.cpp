#include "TypeInfo.hpp"
#include "Method.hpp"
#include "Field.hpp"
#include "Property.hpp"
#include "ManagedObject.hpp"
#include "NativeObject.hpp"
#include "Proxy.hpp"

#include "TypeDefinition.hpp"

namespace REFrameworkNET {
    NativeObject^ TypeDefinition::Statics::get() {
        return gcnew NativeObject(this);
    }

    REFrameworkNET::Method^ TypeDefinition::FindMethod(System::String^ name)
    {
        auto result = m_type->find_method(msclr::interop::marshal_as<std::string>(name));

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew REFrameworkNET::Method(result);
    }

    REFrameworkNET::Field^ TypeDefinition::FindField(System::String^ name)
    {
        auto result = m_type->find_field(msclr::interop::marshal_as<std::string>(name));

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew Field(result);
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
        auto methods = m_type->get_methods();
        auto result = gcnew System::Collections::Generic::List<Method^>();

        for (auto& method : methods) {
            if (method == nullptr) {
                continue;
            }

            result->Add(gcnew Method(method));
        }

        return result;
    }

    System::Collections::Generic::List<REFrameworkNET::Field^>^ TypeDefinition::GetFields()
    {
        auto fields = m_type->get_fields();
        auto result = gcnew System::Collections::Generic::List<Field^>();

        for (auto& field : fields) {
            if (field == nullptr) {
                continue;
            }

            result->Add(gcnew Field(field));
        }

        return result;
    }

    System::Collections::Generic::List<REFrameworkNET::Property^>^ TypeDefinition::GetProperties()
    {
        auto properties = m_type->get_properties();
        auto result = gcnew System::Collections::Generic::List<Property^>();

        for (auto& property : properties) {
            if (property == nullptr) {
                continue;
            }

            result->Add(gcnew Property(property));
        }

        return result;
    }

    ManagedObject^ TypeDefinition::CreateInstance(int32_t flags)
    {
        auto result = m_type->create_instance(flags);

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew ManagedObject(result);
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
        auto result = m_type->get_runtime_type();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew ManagedObject(result);
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
}