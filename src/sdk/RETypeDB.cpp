#include "REFramework.hpp"

#include "RETypeDB.hpp"

namespace sdk {
sdk::RETypeDefinition* REField::get_declaring_type() const {
    auto tdb = g_framework->get_types()->get_type_db();

    if (this->declaring_typeid == 0) {
        return nullptr;
    }

    return &(*tdb->types)[this->declaring_typeid];
}

sdk::RETypeDefinition* REField::get_type() const {
    auto tdb = g_framework->get_types()->get_type_db();

#if TDB_VER >= 69
    auto& impl = (*tdb->fieldsImpl)[this->impl_id];
    const auto field_typeid = impl.field_typeid;
#else
    const auto field_typeid = this->field_typeid;
#endif

    if (field_typeid == 0) {
        return nullptr;
    }

    return &(*tdb->types)[field_typeid];
}

const char* REField::get_name() const {
    auto tdb = g_framework->get_types()->get_type_db();

#if TDB_VER >= 69
    auto& impl = (*tdb->fieldsImpl)[this->impl_id];
    const auto name_offset = impl.name_offset;
#else
    const auto name_offset = this->name_offset;
#endif

    return Address{tdb->stringPool}.get(name_offset).as<const char*>();
}

uint32_t REField::get_flags() const {
    auto tdb = g_framework->get_types()->get_type_db();

#if TDB_VER >= 69
    auto& impl = (*tdb->fieldsImpl)[this->impl_id];
    return impl.flags;
#else
    return this->flags;
#endif
}

void* REField::get_init_data() const {
    auto tdb = g_framework->get_types()->get_type_db();

#if TDB_VER >= 69
    auto& impl = (*tdb->fieldsImpl)[this->impl_id];
    const auto init_data_index = impl.init_data_lo | (impl.init_data_hi << 14);
#else
    const auto init_data_index = this->init_data_index;
#endif

    const auto init_data_offset = (*tdb->initData)[init_data_index];
    auto init_data = &(*tdb->bytePool)[init_data_offset];

    // WACKY
    if (init_data_offset < 0) {
        init_data = &((uint8_t*)tdb->stringPool)[init_data_offset * -1];
    }

    return init_data;
}

uint32_t REField::get_offset_from_fieldptr() const {
    return this->offset;
}

uint32_t REField::get_offset_from_base() const {
    auto offset_from_fieldptr = get_offset_from_fieldptr();
    auto declaring_type = this->get_declaring_type();

    if (declaring_type != nullptr) {
        return declaring_type->get_fieldptr_offset() + offset_from_fieldptr;
    }

    return offset_from_fieldptr;
}

void* REField::get_data_raw(void* object, bool is_value_type) const {
    const auto field_flags = get_flags();

    if ((field_flags & (uint16_t)via::clr::FieldFlag::Static) != 0) {
        if ((field_flags & (uint16_t)via::clr::FieldFlag::Literal) != 0) {
            return this->get_init_data();
        }

        auto tbl = sdk::REGlobalContext::get()->get_static_tbl_for_type(this->get_declaring_type()->index);

        if (tbl != nullptr) {
            return Address{tbl}.get(this->get_offset_from_fieldptr());
        }
    } else {
        if (is_value_type) {
            return Address{object}.get(this->get_offset_from_fieldptr());
        }

        return Address{object}.get(this->get_offset_from_base());
    }

    return nullptr;
}
} // namespace sdk