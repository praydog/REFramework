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

protected:
    virtual Object^ Invoke(Reflection::MethodInfo^ targetMethod, array<Object^>^ args) override {
        // Get the REFrameworkNET::Attributes::Method attribute from the method
        //auto methodAttributes = targetMethod->GetCustomAttributes(REFrameworkNET::Attributes::Method::typeid, false);
        auto methodAttribute = (REFrameworkNET::Attributes::Method^)System::Attribute::GetCustomAttribute(targetMethod, REFrameworkNET::Attributes::Method::typeid);

        Object^ result = nullptr;
        auto iobject = dynamic_cast<REFrameworkNET::IObject^>(Instance);

        if (methodAttribute != nullptr) {
            auto method = methodAttribute->GetMethod();

            if (iobject != nullptr) {
                iobject->HandleInvokeMember_Internal(method, args, result);
            } else {
                throw gcnew System::InvalidOperationException("Proxy: T2 must be IObject derived");
            }
        } else {
            // This is a fallback
            if (iobject != nullptr) {
                iobject->HandleInvokeMember_Internal(targetMethod->Name, args, result);
            } else {
                throw gcnew System::InvalidOperationException("Proxy: T2 must be IObject derived");
            }
        }

        auto targetReturnType = targetMethod->ReturnType;

        if (targetReturnType == nullptr) {
            return result;
        }

        if (!targetReturnType->IsPrimitive && !targetReturnType->IsEnum) {
            if (targetReturnType == String::typeid) {
                return result;
            }

            if (targetReturnType == REFrameworkNET::ManagedObject::typeid) {
                return result;
            }
            
            if (targetReturnType == REFrameworkNET::NativeObject::typeid) {
                return result;
            }

            if (targetReturnType == REFrameworkNET::TypeDefinition::typeid) {
                return result;
            }

            if (targetReturnType == REFrameworkNET::Method::typeid) {
                return result;
            }

            if (targetReturnType == REFrameworkNET::Field::typeid) {
                return result;
            }
        }

        if (targetReturnType->IsEnum) {
            auto underlyingType = targetReturnType->GetEnumUnderlyingType();

            if (underlyingType != nullptr) {
                auto underlyingResult = Convert::ChangeType(result, underlyingType);
                return Enum::ToObject(targetReturnType, underlyingResult);
            }
        }

        if (targetMethod->DeclaringType == nullptr) {
            return result;
        }

        if (!targetReturnType->IsPrimitive && targetMethod->DeclaringType->IsInterface && result != nullptr) {
            auto iobjectResult = dynamic_cast<REFrameworkNET::IObject^>(result);

            if (iobjectResult != nullptr && targetReturnType->IsInterface) {
                auto proxy = DispatchProxy::Create(targetReturnType, Proxy<T, T2>::typeid->GetGenericTypeDefinition()->MakeGenericType(T::typeid, result->GetType()));
                ((IProxy^)proxy)->SetInstance(iobjectResult);
                result = proxy;
                return result;
            }
        }

        return result;
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