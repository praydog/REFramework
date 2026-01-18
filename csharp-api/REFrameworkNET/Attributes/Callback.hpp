#pragma once

#include <cstdint>

#include "../Callbacks.hpp"

namespace REFrameworkNET {
    public enum class CallbackType : uint8_t {
        Pre,
        Post
    };
}

namespace REFrameworkNET::Attributes {
    [System::AttributeUsage(System::AttributeTargets::Method)]
    public ref class CallbackAttribute : public System::Attribute {
    public:
        CallbackAttribute(System::Type^ declaringType, CallbackType type) {
            if (declaringType == nullptr) {
                throw gcnew System::ArgumentNullException("declaringType");
            }

            if (!REFrameworkNET::ICallback::typeid->IsAssignableFrom(declaringType)) {
                throw gcnew System::ArgumentException("Type does not implement ICallback");
            }

            const auto wantedFlags = System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Static;

            // Double check that the type has Pre and Post events
            auto preEvent = declaringType->GetField("PreImplementation", wantedFlags);
            auto postEvent = declaringType->GetField("PostImplementation", wantedFlags);

            if (preEvent == nullptr || postEvent == nullptr) {
                throw gcnew System::ArgumentException("Callback type does not have Pre and Post events");
            }

            m_declaringType = declaringType;
        }

        property bool Valid {
        public:
            bool get() {
                return m_declaringType != nullptr;
            }
        }

        void Install(System::Reflection::MethodInfo^ destination) {
            if (!Valid) {
                throw gcnew System::InvalidOperationException("Callback is not valid");
            }

            if (!destination->IsStatic) {
                throw gcnew System::ArgumentException("Destination method must be static");
            }

            const auto wantedFlags = System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Static;

            auto preEvent = m_declaringType->GetField("PreImplementation", wantedFlags);
            auto postEvent = m_declaringType->GetField("PostImplementation", wantedFlags);

            if (preEvent == nullptr || postEvent == nullptr) {
                throw gcnew System::ArgumentException("Callback type does not have Pre and Post fields");
            }

            auto preDelegate = (BaseCallback::Delegate^)preEvent->GetValue(nullptr);
            auto postDelegate = (BaseCallback::Delegate^)postEvent->GetValue(nullptr);

            if (m_callbackType == CallbackType::Pre) {
                auto del = (BaseCallback::Delegate^)System::Delegate::CreateDelegate(BaseCallback::Delegate::typeid, destination);

                if (preDelegate == nullptr) {
                    preDelegate = del;
                } else {
                    preDelegate = preDelegate + del;
                }

                preEvent->SetValue(nullptr, preDelegate);
            } else {
                auto del = (BaseCallback::Delegate^)System::Delegate::CreateDelegate(BaseCallback::Delegate::typeid, destination);

                if (postDelegate == nullptr) {
                    postDelegate = del;
                } else {
                    postDelegate = postDelegate + del;
                }

                postEvent->SetValue(nullptr, postDelegate);
            }
        }

    private:
        System::Type^ m_declaringType{nullptr};
        CallbackType m_callbackType;
    };
}