#include <bit>

#include "ManagedObject.hpp"
#include "NativeObject.hpp"

#include "Method.hpp"
#include "Field.hpp"

#include "API.hpp"

#include "Utility.hpp"

namespace REFrameworkNET {
REFrameworkNET::InvokeRet^ Method::Invoke(System::Object^ obj, array<System::Object^>^ args) {
    if (obj == nullptr && !this->IsStatic()) {
        System::String^ declaringName = this->GetDeclaringType() != nullptr ? this->GetDeclaringType()->GetFullName() : "Unknown";
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
            const auto t = args[i]->GetType();

            if (t == REFrameworkNET::ManagedObject::typeid) {
                args2[i] = safe_cast<REFrameworkNET::ManagedObject^>(args[i])->Ptr();
            } else if (t == System::Boolean::typeid) {
                bool v = System::Convert::ToBoolean(args[i]);
                args2[i] = (void*)(intptr_t)v;
            } else if (t == System::Int32::typeid) {
                int32_t v = System::Convert::ToInt32(args[i]);
                args2[i] = (void*)(intptr_t)v;
            } else if (t == System::Byte::typeid) {
                uint8_t v = System::Convert::ToByte(args[i]);
                args2[i] = (void*)(uint64_t)v;
            } else if (t == System::UInt16::typeid) {
                uint16_t v = System::Convert::ToUInt16(args[i]);
                args2[i] = (void*)(uint64_t)v;
            } else if (t == System::UInt32::typeid) {
                uint32_t v = System::Convert::ToUInt32(args[i]);
                args2[i] = (void*)(uint64_t)v;
            } else if (t == System::Single::typeid) {
                // this might seem counterintuitive, converting a float to a double
                // but the invoke wrappers ALWAYS expect a double, so we need to do this
                // even when they take a System.Single, the wrappers take in a double and convert it to a float
                float v = System::Convert::ToSingle(args[i]);
                auto d = (double)v;
                auto n = *(int64_t*)&d;
                args2[i] = (void*)(uint64_t)n;
            } else if (t == System::UInt64::typeid) {
                uint64_t v = System::Convert::ToUInt64(args[i]);
                args2[i] = (void*)(uint64_t)v;
            } else if (t == System::Double::typeid) {
                double v = System::Convert::ToDouble(args[i]);
                auto n = *(int64_t*)&v;
                args2[i] = (void*)(uint64_t)n;
            } else if (t == System::IntPtr::typeid) {
                args2[i] = (void*)(uint64_t)System::Convert::ToInt64(args[i]);
            } else {
                args2[i] = nullptr;
                System::Console::WriteLine("Unknown type passed to method invocation @ arg " + i);
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

    const auto native_result = m_method->invoke((reframework::API::ManagedObject*)obj_ptr, args2);

    return gcnew REFrameworkNET::InvokeRet(native_result);
}

bool Method::HandleInvokeMember_Internal(System::Object^ obj, System::String^ methodName, array<System::Object^>^ args, System::Object^% result) {
    //auto methodName = binder->Name;
    auto tempResult = this->Invoke(obj, args);

    if (tempResult != nullptr && tempResult->QWord != 0) {
        auto returnType = this->GetReturnType();

        if (returnType == nullptr) {
            result = tempResult;
            return true;
        }

        // Check the return type of the method and return it as a NativeObject if possible
        if (!returnType->IsValueType()) {
            if (returnType->GetVMObjType() == VMObjType::Object) {
                if (tempResult->QWord == 0) {
                    result = nullptr;
                    return true;
                }

                result = gcnew REFrameworkNET::ManagedObject((::REFrameworkManagedObjectHandle)tempResult->QWord);
                return true;
            }

            if (returnType->GetVMObjType() == VMObjType::String) {
                if (tempResult->QWord == 0) {
                    result = nullptr;
                    return true;
                }
                
                // Maybe don't create the GC version and just use the native one?
                auto strObject = gcnew REFrameworkNET::ManagedObject((::REFrameworkManagedObjectHandle)tempResult->QWord);
                auto strType = strObject->GetTypeDefinition();
                const auto offset = strType->GetField("_firstChar")->GetOffsetFromBase();

                wchar_t* chars = (wchar_t*)((uintptr_t)strObject->Ptr() + offset);
                result = gcnew System::String(chars);
                return true;
            }

            // TODO: other managed types
            result = gcnew REFrameworkNET::NativeObject((uintptr_t)tempResult->QWord, returnType);
            return true;
        }

        const auto raw_rt = (reframework::API::TypeDefinition*)returnType;

        #define CONCAT_X_C(X, DOT, C) X ## DOT ## C

        #define MAKE_TYPE_HANDLER_2(X, C, Y, Z) \
            case CONCAT_X_C(#X, ".", #C)_fnv: \
                result = gcnew X::C((Y)tempResult->Z); \
                break;

        switch (REFrameworkNET::hash(raw_rt->get_full_name().c_str())) {
        MAKE_TYPE_HANDLER_2(System, Boolean, bool, Byte)
        MAKE_TYPE_HANDLER_2(System, Byte, uint8_t, Byte)
        MAKE_TYPE_HANDLER_2(System, UInt16, uint16_t, Word)
        MAKE_TYPE_HANDLER_2(System, UInt32, uint32_t, DWord)
        MAKE_TYPE_HANDLER_2(System, UInt64, uint64_t, QWord)
        MAKE_TYPE_HANDLER_2(System, SByte, int8_t, Byte)
        MAKE_TYPE_HANDLER_2(System, Int16, int16_t, Word)
        MAKE_TYPE_HANDLER_2(System, Int32, int32_t, DWord)
        MAKE_TYPE_HANDLER_2(System, Int64, int64_t, QWord)
        // Because invoke wrappers returning a single actually return a double
        // for consistency purposes
        MAKE_TYPE_HANDLER_2(System, Single, double, Double)
        MAKE_TYPE_HANDLER_2(System, Double, double, Double)
        default:
            result = tempResult;
            break;
        }
    }

    return true;
}
}