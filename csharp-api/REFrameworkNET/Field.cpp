#include "ManagedObject.hpp"
#include "Utility.hpp"
#include "ValueType.hpp"
#include "Proxy.hpp"

#include "Field.hpp"

namespace REFrameworkNET {
    generic <typename T>
    T Field::GetDataT(uintptr_t obj, bool isValueType) {
        const auto raw_data = GetDataRaw(obj, isValueType);
        if (raw_data == 0) {
            return T();
        }
        
        if (T::typeid->IsValueType) {
            return (T)System::Runtime::InteropServices::Marshal::PtrToStructure(System::IntPtr((void*)raw_data), T::typeid);
        }

        auto mo = ManagedObject::Get<ManagedObject>((reframework::API::ManagedObject*)raw_data);

        if (T::typeid == ManagedObject::typeid) {
            return (T)mo;
        }

        //throw gcnew System::NotImplementedException();
        return mo->As<T>();
    }

    System::Object^ Field::GetDataBoxed(uintptr_t obj, bool isValueType) {
        auto raw_data = GetDataRaw(obj, isValueType);

        if (raw_data == 0) {
            return nullptr;
        }

        return Utility::BoxData((uintptr_t*)raw_data, this->Type, false, this);
    }

    System::Object^ Field::GetDataBoxed(System::Type^ targetReturnType, uintptr_t obj, bool isValueType) {
        return Utility::TranslateBoxedData(targetReturnType, GetDataBoxed(obj, isValueType));
    }

    void Field::SetDataBoxed(uintptr_t obj, System::Object^ value, bool isValueType) {
        auto data_ptr = GetDataRaw(obj, isValueType);

        if (data_ptr == 0) {
            return;
        }

        auto field_type = this->GetType();

        if (field_type == nullptr) {
            return;
        }

        if (!field_type->IsValueType()) {
            if (value == nullptr) {
                // Lightweight managed object
                auto originalObject = *(reframework::API::ManagedObject**)data_ptr;

                if (originalObject != nullptr && originalObject->get_ref_count() > 0) {
                    originalObject->release();
                }

                *(uintptr_t*)data_ptr = 0;
                return;
            }

            auto iobject = dynamic_cast<REFrameworkNET::IObject^>(value);

            if (iobject != nullptr) {
                // Lightweight managed object
                auto originalObject = *(reframework::API::ManagedObject**)data_ptr;

                if (originalObject != nullptr && originalObject->get_ref_count() > 0) {
                    originalObject->release();
                }

                *(uintptr_t*)data_ptr = iobject->GetAddress();

                // Add a reference onto the object now that something else is holding onto it
                auto proxy = dynamic_cast<REFrameworkNET::IProxy^>(iobject);
                auto managedObject = proxy != nullptr ? dynamic_cast<REFrameworkNET::ManagedObject^>(proxy->GetInstance()) : dynamic_cast<REFrameworkNET::ManagedObject^>(iobject);

                if (managedObject != nullptr) {
                    managedObject->Globalize(); // Globalize it if it's not already
                    managedObject->AddRef(); // Add a "dangling" reference
                }
            }

            return; // Don't think there's any other way to set a reference type
        }

        auto underlying_type = field_type->GetUnderlyingType();

        // For enums
        if (underlying_type != nullptr) {
            field_type = underlying_type;
        }

        const uintptr_t addr = IsStatic() ? 0 : obj;

        #define MAKE_TYPE_HANDLER_SET(X, Y) \
            case ##X##_fnv: \
                this->GetData<##Y##>(addr, isValueType) = (Y)value; \
                break;
        
        switch (field_type->GetFNV64Hash()) {
            MAKE_TYPE_HANDLER_SET("System.Boolean", bool)
            MAKE_TYPE_HANDLER_SET("System.Byte", uint8_t)
            MAKE_TYPE_HANDLER_SET("System.SByte", int8_t)
            MAKE_TYPE_HANDLER_SET("System.Int16", int16_t)
            MAKE_TYPE_HANDLER_SET("System.UInt16", uint16_t)
            MAKE_TYPE_HANDLER_SET("System.Int32", int32_t)
            MAKE_TYPE_HANDLER_SET("System.UInt32", uint32_t)
            MAKE_TYPE_HANDLER_SET("System.Int64", int64_t)
            MAKE_TYPE_HANDLER_SET("System.UInt64", uint64_t)
            MAKE_TYPE_HANDLER_SET("System.Single", float)
            MAKE_TYPE_HANDLER_SET("System.Double", double)
            MAKE_TYPE_HANDLER_SET("System.Char", wchar_t)
            MAKE_TYPE_HANDLER_SET("System.IntPtr", intptr_t)
            MAKE_TYPE_HANDLER_SET("System.UIntPtr", uintptr_t)

        default:
        {
            const auto iobject = dynamic_cast<REFrameworkNET::IObject^>(value);
            const auto iobject_td = iobject != nullptr ? iobject->GetTypeDefinition() : nullptr;

            if (iobject != nullptr && iobject_td == field_type) {
                if (iobject->IsManaged()) {
                    memcpy((void*)data_ptr, (void*)((uintptr_t)iobject->Ptr() + iobject_td->GetFieldPtrOffset()), field_type->ValueTypeSize);
                } else {
                    memcpy((void*)data_ptr, iobject->Ptr(), field_type->ValueTypeSize);
                }
            }

            break;
        }
        }
    }
}