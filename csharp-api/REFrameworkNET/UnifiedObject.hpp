#pragma once

#include "IObject.hpp"
#include "ObjectEnumerator.hpp"

namespace REFrameworkNET {
ref class TypeDefinition;
interface class IProxy;
value struct InvokeRet;

// UnifiedObject is the base class that ManagedObject and NativeObject will derive from
// It will have several shared methods but some unimplemented methods that will be implemented in the derived classes
public ref class UnifiedObject abstract : public System::Dynamic::DynamicObject, public System::Collections::IEnumerable, public REFrameworkNET::IObject {
public:
    virtual bool IsProxy() {
        return false;
    }

    // These methods will be implemented in the derived classes
    virtual void* Ptr() abstract = 0;
    virtual uintptr_t GetAddress() abstract = 0;
    virtual REFrameworkNET::TypeDefinition^ GetTypeDefinition() abstract = 0;

    generic <typename T>
    virtual T As() abstract = 0;

    virtual IProxy^ GetProxy(System::Type^ type) {
        return ProxyPool::GetIfExists(this, type);
    }

    virtual bool IsManaged() {
        return false;
    }

    virtual bool HandleInvokeMember_Internal(System::String^ methodName, array<System::Object^>^ args, System::Object^% result);
    virtual bool HandleInvokeMember_Internal(uint32_t methodIndex, array<System::Object^>^ args, System::Object^% result);
    virtual bool HandleInvokeMember_Internal(System::Object^ methodObj, array<System::Object^>^ args, System::Object^% result);
    
    virtual bool HandleTryGetMember_Internal(System::String^ fieldName, System::Object^% result);

    virtual REFrameworkNET::InvokeRet^ Invoke(System::String^ methodName, array<System::Object^>^ args);
    virtual bool TryInvokeMember(System::Dynamic::InvokeMemberBinder^ binder, array<System::Object^>^ args, System::Object^% result) override;
    virtual bool TryGetMember(System::Dynamic::GetMemberBinder^ binder, System::Object^% result) override;
    virtual bool TrySetMember(System::Dynamic::SetMemberBinder^ binder, System::Object^ value) override;
    
    virtual System::Object^ Call(System::String^ methodName, ... array<System::Object^>^ args) {
        System::Object^ result = nullptr;
        HandleInvokeMember_Internal(methodName, args, result);

        return result;
    }

    virtual System::Object^ GetField(System::String^ fieldName) {
        System::Object^ result = nullptr;
        if (!HandleTryGetMember_Internal(fieldName, result)) {
            if (!HandleInvokeMember_Internal("get_" + fieldName, gcnew array<System::Object^>{}, result)) {
                // TODO? what else can we do here?
            }
        }

        return result;
    }

public:
    virtual bool Equals(System::Object^ other) override {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        if (!other->GetType()->IsSubclassOf(IObject::typeid)) {
            return false;
        }

        return Ptr() == safe_cast<IObject^>(other)->Ptr();
    }

    // Override equality operator
    virtual bool Equals(IObject^ other) {
        if (System::Object::ReferenceEquals(this, other)) {
            return true;
        }

        if (System::Object::ReferenceEquals(other, nullptr)) {
            return false;
        }

        return Ptr() == other->Ptr();
    }

    static bool operator ==(UnifiedObject^ left, UnifiedObject^ right) {
        if (System::Object::ReferenceEquals(left, right)) {
            return true;
        }

        if (System::Object::ReferenceEquals(left, nullptr) || System::Object::ReferenceEquals(right, nullptr)) {
            return false;
        }

        return left->Ptr() == right->Ptr();
    }

    static bool operator !=(UnifiedObject^ left, UnifiedObject^ right) {
        return !(left == right);
    }

public:
    // IEnumerable implementation
    virtual System::Collections::IEnumerator^ GetEnumerator() {
        return gcnew REFrameworkNET::ObjectEnumerator(this);
    }

internal:
    using WeakProxy = System::WeakReference<IProxy^>;

    generic <typename T>
    T AsCached() {
        auto strongProxy = GetProxy(T::typeid);

        if (strongProxy != nullptr) {
            return (T)strongProxy;
        }

        T instance = As<T>();
        //m_proxies->TryAdd(T::typeid, gcnew WeakProxy((IProxy^)instance));
        ProxyPool::Add(this, T::typeid, (IProxy^)instance);

        return instance;
    }

    virtual void AddProxy(System::Type^ type, IProxy^ proxy) {
        //m_proxies->TryAdd(type, gcnew WeakProxy(proxy));
        ProxyPool::Add(this, type, proxy);
    }

protected:
    // It's weak because internally the proxy will hold a strong reference to the object
    /*System::Collections::Concurrent::ConcurrentDictionary<System::Type^, WeakProxy^>^ m_proxies
        = gcnew System::Collections::Concurrent::ConcurrentDictionary<System::Type^, WeakProxy^>(System::Environment::ProcessorCount * 2, 8);*/

internal:
    // Using a proxy cache that is separate from the object itself
    // because holding weak references in the object itself is asking for trouble
    ref class ProxyPool {
    internal:
        using WeakT = System::WeakReference<IProxy^>;

        static WeakT^ AddValueFactory(System::Type^ key, IProxy^ arg) {
            return gcnew WeakT(arg);
        }

        static System::Func<System::Type^, IProxy^, WeakT^>^ addValueFactory = gcnew System::Func<System::Type^, IProxy^, WeakT^>(AddValueFactory);

        static WeakT^ UpdateFactory(System::Type^ key, WeakT^ value, IProxy^ arg) {
            return gcnew WeakT(arg);
        }

        static System::Func<System::Type^, WeakT^, IProxy^, WeakT^>^ updateFactory = gcnew System::Func<System::Type^, WeakT^, IProxy^, WeakT^>(UpdateFactory);

        static IProxy^ GetIfExists(IObject^ obj, System::Type^ type) {
            const auto addr = obj->GetAddress();

            if (addr == 0) {
                return nullptr;
            }

            InternalDict^ dict = nullptr;
            if (!s_cache->TryGetValue(addr, dict)) {
                return nullptr;
            }

            WeakT^ weak = nullptr;
            if (!dict->TryGetValue(type, weak)) {
                return nullptr;
            }

            IProxy^ target = nullptr;
            if (!weak->TryGetTarget(target)) {
                return nullptr;
            }

            return target;
        }

        static void Add(IObject^ obj, System::Type^ type, IProxy^ proxy) {
            InternalDict^ dict = nullptr;
            const auto hashcode = obj->GetAddress();

            if (hashcode == 0) {
                return;
            }

            if (!s_cache->TryGetValue(hashcode, dict)) {
                dict = gcnew InternalDict(System::Environment::ProcessorCount * 2, 2);
                s_cache->TryAdd(hashcode, dict);
            }

            dict->AddOrUpdate(type, addValueFactory, updateFactory, proxy);
        }

        static void Remove(IObject^ obj) {
            InternalDict^ dict = nullptr;
            s_cache->TryRemove(obj->GetAddress(), dict);
        }

        static void Clear() {
            s_cache->Clear();
        }

        static void DisplayStats() {
            if (ImGuiNET::ImGui::TreeNode("ProxyPool")) {
                ImGuiNET::ImGui::Text("Cache size: " + s_cache->Count.ToString());
                ImGuiNET::ImGui::TreePop();
            }
        }

    private:
        // Not using the actual object as the key so that we can avoid holding strong references to the objects
        using InternalDict = System::Collections::Concurrent::ConcurrentDictionary<System::Type^, WeakT^>;
        using DictT = System::Collections::Concurrent::ConcurrentDictionary<uintptr_t, InternalDict^>;
        static DictT^ s_cache = gcnew DictT(System::Environment::ProcessorCount * 2, 8192);
    };
};
}