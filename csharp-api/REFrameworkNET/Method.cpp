#include "ManagedObject.hpp"

#include "Method.hpp"

namespace REFrameworkNET {
REFrameworkNET::InvokeRet^ Method::Invoke(System::Object^ obj, array<System::Object^>^ args) {
    // We need to convert the managed objects to 8 byte representations
    std::vector<void*> args2{};
    args2.resize(args->Length);

    for (int i = 0; i < args->Length; ++i) {
        //args2[i] = args[i]->ptr();
        const auto t = args[i]->GetType();

        if (t == System::Byte::typeid) {
            uint8_t v = System::Convert::ToByte(args[i]);
            args2[i] = (void*)(uint64_t)v;
        } else if (t == System::UInt16::typeid) {
            uint16_t v = System::Convert::ToUInt16(args[i]);
            args2[i] = (void*)(uint64_t)v;
        } else if (t == System::UInt32::typeid) {
            uint32_t v = System::Convert::ToUInt32(args[i]);
            args2[i] = (void*)(uint64_t)v;
        } else if (t == System::Single::typeid) {
            float v = System::Convert::ToSingle(args[i]);
            args2[i] = (void*)(uint64_t)v;
        } else if (t == System::UInt64::typeid) {
            uint64_t v = System::Convert::ToUInt64(args[i]);
            args2[i] = (void*)(uint64_t)v;
        } else if (t == System::Double::typeid) {
            double v = System::Convert::ToDouble(args[i]);
            args2[i] = (void*)(uint64_t)v;
        } else if (t == System::IntPtr::typeid) {
            args2[i] = (void*)(uint64_t)System::Convert::ToInt64(args[i]);
        } else {
            //args2[i] = args[i]->ptr();
        }
    }

    void* obj_ptr = nullptr;

    if (obj != nullptr) {
        const auto obj_t = obj->GetType();
        if (obj_t == System::IntPtr::typeid || obj_t == System::UIntPtr::typeid) {
            obj_ptr = (void*)(uint64_t)System::Convert::ToInt64(obj);
        } else if (obj_t == REFrameworkNET::ManagedObject::typeid) {
            obj_ptr = safe_cast<REFrameworkNET::ManagedObject^>(obj)->Ptr();
        } else {
            //obj_ptr = obj->ptr();
        }
    }

    const auto native_result = m_method->invoke((reframework::API::ManagedObject*)obj_ptr, args2);

    return gcnew REFrameworkNET::InvokeRet(native_result);
}
}