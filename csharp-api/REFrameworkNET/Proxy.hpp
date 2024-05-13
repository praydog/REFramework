#pragma once

#include <cstdint>

#include "./API.hpp"
#include "ManagedObject.hpp"
#include "TypeDefinition.hpp"
#include "Attributes/Method.hpp"

using namespace System;

// Classes for proxying interfaces from auto generated assemblies to our dynamic types
namespace REFrameworkNET {
// Inherit IObject's interface as well
// so we can easily pretend that this is an IObject, even though it just forwards the calls to the actual object
public interface class IProxy : IObject {
    IObject^ GetInstance();
    void SetInstance(IObject^ instance);
};

generic <typename T, typename T2>
where T2 : ref class
public ref class Proxy : public Reflection::DispatchProxy, public IProxy, public System::IEquatable<Proxy<T, T2>^>
{
public:
    virtual REFrameworkNET::TypeDefinition^ GetTypeDefinition() {
        return Instance->GetTypeDefinition();
    }

    virtual void* Ptr() {
        return Instance->Ptr();
    }

    virtual uintptr_t GetAddress() {
        return Instance->GetAddress();
    }

    virtual bool IsProxy() {
        return true;
    }

    virtual REFrameworkNET::InvokeRet^ Invoke(System::String^ methodName, array<System::Object^>^ args) {
        return Instance->Invoke(methodName, args);
    }

    virtual bool HandleInvokeMember_Internal(uint32_t methodIndex, array<System::Object^>^ args, System::Object^% result) {
        return Instance->HandleInvokeMember_Internal(methodIndex, args, result);
    }

    virtual bool HandleInvokeMember_Internal(System::String^ methodName, array<System::Object^>^ args, System::Object^% result) {
        return Instance->HandleInvokeMember_Internal(methodName, args, result);
    }

    virtual bool HandleInvokeMember_Internal(System::Object^ methodObj, array<System::Object^>^ args, System::Object^% result) {
        return Instance->HandleInvokeMember_Internal(methodObj, args, result);
    }

    virtual bool HandleTryGetMember_Internal(System::String^ fieldName, System::Object^% result) {
        return Instance->HandleTryGetMember_Internal(fieldName, result);
    }

    virtual System::Object^ GetField(System::String^ fieldName) {
        return Instance->GetField(fieldName);
    }
    
    virtual System::Object^ Call(System::String^ methodName, ... array<System::Object^>^ args) {
        return Instance->Call(methodName, args);
    }


    // For interface types
    generic <typename T>
    virtual T As() {
        return Instance->As<T>();
    }

    virtual IProxy^ GetProxy(System::Type^ type) {
        return Instance->GetProxy(type);
    }
    
    virtual bool IsManaged() {
        return Instance->IsManaged();
    }

    static T Create(IObject^ target) {
        auto proxy = Reflection::DispatchProxy::Create<T, Proxy<T, T2>^>();
        ((IProxy^)proxy)->SetInstance(target);
        return proxy;
    }

    virtual IObject^ GetInstance() {
        return Instance;
    }

    virtual void SetInstance(IObject^ instance) {
        Instance = instance;
    }

    virtual bool Equals(Proxy<T, T2>^ other) {
        return Instance == other->Instance;
    }

    bool Equals(Object^ obj) override {
        if (obj == nullptr) {
            return false;
        }

        auto other = dynamic_cast<Proxy<T, T2>^>(obj);

        if (other == nullptr) {
            return false;
        }

        return Equals(other);
    }

    virtual bool Equals(IObject^ other) {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        return Ptr() == other->Ptr();
    }

    int GetHashCode() override {
        return Instance->GetHashCode();
    }

    static bool operator ==(Proxy<T, T2>^ left, Proxy<T, T2>^ right) {
        return left->Equals(right);
    }

    static bool operator !=(Proxy<T, T2>^ left, Proxy<T, T2>^ right) {
        return !left->Equals(right);
    }

internal:
    ~Proxy() {
        this->!Proxy();
    }

    !Proxy() {

    }

protected:
    virtual Object^ Invoke(Reflection::MethodInfo^ targetMethod, array<Object^>^ args) override {
        // Get the REFrameworkNET::Attributes::Method attribute from the method
        auto methodAttribute = REFrameworkNET::Attributes::MethodAttribute::GetCachedAttribute(targetMethod);

        Object^ result = nullptr;
        auto iobject = static_cast<REFrameworkNET::IObject^>(Instance);

        if (methodAttribute != nullptr) {
            if (iobject != nullptr) {
                auto underlyingObject = methodAttribute->GetUnderlyingObject(iobject->GetTypeDefinition());

                // Property getter/setters for fields
                if (methodAttribute->IsField) {
                    auto field = (REFrameworkNET::Field^)underlyingObject;
                    bool isValueType = !IsManaged() && GetTypeDefinition()->IsValueType();

                    if (methodAttribute->FieldFacade == REFrameworkNET::FieldFacadeType::Getter) {
                        return field->GetDataBoxed(iobject->GetAddress(), isValueType);
                    }

                    field->SetDataBoxed(iobject->GetAddress(), args[0], isValueType);
                    return nullptr;
                }

                auto method = (REFrameworkNET::Method^)underlyingObject;
                return method->InvokeBoxed(targetMethod->ReturnType, iobject, args);
            } else {
                throw gcnew System::InvalidOperationException("Proxy: T2 must be IObject derived");
            }
        } else {
            // This is a fallback
            if (iobject != nullptr) {
                auto method = iobject->GetTypeDefinition()->GetMethod(targetMethod->Name);

                if (method != nullptr) {
                    return method->InvokeBoxed(targetMethod->ReturnType, iobject, args);
                }

                throw gcnew System::InvalidOperationException("Proxy: Method not found");
            } else {
                throw gcnew System::InvalidOperationException("Proxy: T2 must be IObject derived");
            }
        }

        return nullptr;
    }

private:
    property IObject^ Instance;
};

generic <typename T>
public ref class ManagedProxy : public Proxy<T, REFrameworkNET::ManagedObject^> {
public:
    static T Create(Object^ target) {
        return Proxy<T, REFrameworkNET::ManagedObject^>::Create(dynamic_cast<IObject^>(target));
    }
    static T CreateFromSingleton(System::String^ singletonName) {
        return Proxy<T, REFrameworkNET::ManagedObject^>::Create(REFrameworkNET::API::GetManagedSingleton(singletonName));
    }
};

generic <typename T>
public ref class NativeProxy : public Proxy<T, REFrameworkNET::NativeObject^> {
public:
    static T Create(Object^ target) {
        return Proxy<T, REFrameworkNET::NativeObject^>::Create(dynamic_cast<IObject^>(target));
    }
    static T CreateFromSingleton(System::String^ singletonName) {
        return Proxy<T, REFrameworkNET::NativeObject^>::Create(REFrameworkNET::API::GetNativeSingleton(singletonName));
    }
};

generic <typename T>
public ref class AnyProxy : public Proxy<T, REFrameworkNET::IObject^> {
public:
    static T Create(Object^ target) {
        return Proxy<T, REFrameworkNET::IObject^>::Create(dynamic_cast<IObject^>(target));
    }
};
}