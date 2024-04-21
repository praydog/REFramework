#include "API.hpp"

#include "TypeDefinition.hpp"
#include "UnifiedObject.hpp"

#include "./Utility.hpp"

namespace REFrameworkNET {
    REFrameworkNET::InvokeRet^ UnifiedObject::Invoke(System::String^ methodName, array<System::Object^>^ args) {
        // Get method
        auto t = this->GetTypeDefinition();
        if (t == nullptr) {
            return nullptr;
        }

        auto m = t->GetMethod(methodName);

        if (m == nullptr) {
            return nullptr;
        }

        return m->Invoke(this, args);
    }

    bool UnifiedObject::HandleInvokeMember_Internal(System::String^ methodName, array<System::Object^>^ args, System::Object^% result) {
        auto t = this->GetTypeDefinition();

        if (t == nullptr) {
            return false;
        }

        auto method = t->FindMethod(methodName);

        if (method != nullptr)
        {
            // Re-used with UnifiedObject::TryInvokeMember
            return method->HandleInvokeMember_Internal(this, args, result);
        }

        REFrameworkNET::API::LogInfo("Method not found: " + methodName);

        result = nullptr;
        return false;
    }

    bool UnifiedObject::HandleInvokeMember_Internal(uint32_t methodIndex, array<System::Object^>^ args, System::Object^% result) {
        auto method = REFrameworkNET::TDB::Get()->GetMethod(methodIndex);

        if (method != nullptr)
        {
            // Re-used with UnifiedObject::TryInvokeMember
            return method->HandleInvokeMember_Internal(this, args, result);
        }

        result = nullptr;
        return false;
    }

    bool UnifiedObject::HandleInvokeMember_Internal(System::Object^ methodObj, array<System::Object^>^ args, System::Object^% result) {
        auto method = dynamic_cast<REFrameworkNET::Method^>(methodObj);
        if (method != nullptr)
        {
            // Re-used with UnifiedObject::TryInvokeMember
            return method->HandleInvokeMember_Internal(this, args, result);
        }

        result = nullptr;
        return false;
    }

    bool UnifiedObject::TryInvokeMember(System::Dynamic::InvokeMemberBinder^ binder, array<System::Object^>^ args, System::Object^% result)
    {
        auto methodName = binder->Name;
        return HandleInvokeMember_Internal(methodName, args, result);
    }

    bool UnifiedObject::TryGetMember(System::Dynamic::GetMemberBinder^ binder, System::Object^% result) {
        auto memberName = binder->Name;
        return HandleTryGetMember_Internal(memberName, result);
    }

