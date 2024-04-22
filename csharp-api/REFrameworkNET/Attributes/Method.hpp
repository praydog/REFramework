#pragma once

#include "../TDB.hpp"

namespace REFrameworkNET::Attributes {
    /// <summary>Attribute to mark a reference assembly method for easier lookup.</summary>
    [System::AttributeUsage(System::AttributeTargets::Method)]
    public ref class Method : public System::Attribute {
    private:
        static System::Collections::Concurrent::ConcurrentDictionary<System::Reflection::MethodInfo^, Method^>^ cache 
          = gcnew System::Collections::Concurrent::ConcurrentDictionary<System::Reflection::MethodInfo^, Method^>(System::Environment::ProcessorCount * 2, 8192);

    public:
        static Method^ GetCachedAttribute(System::Reflection::MethodInfo^ target) {
            Method^ attr = nullptr;

            if (cache->TryGetValue(target, attr)) {
                return attr;
            }

            if (attr = (REFrameworkNET::Attributes::Method^)System::Attribute::GetCustomAttribute(target, REFrameworkNET::Attributes::Method::typeid); attr != nullptr) {
                cache->TryAdd(target, attr);

#ifdef REFRAMEWORK_VERBOSE
                System::Console::WriteLine("Cached Method attribute for {0}", target->Name);
#endif
                return attr;
            }


            return attr;
        }

    public:
        Method(uint32_t methodIndex) {
            method = REFrameworkNET::TDB::Get()->GetMethod(methodIndex);

            if (method != nullptr && method->IsVirtual()) {
                signature = method->GetMethodSignature();
                isVirtual = method->IsVirtual();
            }
        }

        REFrameworkNET::Method^ GetMethod(REFrameworkNET::TypeDefinition^ td) {
            // Signature is used for virtual methods
            if (isVirtual && signature != nullptr && td != method->GetDeclaringType()) {
                if (auto result = td->GetMethod(signature); result != nullptr) {
                    return result;
                }
            }

            return method;
        }

    private:
        REFrameworkNET::Method^ method;
        System::String^ signature;
        bool isVirtual;
    };
}