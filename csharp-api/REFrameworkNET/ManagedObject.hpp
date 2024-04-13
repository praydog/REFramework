#pragma once

#include <reframework/API.hpp>
#include "IObject.hpp"

#include "ObjectEnumerator.hpp"
#include "UnifiedObject.hpp"

#pragma managed

namespace REFrameworkNET {
ref class TypeDefinition;
ref class TypeInfo;
ref class InvokeRet;
ref class ManagedObject;

public ref class ManagedObject : public REFrameworkNET::UnifiedObject
{
internal:
    template <class T>
    ref class Cache {
    internal:
        using WeakT = System::WeakReference<T^>;

        static T^ Get(uintptr_t addr) {
            if (addr == 0) {
                return nullptr;
            }

            WeakT^ result = nullptr;

            if (s_cache->TryGetValue(addr, result)) {
                T^ strong = nullptr;

                if (result->TryGetTarget(strong)) {
                    return strong;
                }

                Cleanup(addr);
            }

            auto obj = gcnew T((::REFrameworkManagedObjectHandle)addr);
            result = gcnew WeakT(obj);
            if (!s_cache->TryAdd(addr, result)) {
                REFrameworkNET::API::LogWarning("Duplicate managed object cache entry for " + addr.ToString("X") + "!, finding the existing entry...");


                obj = nullptr;
                result = nullptr;
                if (s_cache->TryGetValue(addr, result)) {
                    if (result->TryGetTarget(obj)) {
                        REFrameworkNET::API::LogInfo("Found the existing entry for " + addr.ToString("X") + "!");
                        return obj;
                    }

                    Cleanup(addr);
                }

                REFrameworkNET::API::LogError("Failed to find the existing entry for " + addr.ToString("X") + "!");
            }

            return obj;
        }

        static void Cleanup(uintptr_t entry) {
            WeakT^ result = nullptr;

            if (s_cache->TryRemove(entry, result)) {
            }
        }

    private:
        static System::Collections::Concurrent::ConcurrentDictionary<uintptr_t, WeakT^>^ s_cache = gcnew System::Collections::Concurrent::ConcurrentDictionary<uintptr_t, WeakT^>();
    };

public:
    template <class T = ManagedObject>
    static T^ Get(reframework::API::ManagedObject* obj) {
        return Cache<T>::Get((uintptr_t)obj);
    }

    template <class T = ManagedObject>
    static T^ Get(::REFrameworkManagedObjectHandle handle) {
        return Cache<T>::Get((uintptr_t)handle);
    }

protected:
    ManagedObject(reframework::API::ManagedObject* obj) 
        : m_object(obj),
          m_weak(true)
    {
        AddRefIfGlobalized();
    }

    ManagedObject(::REFrameworkManagedObjectHandle handle) 
        : m_object(reinterpret_cast<reframework::API::ManagedObject*>(handle)),
          m_weak(true)
    {
        AddRefIfGlobalized();
    }

    // Double check if we really want to allow this
    // We might be better off having a global managed object cache
    // instead of AddRef'ing every time we create a new ManagedObject
    ManagedObject(ManagedObject^ obj) 
        : m_object(obj->m_object),
          m_weak(true)
    {
        if (m_object != nullptr) {
            AddRefIfGlobalized();
        }
    }

internal:
    // Dispose
    ~ManagedObject() {
        this->!ManagedObject();
    }

    // Finalizer
    !ManagedObject() {
        if (m_object != nullptr) {
            ReleaseIfGlobalized();
            ManagedObject::Cache<ManagedObject>::Cleanup((uintptr_t)m_object);
        }
    }

public:

    /// <summary>
    /// <para>
    /// Globalizes the managed object if it is not already globalized <br/>
    /// This is usually useful in instances where objects are newly created, and you want <br/>
    /// to keep long term references to them, otherwise the object will be quickly destroyed <br/>
    /// A local object is an object that can only be referenced by the spawned thread <br/>
    /// A global object is an object that can be referenced by any thread <br/>
    /// <br/>
    /// To quote the talk, "Achieve Rapid Iteration: RE ENGINE Design": <br/>
    /// <br/>
    /// Local object <br/>
    ///  Objects that can only be referenced by the spawned thread <br/>
    ///  Registered in the local table for each thread <br/>
    ///  Reference counter (RC) is negative and points to the index of the local table <br/>
    ///  All objects created from C# will be local objects <br/>
    /// <br/>
    /// Local => Global conversion <br/>
    ///  Convert when it becomes available to all threads <br/>
    ///    When storing in a static field <br/>
    ///    When storing in a field of a global object <br/>
    ///  Cleared from local table and RC becomes 1 <br/>
    ///    Convert all references globally <br/>
    /// <br/>
    /// Global object <br/>
    ///  Objects that can be referenced by all threads <br/>
    ///  Reference counter (RC) is positive and represents the number of object references <br/>
    ///  All objects generated from C++ become global objects <br/>
    /// <br/>
    /// </para>
    /// </summary>
    /// <returns>The current managed object</returns>
    /// <remarks>
    /// <para>
    /// This should only need to be called in the following instances: <br/>
    /// You are manually creating an instance of a managed object <br/>
    /// A method you are calling is freshly creating a new managed object (usually arrays or some other kind of constructor) <br/>
    /// More information: <a href="https://github.com/kasicass/blog/blob/master/3d-reengine/2021_03_10_achieve_rapid_iteration_re_engine_design.md#framegc-algorithm-17">https://github.com/kasicass/blog/blob/master/3d-reengine/2021_03_10_achieve_rapid_iteration_re_engine_design.md#framegc-algorithm-17</a> <br/>
    /// </para>
    /// </remarks>
    ManagedObject^ Globalize();

    /// <summary>
    /// Adds a reference to the managed object
    /// </summary>
    /// <remarks>Try to avoid calling this manually except in extraordinary circumstances</remarks>
    void AddRef();

    /// <summary>
    /// Releases a reference to the managed object
    /// </summary>
    /// <remarks>Try to avoid calling this manually except in extraordinary circumstances</remarks>
    void Release();

    void ForceRelease() {
        if (m_object == nullptr) {
            return;
        }

        m_object->release();
    }

    void AddRefIfGlobalized() {
        if (m_object == nullptr || !IsGlobalized()) {
            return;
        }

        AddRef();
    }

    void ReleaseIfGlobalized() {
        if (m_object == nullptr || !IsGlobalized()) {
            return;
        }

        Release();
    }

    int32_t GetReferenceCount() {
        if (m_object == nullptr) {
            return 0;
        }

        // TODO: Make this less hacky/pull from the C++ API?
        return *(int32_t*)((uintptr_t)m_object + 0x8);
    }

    bool IsLocalVariable() {
        return GetReferenceCount() < 0;
    }

    bool IsGlobalized() {
        return GetReferenceCount() >= 0 || !m_weak;
    }

    bool IsGoingToBeDestroyed() {
        return GetReferenceCount() == 0;
    }

    static bool IsManagedObject(uintptr_t ptr) {
        if (ptr == 0) {
            return false;
        }

        static auto fn = reframework::API::get()->param()->sdk->managed_object->is_managed_object;
        return fn((void*)ptr);
    }

    static ManagedObject^ ToManagedObject(uintptr_t ptr) {
        if (ptr == 0) {
			return nullptr;
		}

        if (IsManagedObject(ptr)) {
            return gcnew ManagedObject((reframework::API::ManagedObject*)ptr);
        }

        return nullptr;
    }

    static ManagedObject^ FromAddress(uintptr_t ptr) {
        return ToManagedObject(ptr);
    }

    TypeInfo^ GetTypeInfo();

public: // IObject
    virtual void* Ptr() override {
        return (void*)m_object;
    }

    virtual uintptr_t GetAddress() override  {
        return (uintptr_t)m_object;
    }

    virtual TypeDefinition^ GetTypeDefinition() override;

    generic <typename T>
    virtual T As() override;

    // TODO methods:
    /*public Void* GetReflectionProperties() {
        return _original.get_reflection_properties();
    }*/

    /*public ReflectionProperty GetReflectionPropertyDescriptor(basic_string_view<char\, std::char_traits<char>> name) {
        return _original.get_reflection_property_descriptor(name);
    }

    public ReflectionMethod GetReflectionMethodDescriptor(basic_string_view<char\, std::char_traits<char>> name) {
        return _original.get_reflection_method_descriptor(name);
    }*/

protected:
    reframework::API::ManagedObject* m_object;
    bool m_weak{true};
};
}