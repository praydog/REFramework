#pragma once

#include <cstdint>

#include "../TDB.hpp"

namespace REFrameworkNET {
    public enum class FieldFacadeType : uint8_t {
        None,
        Getter,
        Setter
    };
}

namespace REFrameworkNET::Attributes {
    /// <summary>Attribute to mark a reference assembly method for easier lookup.</summary>
    [System::AttributeUsage(System::AttributeTargets::Method)]
    public ref class MethodAttribute : public System::Attribute {
    private:
        static System::Collections::Concurrent::ConcurrentDictionary<System::Reflection::MethodInfo^, MethodAttribute^>^ cache 
          = gcnew System::Collections::Concurrent::ConcurrentDictionary<System::Reflection::MethodInfo^, MethodAttribute^>(System::Environment::ProcessorCount * 2, 8192);

    public:
        static MethodAttribute^ GetCachedAttribute(System::Reflection::MethodInfo^ target) {
            MethodAttribute^ attr = nullptr;

            if (cache->TryGetValue(target, attr)) {
                return attr;
            }

            if (attr = (MethodAttribute^)System::Attribute::GetCustomAttribute(target, MethodAttribute::typeid); attr != nullptr) {
                cache->TryAdd(target, attr);

#ifdef REFRAMEWORK_VERBOSE
                System::Console::WriteLine("Cached Method attribute for {0}", target->Name);
#endif
                return attr;
            }


            return attr;
        }

    public:
        MethodAttribute(uint32_t methodIndex, FieldFacadeType fieldFacadeType) {
            if (fieldFacadeType != FieldFacadeType::None) {
                field = REFrameworkNET::TDB::Get()->GetField(methodIndex);
                fieldFacade = fieldFacadeType;

                if (field != nullptr) {
                    signature = nullptr;
                    isVirtual = false;
                }

                return;
            }

            method = REFrameworkNET::TDB::Get()->GetMethod(methodIndex);
            fieldFacade = FieldFacadeType::None;

            if (method != nullptr && method->IsVirtual()) {
                signature = method->GetMethodSignature();
                isVirtual = method->IsVirtual();
            }
        }

        property bool IsField {
            bool get() {
                return field != nullptr;
            }
        }

        property FieldFacadeType FieldFacade {
            FieldFacadeType get() {
                return fieldFacade;
            }
        }

        REFrameworkNET::Method^ GetMethod(REFrameworkNET::TypeDefinition^ td) {
            if (method == nullptr) {
                return nullptr;
            }

            // Signature is used for virtual methods
            if (isVirtual && signature != nullptr && td != method->GetDeclaringType()) {
                if (auto result = td->GetMethod(signature); result != nullptr) {
                    return result;
                }
            }

            return method;
        }

        REFrameworkNET::Field^ GetField() {
            return field;
        }

        System::Object^ GetUnderlyingObject(REFrameworkNET::TypeDefinition^ td) {
            if (auto method = GetMethod(td); method != nullptr) {
                return method;
            }

            return field;
        }

    private:
        REFrameworkNET::Method^ method;
        REFrameworkNET::Field^ field;
        FieldFacadeType fieldFacade{ FieldFacadeType::None };
        System::String^ signature;
        bool isVirtual;
    };
}