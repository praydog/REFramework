#pragma once

#include <reframework/API.hpp>

#pragma managed

#include "InvokeRet.hpp"
#include "MethodParameter.hpp"
#include "NativePool.hpp"

namespace REFrameworkNET {
ref class MethodHook;

public enum class PreHookResult : int32_t {
    Continue = 0,
    Skip = 1,
};

/// <summary>
/// Represents a method in the RE Engine's IL2CPP metadata.
/// Equivalent to System.RuntimeMethodHandle in .NET.
/// </summary>
/// <remarks>
/// </remarks>
public ref class Method : public System::IEquatable<Method^>
{
public:
    static Method^ GetInstance(reframework::API::Method* m) {
        return NativePool<Method>::GetInstance((uintptr_t)m, s_createFromPointer);
    }

    static Method^ GetInstance(::REFrameworkMethodHandle handle) {
        return NativePool<Method>::GetInstance((uintptr_t)handle, s_createFromPointer);
    }

private:
    Method(reframework::API::Method* method) : m_method(method) {}
    Method(::REFrameworkMethodHandle handle) : m_method(reinterpret_cast<reframework::API::Method*>(handle)) {}

public:
    static Method^ createFromPointer(uintptr_t ptr) {
        return gcnew Method((reframework::API::Method*)ptr);
    }

    static NativePool<Method>::CreatorDelegate^ s_createFromPointer = gcnew NativePool<Method>::CreatorDelegate(createFromPointer);


    void* GetRaw() {
        return m_method;
    }

    /// <summary>
    /// Invokes this method with the given arguments.
    /// </summary>
    /// <param name="obj">The object to invoke the method on. Null for static methods.</param>
    /// <param name="args">The arguments to pass to the method.</param>
    /// <returns>The return value of the method. 128 bytes in size internally.</returns>
    /// <remarks>
    /// Generally should not be used unless you know what you're doing.
    /// Use the other invoke method to automatically convert the return value correctly into a usable object.
    /// </remarks>
    REFrameworkNET::InvokeRet Invoke(System::Object^ obj, array<System::Object^>^ args);

private:
    ::reframework::InvokeRet Invoke_Internal(System::Object^ obj, array<System::Object^>^ args);

public:

    /// <summary>
    /// Invokes this method with the given arguments.
    /// </summary>
    /// <param name="obj">The object to invoke the method on. Null for static methods.</param>
    /// <param name="args">The arguments to pass to the method.</param>
    /// <param name="result">The return value of the method. REFramework will attempt to convert this into a usable object.</param>
    /// <returns>True if the method was successfully found and invoked, false otherwise.</returns>
    bool HandleInvokeMember_Internal(System::Object^ obj, array<System::Object^>^ args, System::Object^% result);

    void* GetFunctionPtr() {
        return m_method->get_function_raw();
    }

    /// <returns>The name of the method.</returns>
    System::String^ GetName() {
        return gcnew System::String(m_method->get_name());
    }

    property System::String^ Name {
        System::String^ get() {
            return GetName();
        }
    }

    /// <returns>The declaring <see cref="TypeDefinition"/> of the method.</returns>
    TypeDefinition^ GetDeclaringType() {
        auto result = m_method->get_declaring_type();

        if (result == nullptr) {
            return nullptr;
        }

        return TypeDefinition::GetInstance(result);
    }

    property TypeDefinition^ DeclaringType {
        TypeDefinition^ get() {
            return GetDeclaringType();
        }
    }

    /// <returns>The return <see cref="TypeDefinition"/> of the method.</returns>
    TypeDefinition^ GetReturnType() {
        if (m_returnType == nullptr) {
            m_lock->EnterWriteLock();

            try {
                if (m_returnType == nullptr) {
                    auto result = m_method->get_return_type();

                    if (result == nullptr) {
                        return nullptr;
                    }

                    m_returnType = TypeDefinition::GetInstance(result);
                }
            } finally {
                m_lock->ExitWriteLock();
            }
        }

        return m_returnType;
    }

    property TypeDefinition^ ReturnType {
        TypeDefinition^ get() {
            return GetReturnType();
        }
    }

    /// <returns>The number of parameters of the method.</returns>
    uint32_t GetNumParams() {
        return m_method->get_num_params();
    }

    property uint32_t NumParams {
        uint32_t get() {
            return GetNumParams();
        }
    }

    /// <returns>The parameters of the method.</returns>
    /// <remarks><see cref="MethodParameter"/></remarks>
    System::Collections::Generic::List<REFrameworkNET::MethodParameter^>^ GetParameters() {
        const auto params = m_method->get_params();

        auto ret = gcnew System::Collections::Generic::List<REFrameworkNET::MethodParameter^>();

        for (auto& p : params) {
            ret->Add(gcnew REFrameworkNET::MethodParameter(p));
        }

        return ret;
    }

    property System::Collections::Generic::List<REFrameworkNET::MethodParameter^>^ Parameters {
        System::Collections::Generic::List<REFrameworkNET::MethodParameter^>^ get() {
            return GetParameters();
        }
    }

    uint32_t GetIndex() {
        return m_method->get_index();
    }

    property uint32_t Index {
        uint32_t get() {
            return GetIndex();
        }
    }

    int32_t GetVirtualIndex() {
        return m_method->get_virtual_index();
    }

    property int32_t VirtualIndex {
        int32_t get() {
            return GetVirtualIndex();
        }
    }

    bool IsVirtual() {
        return VirtualIndex >= 0;
    }

    bool IsStatic() {
        return m_method->is_static();
    }

    uint16_t GetFlags() {
        return m_method->get_flags();
    }

    property uint16_t Flags {
        uint16_t get() {
            return GetFlags();
        }
    }

    uint16_t GetImplFlags() {
        return m_method->get_impl_flags();
    }

    property uint16_t ImplFlags {
        uint16_t get() {
            return GetImplFlags();
        }
    }

    /// <summary>
    /// Gets the invoke ID of this method. This is the index into the global invoke wrapper table.
    /// </summary>
    /// <returns>The invoke ID.</returns>
    uint32_t GetInvokeID() {
        return m_method->get_invoke_id();
    }

    property uint32_t InvokeID {
        uint32_t get() {
            return GetInvokeID();
        }
    }
    
    /// <summary>
    /// Adds a hook to this method. Additional operations can be performed on the returned hook object.
    /// </summary>
    /// <param name="ignore_jmp">If true, the hook will not look for a nearby jmp to locate the "real" function.</param>
    /// <returns>The hook object.</returns>
    MethodHook^ AddHook(bool ignore_jmp);

    /// <returns>The System.MethodInfo object for this method. Not cached.</returns>
    ManagedObject^ GetRuntimeMethod();


    System::Collections::Generic::List<Method^>^ GetMatchingParentMethods(); // mainly for the assembly generator (temporary?)
    bool IsOverride();

    /// <returns>The method signature as a string in the form of "MethodName(Type1, Type2, ...)"</returns>
    System::String^ GetMethodSignature() {
        auto name = GetName();

        if (GetNumParams() == 0) {
            return name + "()";
        }

        auto params = GetParameters();

        System::String^ ret = name + "(";

        for (int i = 0; i < params->Count; i++) {
            ret += params[i]->Type->FullName;

            if (i < params->Count - 1) {
                ret += ", ";
            }
        }

        ret += ")";

        return ret;
    }

public:
    virtual bool Equals(System::Object^ other) override {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        if (other->GetType() != Method::typeid) {
            return false;
        }

        return m_method == safe_cast<Method^>(other)->m_method;
    }

    virtual bool Equals(Method^ other) {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        return m_method == other->m_method;
    }

    static bool operator ==(Method^ left, Method^ right) {
        if (System::Object::ReferenceEquals(left, right)) {
            return true;
        }

        if (System::Object::ReferenceEquals(left, nullptr) || System::Object::ReferenceEquals(right, nullptr)) {
            return false;
        }

        return left->m_method == right->m_method;
    }

    static bool operator !=(Method^ left, Method^ right) {
        return !(left == right);
    }

    virtual int GetHashCode() override {
        return (gcnew System::UIntPtr((uintptr_t)m_method))->GetHashCode();
    }

private:
    reframework::API::Method* m_method;

    System::Threading::ReaderWriterLockSlim^ m_lock{gcnew System::Threading::ReaderWriterLockSlim()};
    TypeDefinition^ m_returnType{nullptr};
};
}