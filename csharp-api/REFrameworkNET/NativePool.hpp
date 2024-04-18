#pragma once

#include <cstdint>

namespace REFrameworkNET {
    // Class to pool frequently re-used classes like TypeDefinition, Method, Field
    // which only hold a pointer to an unmanaged object.
    // And this internal object never moves or anything, so we can just keep one instance of it instead of creating a new one every time.
    // The goal is to minimize GC pressure and improve performance.
    template <typename T>
    public ref class NativePool
    {
    private:
        static System::Collections::Concurrent::ConcurrentDictionary<uintptr_t, T^>^ s_cache =
            gcnew System::Collections::Concurrent::ConcurrentDictionary<uintptr_t, T^>();

    public:
        delegate T^ CreatorDelegate(uintptr_t nativePtr);

        static T^ GetInstance(uintptr_t nativePtr, CreatorDelegate^ creator) {
            if (nativePtr == 0) {
                throw gcnew System::ArgumentNullException("nativePtr cannot be Zero");
            }

            T^ cachedItem;
            if (s_cache->TryGetValue(nativePtr, cachedItem)) {
                return cachedItem;
            }
            
            T^ newItem = creator(nativePtr);
            s_cache->TryAdd(nativePtr, newItem);
            return newItem;
        }

        static void DisplayStats() {
            if (ImGuiNET::ImGui::TreeNode(T::typeid->Name + " Cache")) {
                ImGuiNET::ImGui::Text("Cache size: " + s_cache->Count.ToString());
                ImGuiNET::ImGui::TreePop();
            }
        }
    };
}