    bool UnifiedObject::HandleTryGetMember_Internal(System::String^ memberName, System::Object^% result) {
        auto t = this->GetTypeDefinition();

        if (t == nullptr) {
            return false;
        }

        auto field = t->FindField(memberName);

        if (field != nullptr)
        {
            const auto field_type = field->GetType();

            if (field_type == nullptr) {
                return false;
            }

            const auto raw_ft = (reframework::API::TypeDefinition*)field_type;
            const uintptr_t addr = field->IsStatic() ? 0 : this->GetAddress();
            const auto vm_obj_type = field_type->GetVMObjType();

            #define MAKE_TYPE_HANDLER(X, Y) \
                case ##X##_fnv: \
                    result = gcnew Y(field->GetData<##Y##>(addr, field_type->IsValueType())); \
                    break;

            switch (REFrameworkNET::hash(raw_ft->get_full_name())) {
                MAKE_TYPE_HANDLER("System.Boolean", bool)
                MAKE_TYPE_HANDLER("System.Byte", uint8_t)
                MAKE_TYPE_HANDLER("System.SByte", int8_t)
                MAKE_TYPE_HANDLER("System.Int16", int16_t)
                MAKE_TYPE_HANDLER("System.UInt16", uint16_t)
                MAKE_TYPE_HANDLER("System.Int32", int32_t)
                MAKE_TYPE_HANDLER("System.UInt32", uint32_t)
                MAKE_TYPE_HANDLER("System.Int64", int64_t)
                MAKE_TYPE_HANDLER("System.UInt64", uint64_t)
                MAKE_TYPE_HANDLER("System.Single", float)
                MAKE_TYPE_HANDLER("System.Double", double)
                MAKE_TYPE_HANDLER("System.Char", wchar_t)
                MAKE_TYPE_HANDLER("System.IntPtr", intptr_t)
                MAKE_TYPE_HANDLER("System.UIntPtr", uintptr_t)
                case "System.String"_fnv:
                    {
                        if (field->IsLiteral()) {
                            result = gcnew System::String((const char*)field->GetInitDataPtr());
                            break;
                        }

                        // TODO: Check if this half of it works
                        auto strObject = field->GetData<reframework::API::ManagedObject*>(addr, field_type->IsValueType());

                        if (strObject == nullptr) {
                            result = nullptr;
                            break;
                        }

                        const auto firstCharField = field_type->GetField("_firstChar");
                        uint32_t offset = 0;

                        if (firstCharField != nullptr) {
                            offset = field_type->IsValueType() ? firstCharField->GetOffsetFromFieldPtr() : firstCharField->GetOffsetFromBase();
                        } else {
                            const auto fieldOffset = *(uint32_t*)(*(uintptr_t*)strObject - sizeof(void*));
                            offset = fieldOffset + 4;
                        }

                        wchar_t* chars = (wchar_t*)((uintptr_t)strObject + offset);
                        result = gcnew System::String(chars);
                        break;
                    }
                default:
                    if (vm_obj_type > VMObjType::NULL_ && vm_obj_type < VMObjType::ValType) {
                        switch (vm_obj_type) {
                        case VMObjType::Array:
                            /*
                            Just return it as an managed object for now
                            */
                        default: {
                            //const auto td = utility::re_managed_object::get_type_definition(*(::REManagedObject**)data);
                            auto& obj = field->GetData<reframework::API::ManagedObject*>(addr, field_type->IsValueType());

                            if (obj == nullptr) {
                                result = nullptr;
                                break;
                            }

                            auto td = TypeDefinition::GetInstance(obj->get_type_definition());

                            // another fallback incase the method returns an object which is an array
                            if (td != nullptr && td->GetVMObjType() == VMObjType::Array) {
                                /*
                                Just return it as an managed object for now
                                */
                            }

                            result = ManagedObject::Get(obj);
                            break;
                        }
                        }
                    } else {
                        switch (field_type->GetSize()) {
                        case 8:
                            result = gcnew System::UInt64(field->GetData<uint64_t>(addr, field_type->IsValueType()));
                            break;
                        case 4:
                            result = gcnew System::UInt32(field->GetData<uint32_t>(addr, field_type->IsValueType()));
                            break;
                        case 2:
                            result = gcnew System::UInt16(field->GetData<uint16_t>(addr, field_type->IsValueType()));
                            break;
                        case 1:
                            result = gcnew System::Byte(field->GetData<uint8_t>(addr, field_type->IsValueType()));
                            break;
                        default:
                            result = nullptr;
                            break;
                        }

                        break;
                    }
            };

            return true;
        }

        /*auto property = t->FindProperty(memberName);

        if (property != nullptr)
        {
            result = property->GetValue(this);
            return true;
        }*/

        REFrameworkNET::API::LogInfo("Member not found: " + memberName);

        result = nullptr;
        return false;
    }

    bool UnifiedObject::TrySetMember(System::Dynamic::SetMemberBinder^ binder, System::Object^ value)
    {
        auto memberName = binder->Name;
        auto t = this->GetTypeDefinition();

        if (t == nullptr) {
            return false;
        }

        auto field = t->FindField(memberName);

        if (field != nullptr)
        {
            const auto field_type = field->GetType();

            if (field_type == nullptr) {
                return false;
            }

            const auto raw_ft = (reframework::API::TypeDefinition*)field_type;
            const uintptr_t addr = field->IsStatic() ? 0 : this->GetAddress();
            const auto vm_obj_type = field_type->GetVMObjType();

            #define MAKE_TYPE_HANDLER_SET(X, Y) \
                case ##X##_fnv: \
                    field->GetData<##Y##>(addr, field_type->IsValueType()) = (Y)value; \
                    break;
            
            switch (REFrameworkNET::hash(raw_ft->get_full_name())) {
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

            return true;
        }

        return false;
    }
}