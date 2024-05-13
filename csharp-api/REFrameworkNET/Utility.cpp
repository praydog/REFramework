#include "ManagedObject.hpp"
#include "NativeObject.hpp"
#include "ValueType.hpp"

#include "TypeDefinition.hpp"
#include "Field.hpp"
#include "Method.hpp"
#include "Proxy.hpp"

#include "Utility.hpp"

namespace REFrameworkNET {
public interface class DummyInterface {

};

System::Object^ Utility::BoxData(uintptr_t* ptr, TypeDefinition^ t, bool fromInvoke, Field^ field) {
    System::Object^ result = nullptr;

    if (t == nullptr) {
        return nullptr;
    }

    // Check the return type of the method and return it as a NativeObject if possible
    if (!t->IsValueType()) {
        if (ptr == nullptr || *ptr == 0) {
            return nullptr;
        }

        const auto vm_object_type = t->GetVMObjType();

        if (vm_object_type == VMObjType::Object) {
            return REFrameworkNET::ManagedObject::Get((::REFrameworkManagedObjectHandle)*ptr);
        }

        // TODO: Clean this up
        if (vm_object_type == VMObjType::String) {
            if (field != nullptr && field->IsLiteral()) {
                return gcnew System::String((const char*)field->GetInitDataPtr());
            }

            auto strObject = (reframework::API::ManagedObject*)*ptr;
            auto strType = strObject->get_type_definition();
            const auto firstCharField = strType->find_field("_firstChar");
            uint32_t offset = 0;

            if (firstCharField != nullptr) {
                offset = firstCharField->get_offset_from_base();
            } else {
                const auto fieldOffset = *(uint32_t*)(*(uintptr_t*)*ptr - sizeof(void*));
                offset = fieldOffset + 4;
            }

            wchar_t* chars = (wchar_t*)((uintptr_t)strObject + offset);
            return gcnew System::String(chars);
        }

        if (vm_object_type == VMObjType::Array) {
            // TODO? Implement array
            return REFrameworkNET::ManagedObject::Get((::REFrameworkManagedObjectHandle)*ptr);
        }

        // TODO: other managed types
        return gcnew REFrameworkNET::NativeObject((uintptr_t)*ptr, t);
    }

    if (t->IsEnum()) {
        if (auto underlying = t->GetUnderlyingType(); underlying != nullptr) {
            t = underlying; // easy mode
        }
    }

    #define CONCAT_X_C(X, DOT, C) X ## DOT ## C

    #define MAKE_TYPE_HANDLER_2(X, C, Y, Z) \
        case CONCAT_X_C(#X, ".", #C)_fnv: \
            result = gcnew X::C(*(Y*)ptr); \
            break;

    switch (t->GetFNV64Hash()) {
    MAKE_TYPE_HANDLER_2(System, Boolean, bool, byte)
    MAKE_TYPE_HANDLER_2(System, Byte, uint8_t, byte)
    MAKE_TYPE_HANDLER_2(System, Char, wchar_t, byte)
    MAKE_TYPE_HANDLER_2(System, UInt16, uint16_t, word)
    MAKE_TYPE_HANDLER_2(System, UInt32, uint32_t, dword)
    MAKE_TYPE_HANDLER_2(System, UInt64, uint64_t, qword)
    MAKE_TYPE_HANDLER_2(System, SByte, int8_t, byte)
    MAKE_TYPE_HANDLER_2(System, Int16, int16_t, word)
    MAKE_TYPE_HANDLER_2(System, Int32, int32_t, dword)
    MAKE_TYPE_HANDLER_2(System, Int64, int64_t, qword)
    MAKE_TYPE_HANDLER_2(System, IntPtr, intptr_t, qword)
    MAKE_TYPE_HANDLER_2(System, UIntPtr, uintptr_t, qword)
    MAKE_TYPE_HANDLER_2(System, Double, double, d)
    // Because invoke wrappers returning a single actually return a double
    // for consistency purposes
    //MAKE_TYPE_HANDLER_2(System, Single, double, d)
    case "System.Single"_fnv: {
        if (fromInvoke) {
            result = gcnew System::Single((float)*(double*)ptr);
            break;
        }
        
        result = gcnew System::Single(*(float*)ptr);
        break;
    }
    case "System.RuntimeTypeHandle"_fnv: {
        if (ptr == 0) {
            return nullptr;
        }

        result = TypeDefinition::GetInstance((::REFrameworkTypeDefinitionHandle)*ptr);
        break;
    }
    case "System.RuntimeMethodHandle"_fnv: {
        if (ptr == 0) {
            return nullptr;
        }

        result = Method::GetInstance((::REFrameworkMethodHandle)*ptr);
        break;
    }
    case "System.RuntimeFieldHandle"_fnv: {
        if (ptr == 0) {
            result = nullptr;
            return true;
        }

        result = Field::GetInstance((::REFrameworkFieldHandle)*ptr);
        break;
    }
    default:
        //result = safe_cast<System::Object^>(REFrameworkNET::InvokeRet::FromNative(tempResult));
        {
            // Here's hoping this works
            auto vt = gcnew REFrameworkNET::ValueType(t);
            memcpy(vt->Ptr(), ptr, t->ValueTypeSize);
            result = vt;
        }
        break;
    }

    return result;
}

System::Object^ Utility::TranslateBoxedData(System::Type^ targetReturnType, System::Object^ result) {
    if (result == nullptr) {
        return nullptr; // ez
    }

    if (targetReturnType == nullptr) {
        return result;
    }

    if (!targetReturnType->IsPrimitive && !targetReturnType->IsEnum && !targetReturnType->IsInterface) {
        if (targetReturnType == System::String::typeid) {
            return result;
        }

        if (targetReturnType == REFrameworkNET::ManagedObject::typeid) {
            return result;
        }
        
        if (targetReturnType == REFrameworkNET::NativeObject::typeid) {
            return result;
        }

        if (targetReturnType == REFrameworkNET::TypeDefinition::typeid) {
            return result;
        }

        if (targetReturnType == REFrameworkNET::Method::typeid) {
            return result;
        }

        if (targetReturnType == REFrameworkNET::Field::typeid) {
            return result;
        }
    }

    if (targetReturnType->IsEnum) {
        auto underlyingType = targetReturnType->GetEnumUnderlyingType();

        if (underlyingType != nullptr) {
            auto underlyingResult = System::Convert::ChangeType(result, underlyingType);
            return System::Enum::ToObject(targetReturnType, underlyingResult);
        }
    }

    if (!targetReturnType->IsPrimitive && targetReturnType->IsInterface) {
        auto iobjectResult = dynamic_cast<REFrameworkNET::IObject^>(result);

        if (iobjectResult != nullptr && targetReturnType->IsInterface) {
            // Caching mechanism to prevent creating multiple proxies for the same object and type so we dont stress the GC
            if (auto existingProxy = iobjectResult->GetProxy(targetReturnType); existingProxy != nullptr) {
                return existingProxy;
            }

            auto proxy = System::Reflection::DispatchProxy::Create(targetReturnType, Proxy<DummyInterface^, IObject^>::typeid->GetGenericTypeDefinition()->MakeGenericType(targetReturnType, result->GetType()));
            ((IProxy^)proxy)->SetInstance(iobjectResult);

            if (auto unified = dynamic_cast<REFrameworkNET::UnifiedObject^>(iobjectResult); unified != nullptr) {
                unified->AddProxy(targetReturnType, (IProxy^)proxy);
            }

            result = proxy;
            return result;
        }
    }

    return result;
}
}