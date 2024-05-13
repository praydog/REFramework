#include "ManagedObject.hpp"
#include "Utility.hpp"

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

        return Utility::BoxData((uintptr_t*)raw_data, this->Type, false);
    }

    void Field::SetDataBoxed(uintptr_t obj, System::Object^ value, bool isValueType) {
        auto data_ptr = GetDataRaw(obj, isValueType);

        if (data_ptr == 0) {
            return;
        }

        const auto field_type = this->GetType();

        if (field_type == nullptr) {
            return;
        }

        if (!field_type->IsValueType()) {
            const auto iobject = dynamic_cast<REFrameworkNET::IObject^>(value);

            if (iobject != nullptr) {
                *(uintptr_t*)data_ptr = iobject->GetAddress();
            }

            return; // Don't think there's any other way to set a reference type
        }

        const uintptr_t addr = IsStatic() ? 0 : obj;
        const auto vm_obj_type = field_type->GetVMObjType();

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
            break;
        }
    }
}