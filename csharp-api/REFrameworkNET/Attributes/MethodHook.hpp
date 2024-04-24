#pragma once

#include <cstdint>

#include  "../TDB.hpp"
#include "../Method.hpp"
#include "../MethodHook.hpp"

namespace REFrameworkNET {
    ref class TypeDefinition;
    ref class Method;

    public enum class MethodHookType : uint8_t {
        Pre,
        Post
    };
}

namespace REFrameworkNET::Attributes {
    /// <summary>Attribute to mark a method as a hook.</summary>
    [System::AttributeUsage(System::AttributeTargets::Method)]
    public ref class MethodHookAttribute : public System::Attribute {
    internal:
        void BaseConstructor(System::Type^ declaringType, System::String^ methodSignature, MethodHookType type) {
            if (declaringType == nullptr) {
                throw gcnew System::ArgumentNullException("declaringType");
            }

            // new static readonly TypeDefinition REFType = TDB.Get().FindType("app.Collision.HitController.DamageInfo");
            auto refTypeField = declaringType->GetField("REFType", System::Reflection::BindingFlags::Static | System::Reflection::BindingFlags::Public);

            if (refTypeField == nullptr) {
                throw gcnew System::ArgumentException("Type does not have a REFrameworkNET::TypeDefinition field");
            }

            m_declaringType = (TypeDefinition^)refTypeField->GetValue(nullptr);

            if (m_declaringType == nullptr) {
                throw gcnew System::ArgumentException("Type does not have a REFrameworkNET::TypeDefinition field");
            }

            m_method = m_declaringType != nullptr ? m_declaringType->GetMethod(methodSignature) : nullptr;
            m_hookType = type;
        }
        
    public:
        MethodHookAttribute(System::Type^ declaringType, System::String^ methodSignature, MethodHookType type) {
            BaseConstructor(declaringType, methodSignature, type);
        }

        MethodHookAttribute(System::Type^ declaringType, System::String^ methodSignature, MethodHookType type, bool skipJmp) {
            BaseConstructor(declaringType, methodSignature, type);
            m_skipJmp = skipJmp;
        }

        property bool Valid {
        public:
            bool get() {
                return m_declaringType != nullptr && m_method != nullptr;
            }
        }

        bool Install(System::Reflection::MethodInfo^ destination) {
            if (!Valid) {
                throw gcnew System::ArgumentException("Invalid method hook");
            }
            
            if (!destination->IsStatic) {
                throw gcnew System::ArgumentException("Destination method must be static");
            }
            
            auto hook = REFrameworkNET::MethodHook::Create(m_method, m_skipJmp);

            if (m_hookType == MethodHookType::Pre) {
                auto del = System::Delegate::CreateDelegate(REFrameworkNET::MethodHook::PreHookDelegate::typeid, destination);
                hook->AddPre((REFrameworkNET::MethodHook::PreHookDelegate^)del);
            } else if (m_hookType == MethodHookType::Post) {
                auto del = System::Delegate::CreateDelegate(REFrameworkNET::MethodHook::PostHookDelegate::typeid, destination);
                hook->AddPost((REFrameworkNET::MethodHook::PostHookDelegate^)del);
            }

            return true;
        }

    protected:
        REFrameworkNET::TypeDefinition^ m_declaringType;
        REFrameworkNET::Method^ m_method;
        MethodHookType m_hookType;
        bool m_skipJmp{ false };
    };
}