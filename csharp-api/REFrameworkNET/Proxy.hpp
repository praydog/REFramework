#pragma once

#include <cstdint>

#include "./API.hpp"
#include "ManagedObject.hpp"
#include "TypeDefinition.hpp"

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

    virtual REFrameworkNET::InvokeRet^ Invoke(System::String^ methodName, array<System::Object^>^ args) {
        return Instance->Invoke(methodName, args);
    }

    virtual bool HandleInvokeMember_Internal(System::String^ methodName, array<System::Object^>^ args, System::Object^% result) {
        return Instance->HandleInvokeMember_Internal(methodName, args, result);
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
        Object^ result = nullptr;
        auto iobject = dynamic_cast<REFrameworkNET::IObject^>(Instance);

        // how am i gonna handle static methods?
        if (iobject != nullptr) {
            iobject->HandleInvokeMember_Internal(targetMethod->Name, args, result);
        } else {
            throw gcnew System::InvalidOperationException("Proxy: T2 must be IObject derived");
        }

        if (targetMethod->ReturnType == REFrameworkNET::ManagedObject::typeid) {
            return result;
        }
        
        if (targetMethod->ReturnType == REFrameworkNET::NativeObject::typeid) {
            return result;
        }

        if (targetMethod->ReturnType == String::typeid) {
            return result;
        }

        if (targetMethod->DeclaringType == nullptr) {
            return result;
        }

        if (!targetMethod->ReturnType->IsPrimitive && targetMethod->DeclaringType->IsInterface && result != nullptr) {
            auto iobjectResult = dynamic_cast<REFrameworkNET::IObject^>(result);

            if (iobjectResult != nullptr) {
                auto t = iobjectResult->GetTypeDefinition();
                auto fullName = t->FullName;
                auto localType = T::typeid->Assembly->GetType(fullName);

                if (localType != nullptr) {
                    auto proxy = DispatchProxy::Create(localType, Proxy<T, T2>::typeid->GetGenericTypeDefinition()->MakeGenericType(T::typeid, result->GetType()));
                    ((IProxy^)proxy)->SetInstance(iobjectResult);
                    result = proxy;
                    return result;
                } else {
                    System::Console::WriteLine("Type not found: " + fullName);
                }
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