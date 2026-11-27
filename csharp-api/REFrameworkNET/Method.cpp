#include <algorithm>
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
#include "ValueType.hpp"

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
    // Native copy of a ValueType `this` (behind a fake 0x10 byte object header from TDB 74 on)
    std::vector<uint8_t> fake_boxed_storage;
    size_t fake_header_size{};
    uintptr_t fake_object_ptr{};
    // A real boxed object for methods a reference type declares (see below).
    ::reframework::API::ManagedObject* real_box{};

    if (obj != nullptr) try {
        const auto obj_t = obj->GetType();

        if (REFrameworkNET::IObject::typeid->IsAssignableFrom(obj_t)) {
            auto iobj = static_cast<REFrameworkNET::IObject^>(obj);

            // Since TDB 74, invoke() treats `this` of a value-type instance method as a boxed object and skips its
            // 0x10 byte header, so a ValueType goes in a fake boxed object:
            //   0x00: REObjectInfo* (type info / vtable pointer)
            //   0x08: uint32_t refcount + padding
            //   0x10: actual value type data starts here
            // Before TDB 74 invoke() passes `this` through unchanged: a ValueType goes as its data, and a boxed value
            // (a ManagedObject of a value type) as its data past the header, for methods the value type declares.
            // Methods declared on a reference type (System.Object.GetType, System.Enum.ToString) use `this` as a real
            // object, which the fake header isn't, so a ValueType goes into a real boxed object of its type.
            // ValueType::Ptr() points into a managed array, so the data is copied to native memory either way.
            // This runs for every call on an object, where none of it applies: the declaring type is cached by the
            // method, and the version (TDB::Get() makes a new object) is read only for a value type's `this`.
            const bool value_type_this = !this->IsStatic() && REFrameworkNET::ValueType::typeid->IsAssignableFrom(obj_t);
            const auto declaring_type = this->GetDeclaringType();
            const bool declared_by_value_type = declaring_type != nullptr && declaring_type->IsValueType();
            const bool declared_by_reference_type = declaring_type != nullptr && !declaring_type->IsValueType();
            const bool invoke_skips_header = (value_type_this || declared_by_value_type) && TDB::Get()->GetVersion() >= 74;

            if (value_type_this && declared_by_reference_type) {
                auto vt = static_cast<REFrameworkNET::ValueType^>(iobj);
                auto type_def = vt->GetTypeDefinition();
                auto native_type = (::reframework::API::TypeDefinition*)type_def->Ptr();

                real_box = native_type->create_instance(0);

                if (real_box == nullptr) {
                    throw gcnew System::Exception("could not box the value for " + this->GetName());
                }

                // The new object is local: the engine keeps it until it releases the thread's local objects, after
                // this call. No add_ref()/release() pair, as in the Lua binding, where one crashed RE9 under load.
                memcpy((uint8_t*)real_box + 0x10, vt->Ptr(), type_def->ValueTypeSize);
                obj_ptr = real_box;
            } else if (value_type_this) {
                auto vt = static_cast<REFrameworkNET::ValueType^>(iobj);
                auto type_def = vt->GetTypeDefinition();
                auto vt_size = type_def->ValueTypeSize;
                const bool boxed_this = invoke_skips_header;

                fake_header_size = boxed_this ? 0x10 : 0;
                // At least one byte, so a zero-size value type still gets a valid `this`.
                fake_boxed_storage.resize(std::max<size_t>(fake_header_size + vt_size, 1), 0);

                if (boxed_this) {
                    fake_object_ptr = (uintptr_t)type_def->Ptr();

                    // REObject header: REObjectInfo* at offset 0x00
                    *(void**)&fake_boxed_storage[0x00] = (void*)fake_object_ptr;
                    // REManagedObject: reference count at offset 0x08 (set high to prevent GC interference)
                    *(uint32_t*)&fake_boxed_storage[0x08] = 9999;
                }

                memcpy(&fake_boxed_storage[fake_header_size], vt->Ptr(), vt_size);

                obj_ptr = fake_boxed_storage.data();
            } else if (declared_by_value_type && !invoke_skips_header && !this->IsStatic() &&
                       REFrameworkNET::ManagedObject::typeid->IsAssignableFrom(obj_t)) {
                obj_ptr = (uint8_t*)iobj->Ptr() + 0x10;
            } else {
                obj_ptr = iobj->Ptr();
            }
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

    auto result = m_method->invoke((::reframework::API::ManagedObject*)obj_ptr, std::span<void*>(args2.data(), argcount));

    // If we passed a native copy of a ValueType, copy any mutations back to it
    if ((!fake_boxed_storage.empty() || real_box != nullptr) && obj != nullptr) {
        auto iobj = static_cast<REFrameworkNET::IObject^>(obj);
        if (REFrameworkNET::ValueType::typeid->IsAssignableFrom(obj->GetType())) {
            auto vt = static_cast<REFrameworkNET::ValueType^>(iobj);
            auto vt_size = vt->GetTypeDefinition()->ValueTypeSize;
            const auto src = real_box != nullptr ? (uint8_t*)real_box + 0x10 : &fake_boxed_storage[fake_header_size];
            memcpy(vt->Ptr(), src, vt_size);
        }
    }

    return result;
}

bool Method::HandleInvokeMember_Internal(System::Object^ obj, array<System::Object^>^ args, System::Object^% result) {
    auto tempResult = this->Invoke_Internal(obj, args);
    auto returnType = this->GetReturnType();

    if (returnType == nullptr) {
        // box the result
        result = static_cast<System::Object^>(REFrameworkNET::InvokeRet::FromNative(tempResult));
        return true;
    }

    // Detect byref (ref T) returns via bit 63 tag.
    // RE Engine tags interior pointers with bts rax, 0x3F. No valid user-space
    // pointer on Windows x64 has bit 63 set, so this is a reliable indicator.
    // The tagged pointer points into array/object storage; dereference it to get
    // the actual data and pass that address to BoxData.
    auto rawPtr = *(uintptr_t*)tempResult.bytes.data();

    if (!returnType->IsValueType() && rawPtr != 0 && (rawPtr & (1ULL << 63)) != 0) {
        uintptr_t interior = rawPtr & ~(1ULL << 63);

        if (interior == 0 || IsBadReadPtr((void*)interior, sizeof(void*))) {
            result = nullptr;
            return true;
        }

        result = Utility::BoxData((uintptr_t*)interior, returnType, true);
        return true;
    }

    result = Utility::BoxData((uintptr_t*)tempResult.bytes.data(), returnType, true);
    return true;
}
}