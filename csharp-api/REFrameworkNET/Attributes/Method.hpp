#pragma once

#include "../TDB.hpp"

namespace REFrameworkNET::Attributes {
    /// <summary>Attribute to mark a reference assembly method for easier lookup.</summary>
    [System::AttributeUsage(System::AttributeTargets::Method)]
    public ref class Method : public System::Attribute {
    public:
        static Method^ GetCachedAttribute(System::Reflection::MethodInfo^ target) {
            Method^ attr = nullptr;

            // Lock the cache for reading
            cacheLock->EnterReadLock();
            try {
                if (cache->TryGetValue(target, attr) && attr != nullptr) {
                    return attr;
                }
            } finally {
                cacheLock->ExitReadLock();
            }

            // Lock the cache for writing
            cacheLock->EnterWriteLock();
            try {
                if (cache->TryGetValue(target, attr) && attr != nullptr) {
                    return attr;
                }

                if (attr = (REFrameworkNET::Attributes::Method^)System::Attribute::GetCustomAttribute(target, REFrameworkNET::Attributes::Method::typeid)) {
                    cache->Add(target, attr);

#ifdef REFRAMEWORK_VERBOSE
                    System::Console::WriteLine("Cached Method attribute for {0}", target->Name);
#endif
                    return attr;
                }
            } finally {
                cacheLock->ExitWriteLock();
            }

            return attr;
        }

    private:
        static System::Collections::Generic::Dictionary<System::Reflection::MethodInfo^, Method^>^ cache = gcnew System::Collections::Generic::Dictionary<System::Reflection::MethodInfo^, Method^>();
        static System::Threading::ReaderWriterLockSlim^ cacheLock = gcnew System::Threading::ReaderWriterLockSlim();

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
            if (signature != nullptr && isVirtual && td != method->GetDeclaringType()) {
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