#pragma once

#include <reframework/API.hpp>
#include "IObject.hpp"

#include "ObjectEnumerator.hpp"
#include "UnifiedObject.hpp"

#pragma managed

namespace REFrameworkNET {
ref class TypeDefinition;
ref class TypeInfo;
value struct InvokeRet;
ref class ManagedObject;

public ref class ManagedObject : public REFrameworkNET::UnifiedObject
{
internal:
    delegate void FinalizerDelegate(ManagedObject^ obj);

    template <typename T>
    ref class Cache {
    internal:
        using WeakT = System::WeakReference<ManagedObject^>;

        static WeakT^ AddValueFactory(uintptr_t key, ManagedObject^ arg) {
            return gcnew WeakT(arg);
        }

        static System::Func<uintptr_t, ManagedObject^, WeakT^>^ addValueFactory = gcnew System::Func<uintptr_t, ManagedObject^, WeakT^>(AddValueFactory);

        static WeakT^ UpdateFactory(uintptr_t key, WeakT^ value, ManagedObject^ arg) {
            return gcnew WeakT(arg);
        }

        static System::Func<uintptr_t, WeakT^, ManagedObject^, WeakT^>^ updateFactory = gcnew System::Func<uintptr_t, WeakT^, ManagedObject^, WeakT^>(UpdateFactory);

        static T^ GetIfExists(uintptr_t addr) {
            if (addr == 0) {
                return nullptr;
            }

            WeakT^ result = nullptr;
            T^ strong = nullptr;

            if (s_impl->cache->TryGetValue(addr, result)) {
                if (result->TryGetTarget(strong)) {
                    return strong;
                }
            }

            return nullptr;
        }

        static T^ Get(uintptr_t addr) {
            if (addr == 0) {
                return nullptr;
            }

            // Do not cache local objects, it's just a burden on the cache
            if (*(int32_t*)(addr + 0x8) < 0) {
                return gcnew T((::REFrameworkManagedObjectHandle)addr, false); // Local object, false for not cached
            }

            WeakT^ result = nullptr;
            T^ strong = nullptr;

            if (s_impl->cache->TryGetValue(addr, result)) {
                if (result->TryGetTarget(strong)) {
                    return strong;
                }

#ifdef REFRAMEWORK_VERBOSE
                System::Console::WriteLine("Existing entry for " + addr.ToString("X") + " is dead! Updating...");
#endif
            }

            strong = GetFromPool((::REFrameworkManagedObjectHandle)addr);
            s_impl->cache->AddOrUpdate(addr, addValueFactory, updateFactory, (ManagedObject^)strong);

            return strong;
        }

        static void ReturnToPool(ManagedObject^ obj) {
            // Remove the weak reference from the cache
            WeakT^ weak = nullptr;
            s_impl->cache->TryRemove(obj->GetAddress(), weak);

            System::GC::ReRegisterForFinalize(obj);
            obj->Deinitialize();
            s_impl->pool->Enqueue((ManagedObject^)obj);
        }

        static void OnObjectFinalize(ManagedObject^ obj) {
            ReturnToPool(obj);
        }

        static T^ GetFromPool(::REFrameworkManagedObjectHandle handle) {
            ManagedObject^ obj = nullptr;
            if (s_impl->pool->TryDequeue(obj)) {
                obj->Initialize(handle);
                return (T^)obj;
            }

            // Create a new entry
            obj = (ManagedObject^)(gcnew T(handle, true));
            obj->SetFinalizerDelegate(s_finalizerDelegate);

            // Object will be returned to pool once the consumer is done with it
            return (T^)obj;
        }

        static void DisplayStats() {
            if (ImGuiNET::ImGui::TreeNode(T::typeid->Name + " Cache")) {
                ImGuiNET::ImGui::Text("Cache size: " + s_impl->cache->Count.ToString());
                ImGuiNET::ImGui::Text("Pool size: " + s_impl->pool->Count.ToString());
                ImGuiNET::ImGui::TreePop();
            }
        }

        static void CleanupAll() {
            s_impl = gcnew Impl();
        }

    private:
        ref struct Impl {
            ~Impl() {
                this->!Impl();
            }

            !Impl() {
            }

            System::Collections::Concurrent::ConcurrentDictionary<uintptr_t, WeakT^>^ cache 
              = gcnew System::Collections::Concurrent::ConcurrentDictionary<uintptr_t, WeakT^>(System::Environment::ProcessorCount * 2, 8192);
            System::Collections::Concurrent::ConcurrentQueue<ManagedObject^>^ pool = gcnew System::Collections::Concurrent::ConcurrentQueue<ManagedObject^>();
        };

        static Impl^ s_impl = gcnew Impl();
        static FinalizerDelegate^ s_finalizerDelegate = gcnew FinalizerDelegate(OnObjectFinalize);
    };

internal:
    static void CleanupKnownCaches();

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
    ManagedObject(::REFrameworkManagedObjectHandle handle, bool cached)
        : m_object(reinterpret_cast<reframework::API::ManagedObject*>(handle)),
        m_weak(true),
        m_cached(cached)
    {
        AddRefIfGlobalized();
    }

    // Double check if we really want to allow this
    // We might be better off having a global managed object cache
    // instead of AddRef'ing every time we create a new ManagedObject
    ManagedObject(ManagedObject^% obj) 
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
        Internal_Finalize();
    }
    void Internal_Finalize();

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

        auto existingEntry = ManagedObject::Cache<ManagedObject>::GetIfExists(ptr);

        if (existingEntry != nullptr) {
            return existingEntry;
        }

        // IsManagedObject can be expensive, so only call it if there's no existing entry
        if (IsManagedObject(ptr)) {
            return ManagedObject::Get<ManagedObject>((::REFrameworkManagedObjectHandle)ptr);
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

internal:
    static bool ShuttingDown = false;

    void Deinitialize() {
        m_initialized = false;
    }

    void Initialize(::REFrameworkManagedObjectHandle handle) {
        m_object = reinterpret_cast<reframework::API::ManagedObject*>(handle);
        m_initialized = true;
        m_weak = true;
        m_cached = true;

        AddRefIfGlobalized();
    }

    void SetFinalizerDelegate(FinalizerDelegate^ delegate) {
        m_finalizerDelegate = delegate;
    }

    bool IsInitialized() {
        return m_initialized;
    }

protected:
    reframework::API::ManagedObject* m_object;

    bool m_weak{true}; // Can be upgraded to a global object after it's created, but not to a cached object.
    bool m_cached{false}; // Cannot be upgraded to a cached object if it was created on a non-globalized object
    bool m_initialized{true}; // Used for the object pool

internal:
    FinalizerDelegate^ m_finalizerDelegate{nullptr};
};
}