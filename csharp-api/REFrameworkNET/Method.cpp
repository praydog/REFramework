#include <bit>

#include "ManagedObject.hpp"
#include "NativeObject.hpp"

#include "MethodHook.hpp"
#include "Method.hpp"
#include "Field.hpp"

#include "API.hpp"
#include "VM.hpp"
#include "SystemString.hpp"
#include "Proxy.hpp"

#include "Utility.hpp"

using namespace System;

namespace REFrameworkNET {
MethodHook^ Method::AddHook(bool ignore_jmp) {
    return MethodHook::Create(this, ignore_jmp);
}

ManagedObject^ Method::GetRuntimeMethod() {
    auto declaringType = this->GetDeclaringType();

    if (declaringType == nullptr) {
        return nullptr;
    }

    auto methods = declaringType->GetRuntimeMethods();
    for each (REFrameworkNET::ManagedObject^ method in methods) {
        // Get the type handle and compare it to this (raw pointer stored in the Method object)
        // System.RuntimeMethodHandle automatically gets converted to a Method object
        auto rawMethodHandle = (System::IntPtr^)method->GetField("_runtimeHandle");
        auto methodHandle = rawMethodHandle != nullptr ? Method::GetInstance(reinterpret_cast<reframework::API::Method*>(rawMethodHandle->ToPointer())) : (Method^)method->Call("get_MethodHandle");
        
        if (methodHandle == nullptr) {
            // This is some stupid way to deal with the absence of get_MethodHandle
            // in monster hunter wilds.
            uintptr_t hacky_ptr = *(uintptr_t*)((uintptr_t)method->Ptr() + 0x10);

            if (hacky_ptr == (uintptr_t)this->GetRaw()) {
                return method;
            }
        }

        if (methodHandle == nullptr) {
            continue;
        }

        if (methodHandle->GetRaw() == this->GetRaw()) {
            return method;
        }
    }

    return nullptr;
}

System::Collections::Generic::List<Method^>^ Method::GetMatchingParentMethods() {
    auto out = gcnew System::Collections::Generic::List<Method^>();
    auto declaringType = this->GetDeclaringType();

    if (declaringType == nullptr) {
        return out;
    }

    if (declaringType->ParentType == nullptr) {
        return out;
    }

    auto returnType = this->GetReturnType();
    auto parameters = this->GetParameters()->ToArray();

    for (auto parentType = declaringType->ParentType; parentType != nullptr; parentType = parentType->ParentType) {
        auto parentMethods = parentType->GetMethods();

        for each (auto parentMethod in parentMethods) {
            if (parentMethod->Name != this->Name) {
                continue;
            }

            if (parentMethod->DeclaringType != parentType) {
                continue;
            }

            /*if (parentMethod->GetReturnType() != returnType) {
                continue;
            }*/

            // Generic return type check
            if (parentMethod->ReturnType->Name->Contains("!")) {
                continue;
            }

            if (parentMethod->GetNumParams() != parameters->Length) {
                continue;
            }

            if (parameters->Length == 0) {
                out->Add(parentMethod);
                continue;
            }

            auto parentParams = parentMethod->GetParameters()->ToArray();
            bool fullParamsMatch = true;

            for (int i = 0; i < parameters->Length; ++i) {
                auto param = parameters[i];
                auto parentParam = parentParams[i];

                if (param->Type != parentParam->Type) {
                    fullParamsMatch = false;
                    break;
                }
            }

            if (fullParamsMatch) {
                out->Add(parentMethod);
            }
        }
    }

    return out;
}

bool Method::IsOverride() {
    return GetMatchingParentMethods()->Count > 0;
}

REFrameworkNET::InvokeRet Method::Invoke(System::Object^ obj, array<System::Object^>^ args) {
    return REFrameworkNET::InvokeRet::FromNative(Invoke_Internal(obj, args));
}

System::Object^ Method::InvokeBoxed(System::Type^ targetReturnType, System::Object^ obj, array<System::Object^>^ args) {
    System::Object^ result = nullptr;
    this->HandleInvokeMember_Internal(obj, args, result);

    return Utility::TranslateBoxedData(targetReturnType, result);
}

::reframework::InvokeRet Method::Invoke_Internal(System::Object^ obj, array<System::Object^>^ args) {
    if (obj == nullptr && !this->IsStatic()) {
        System::String^ declaringName = this->GetDeclaringType() != nullptr ? this->GetDeclaringType()->GetFullName() : gcnew System::String("UnknownType");
        System::String^ errorStr = "Cannot invoke a non-static method without an object (" + declaringName + "." + this->GetName() + ")";
        REFrameworkNET::API::LogError(errorStr);
        throw gcnew System::InvalidOperationException(errorStr);
    }

    //std::vector<void*> args2{};
    std::array<void*, 32> args2{}; // what function has more than 32 arguments?

    if (args != nullptr && args->Length > 0) {
        for (int i = 0; i < args->Length; ++i) try {
            if (args[i] == nullptr) {
                args2[i] = nullptr;
                continue;
            }

            //args2[i] = args[i]->ptr();
            auto t = args[i]->GetType();
            System::Object^ arg = args[i];

            if (t->IsEnum) {
                auto underlyingType = System::Enum::GetUnderlyingType(t);
                arg = System::Convert::ChangeType(args[i], underlyingType);
                t = underlyingType;
            }

            if (REFrameworkNET::IObject::typeid->IsAssignableFrom(t)) {
                auto iobj = static_cast<REFrameworkNET::IObject^>(args[i]);
                args2[i] = iobj->Ptr();
            } else if (t == REFrameworkNET::TypeDefinition::typeid) {
                // TypeDefinitions are wrappers for System.RuntimeTypeHandle
                // However there's basically no functions that actually take a System.RuntimeTypeHandle
                // so we will just convert it to a System.Type.
                auto td = static_cast<REFrameworkNET::TypeDefinition^>(args[i]);

                if (auto rt = td->GetRuntimeType(); rt != nullptr) {
                    args2[i] = rt->Ptr();
                } else {
                    System::Console::WriteLine("TypeDefinition has no runtime type @ arg " + i);
                }
            } else if (t == System::String::typeid) {
                auto createdStr = VM::CreateString(static_cast<System::String^>(args[i]));

                if (createdStr != nullptr) {
                    args2[i] = createdStr->Ptr();
                } else {
                    System::Console::WriteLine("Error creating string @ arg " + i);
                }
            } else if (t == System::Boolean::typeid) {
                bool v = System::Convert::ToBoolean(arg);
                args2[i] = (void*)(intptr_t)v;
            } else if (t == System::Int32::typeid) {
                int32_t v = System::Convert::ToInt32(arg);
                args2[i] = (void*)(intptr_t)v;
            } else if (t == System::Byte::typeid) {
                uint8_t v = System::Convert::ToByte(arg);
                args2[i] = (void*)(uint64_t)v;
            } else if (t == System::SByte::typeid) {
                int8_t v = System::Convert::ToSByte(arg);
                args2[i] = (void*)(uint64_t)v;
            } else if (t == System::Char::typeid) {
                wchar_t v = System::Convert::ToChar(arg);
                args2[i] = (void*)(uint64_t)v;
            } else if (t == System::Int16::typeid) {
                int16_t v = System::Convert::ToInt16(arg);
                args2[i] = (void*)(uint64_t)v;
            } else if (t == System::UInt16::typeid) {
                uint16_t v = System::Convert::ToUInt16(arg);
                args2[i] = (void*)(uint64_t)v;
            } else if (t == System::UInt32::typeid) {
                uint32_t v = System::Convert::ToUInt32(arg);
                args2[i] = (void*)(uint64_t)v;
            } else if (t == System::Single::typeid) {
                // this might seem counterintuitive, converting a float to a double
                // but the invoke wrappers ALWAYS expect a double, so we need to do this
                // even when they take a System.Single, the wrappers take in a double and convert it to a float
                float v = System::Convert::ToSingle(arg);
                auto d = (double)v;
                auto n = *(int64_t*)&d;
                args2[i] = (void*)(uint64_t)n;
            } else if (t == System::UInt64::typeid || t == System::UIntPtr::typeid) {
                args2[i] = (void*)System::Convert::ToUInt64(arg);
            } else if (t == System::Int64::typeid || t == System::IntPtr::typeid) {
                args2[i] = (void*)System::Convert::ToInt64(arg);
            } else if (t == System::Double::typeid) {
                double v = System::Convert::ToDouble(arg);
                auto n = *(int64_t*)&v;
                args2[i] = (void*)(uint64_t)n;
            } else {
                args2[i] = nullptr;
                System::Console::WriteLine("Unknown type passed to method invocation @ arg " + i + " (" + t->FullName + ")");
            }
        } catch (System::Exception^ e) {
            System::Console::WriteLine("Error converting argument " + i + ": " + e->Message);
        }
    }

    void* obj_ptr = nullptr;

    if (obj != nullptr) try {
        const auto obj_t = obj->GetType();

        if (REFrameworkNET::IObject::typeid->IsAssignableFrom(obj_t)) {
            obj_ptr = static_cast<REFrameworkNET::IObject^>(obj)->Ptr();
        } else if (obj_t == System::IntPtr::typeid) {
            obj_ptr = (void*)(intptr_t)safe_cast<System::IntPtr>(obj).ToPointer();
        } else if (obj_t == System::UIntPtr::typeid) {
            obj_ptr = (void*)(uintptr_t)safe_cast<System::UIntPtr>(obj).ToUInt64();
        } else {
            System::Console::WriteLine("Unknown type passed to method invocation @ obj");
        }
    } catch (System::Exception^ e) {
        System::Console::WriteLine("Error converting object: " + e->Message);
    }

    const auto argcount = args != nullptr ? args->Length : 0;

    return m_method->invoke((::reframework::API::ManagedObject*)obj_ptr, std::span<void*>(args2.data(), argcount));
}

bool Method::HandleInvokeMember_Internal(System::Object^ obj, array<System::Object^>^ args, System::Object^% result) {
    auto tempResult = this->Invoke_Internal(obj, args);
    auto returnType = this->GetReturnType();

    if (returnType == nullptr) {
        // box the result
        result = static_cast<System::Object^>(REFrameworkNET::InvokeRet::FromNative(tempResult));
        return true;
    }

    result = Utility::BoxData((uintptr_t*)tempResult.bytes.data(), returnType, true);
    return true;
}
}