#include <bit>

#include "ManagedObject.hpp"
#include "NativeObject.hpp"

#include "MethodHook.hpp"
#include "Method.hpp"
#include "Field.hpp"

#include "API.hpp"
#include "VM.hpp"
#include "SystemString.hpp"

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

    // System.Type
    auto runtimeType = declaringType->GetRuntimeType()/*->As<System::Type^>()*/;

    if (runtimeType == nullptr) {
        return nullptr;
    }

    // Iterate over all methods in the runtime type
    auto methods = (REFrameworkNET::ManagedObject^)runtimeType->Call("GetMethods(System.Reflection.BindingFlags)", System::Reflection::BindingFlags::Public | System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Instance | System::Reflection::BindingFlags::Static);

    if (methods != nullptr) {
        auto methodDefName = this->Name;
        for each (REFrameworkNET::ManagedObject^ method in methods) {
            // Get the type handle and compare it to this (raw pointer stored in the Method object)
            // System.RuntimeMethodHandle automatically gets converted to a Method object
            auto methodHandle = (Method^)method->Call("get_MethodHandle");

            if (methodHandle == nullptr) {
                continue;
            }

            if (methodHandle->GetRaw() == this->GetRaw()) {
                return method;
            }
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

::reframework::InvokeRet Method::Invoke_Internal(System::Object^ obj, array<System::Object^>^ args) {
    if (obj == nullptr && !this->IsStatic()) {
        System::String^ declaringName = this->GetDeclaringType() != nullptr ? this->GetDeclaringType()->GetFullName() : gcnew System::String("UnknownType");
        System::String^ errorStr = "Cannot invoke a non-static method without an object (" + declaringName + "." + this->GetName() + ")";
        REFrameworkNET::API::LogError(errorStr);
        throw gcnew System::InvalidOperationException(errorStr);
    }

    std::vector<void*> args2{};

    if (args != nullptr && args->Length > 0) {
        args2.resize(args->Length);

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
                auto iobj = safe_cast<REFrameworkNET::IObject^>(args[i]);
                args2[i] = iobj->Ptr();
            } else if (t == REFrameworkNET::TypeDefinition::typeid) {
                // TypeDefinitions are wrappers for System.RuntimeTypeHandle
                // However there's basically no functions that actually take a System.RuntimeTypeHandle
                // so we will just convert it to a System.Type.
                auto td = safe_cast<REFrameworkNET::TypeDefinition^>(args[i]);

                if (auto rt = td->GetRuntimeType(); rt != nullptr) {
                    args2[i] = rt->Ptr();
                } else {
                    System::Console::WriteLine("TypeDefinition has no runtime type @ arg " + i);
                }
            } else if (t == System::String::typeid) {
                auto createdStr = VM::CreateString(safe_cast<System::String^>(args[i]));

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

        if (obj_t == System::IntPtr::typeid) {
            obj_ptr = (void*)(intptr_t)safe_cast<System::IntPtr>(obj).ToPointer();
        } else if (obj_t == System::UIntPtr::typeid) {
            obj_ptr = (void*)(uintptr_t)safe_cast<System::UIntPtr>(obj).ToUInt64();
        } else if (obj_t == REFrameworkNET::ManagedObject::typeid) {
            obj_ptr = safe_cast<REFrameworkNET::ManagedObject^>(obj)->Ptr();
        } else if (obj_t == REFrameworkNET::NativeObject::typeid) {
            obj_ptr = safe_cast<REFrameworkNET::NativeObject^>(obj)->Ptr();
        } else {
            System::Console::WriteLine("Unknown type passed to method invocation @ obj");
        }
    } catch (System::Exception^ e) {
        System::Console::WriteLine("Error converting object: " + e->Message);
    }

    return m_method->invoke((reframework::API::ManagedObject*)obj_ptr, args2);
}

bool Method::HandleInvokeMember_Internal(System::Object^ obj, array<System::Object^>^ args, System::Object^% result) {
    auto tempResult = this->Invoke_Internal(obj, args);
    auto returnType = this->GetReturnType();

    if (returnType == nullptr) {
        // box the result
        result = safe_cast<System::Object^>(REFrameworkNET::InvokeRet::FromNative(tempResult));
        return true;
    }

    // Check the return type of the method and return it as a NativeObject if possible
    if (!returnType->IsValueType()) {
        if (tempResult.qword == 0) {
            result = nullptr;
            return true;
        }

        if (returnType->GetVMObjType() == VMObjType::Object) {
            result = REFrameworkNET::ManagedObject::Get((::REFrameworkManagedObjectHandle)tempResult.qword);
            return true;
        }

        if (returnType->GetVMObjType() == VMObjType::String) {
            // Maybe don't create the GC version and just use the native one?
            auto strObject = REFrameworkNET::ManagedObject::Get((::REFrameworkManagedObjectHandle)tempResult.qword);
            auto strType = strObject->GetTypeDefinition();
            const auto firstCharField = strType->GetField("_firstChar");
            uint32_t offset = 0;

            if (firstCharField != nullptr) {
                offset = strType->GetField("_firstChar")->GetOffsetFromBase();
            } else {
                const auto fieldOffset = *(uint32_t*)(*(uintptr_t*)tempResult.qword - sizeof(void*));
                offset = fieldOffset + 4;
            }

            wchar_t* chars = (wchar_t*)((uintptr_t)strObject->Ptr() + offset);
            result = gcnew System::String(chars);
            return true;
        }

        if (returnType->GetVMObjType() == VMObjType::Array) {
            // TODO? Implement array
            result = REFrameworkNET::ManagedObject::Get((::REFrameworkManagedObjectHandle)tempResult.qword);
            return true;
        }

        // TODO: other managed types
        result = gcnew REFrameworkNET::NativeObject((uintptr_t)tempResult.qword, returnType);
        return true;
    }

    if (returnType->IsEnum()) {
        if (auto underlying = returnType->GetUnderlyingType(); underlying != nullptr) {
            returnType = underlying; // easy mode
        }
    }

    const auto raw_rt = (reframework::API::TypeDefinition*)returnType;

    #define CONCAT_X_C(X, DOT, C) X ## DOT ## C

    #define MAKE_TYPE_HANDLER_2(X, C, Y, Z) \
        case CONCAT_X_C(#X, ".", #C)_fnv: \
            result = gcnew X::C((Y)tempResult.Z); \
            break;

    switch (REFrameworkNET::hash(raw_rt->get_full_name().c_str())) {
    MAKE_TYPE_HANDLER_2(System, Boolean, bool, byte)
    MAKE_TYPE_HANDLER_2(System, Byte, uint8_t, byte)
    MAKE_TYPE_HANDLER_2(System, UInt16, uint16_t, word)
    MAKE_TYPE_HANDLER_2(System, UInt32, uint32_t, dword)
    MAKE_TYPE_HANDLER_2(System, UInt64, uint64_t, qword)
    MAKE_TYPE_HANDLER_2(System, SByte, int8_t, byte)
    MAKE_TYPE_HANDLER_2(System, Int16, int16_t, word)
    MAKE_TYPE_HANDLER_2(System, Int32, int32_t, dword)
    MAKE_TYPE_HANDLER_2(System, Int64, int64_t, qword)
    // Because invoke wrappers returning a single actually return a double
    // for consistency purposes
    MAKE_TYPE_HANDLER_2(System, Single, double, d)
    MAKE_TYPE_HANDLER_2(System, Double, double, d)
    case "System.RuntimeTypeHandle"_fnv: {
        result = gcnew REFrameworkNET::TypeDefinition((::REFrameworkTypeDefinitionHandle)tempResult.qword);
        break;
    }
    case "System.RuntimeMethodHandle"_fnv: {
        result = gcnew REFrameworkNET::Method((::REFrameworkMethodHandle)tempResult.qword);
        break;
    }
    case "System.RuntimeFieldHandle"_fnv: {
        result = gcnew REFrameworkNET::Field((::REFrameworkFieldHandle)tempResult.qword);
        break;
    }
    default:
        result = safe_cast<System::Object^>(REFrameworkNET::InvokeRet::FromNative(tempResult));
        break;
    }

    return true;
}
}