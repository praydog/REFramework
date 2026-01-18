#pragma once

#include <REFramework/API.hpp>

#include "NativePool.hpp"

#pragma managed 

namespace REFrameworkNET {
public ref class Module : public System::IEquatable<Module^>
{
public:
    static Module^ GetInstance(reframework::API::Module* fd) {
        return NativePool<Module>::GetInstance((uintptr_t)fd, s_createFromPointer);
    }

    static Module^ GetInstance(::REFrameworkModuleHandle handle) {
        return NativePool<Module>::GetInstance((uintptr_t)handle, s_createFromPointer);
    }

private:
    static Module^ createFromPointer(uintptr_t ptr) {
        return gcnew Module((reframework::API::Module*)ptr);
    }

    static NativePool<Module>::CreatorDelegate^ s_createFromPointer = gcnew NativePool<Module>::CreatorDelegate(createFromPointer);

private:
    Module(const reframework::API::Module* module) : m_module(module) {}
    Module(::REFrameworkModuleHandle* handle) : m_module(reinterpret_cast<const reframework::API::Module*>(handle)) {}

public:
    uint32_t GetMajor() {
        return m_module->get_major();
    }
    
    uint32_t GetMinor() {
        return m_module->get_minor();
    }

    uint32_t GetRevision() {
        return m_module->get_revision();
    }

    uint32_t GetBuild() {
        return m_module->get_build();
    }

    System::String^ GetAssemblyName() {
        return gcnew System::String(m_module->get_assembly_name());
    }

    System::String^ GetLocation() {
        return gcnew System::String(m_module->get_location());
    }

    System::String^ GetModuleName() {
        return gcnew System::String(m_module->get_module_name());
    }

    System::Span<uint32_t> GetTypes() {
        const auto cpp_span = m_module->get_types();
        return System::Span<uint32_t>(cpp_span.data(), cpp_span.size());
    }

    property System::Span<uint32_t> Types {
        System::Span<uint32_t> get() {
            return GetTypes();
        }
    }

    property uint32_t Major {
        uint32_t get() {
            return GetMajor();
        }
    }

    property uint32_t Minor {
        uint32_t get() {
            return GetMinor();
        }
    }

    property uint32_t Revision {
        uint32_t get() {
            return GetRevision();
        }
    }

    property uint32_t Build {
        uint32_t get() {
            return GetBuild();
        }
    }

    property System::String^ AssemblyName {
        System::String^ get() {
            return GetAssemblyName();
        }
    }

    property System::String^ Location {
        System::String^ get() {
            return GetLocation();
        }
    }

    property System::String^ ModuleName {
        System::String^ get() {
            return GetModuleName();
        }
    }

    virtual bool Equals(System::Object^ other) override {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        if (other->GetType() != Module::typeid) {
            return false;
        }

        return m_module == safe_cast<Module^>(other)->m_module;
    }

    virtual bool Equals(Module^ other) {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        return m_module == other->m_module;
    }

    static bool operator ==(Module^ left, Module^ right) {
        if (System::Object::ReferenceEquals(left, right)) {
            return true;
        }

        if (System::Object::ReferenceEquals(left, nullptr) || System::Object::ReferenceEquals(right, nullptr)) {
            return false;
        }

        return left->m_module == right->m_module;
    }

    static bool operator !=(Module^ left, Module^ right) {
        return !(left == right);
    }

    virtual int GetHashCode() override {
        return (gcnew System::UIntPtr((uintptr_t)m_module))->GetHashCode();
    }

private:
    const reframework::API::Module* m_module;
};
}