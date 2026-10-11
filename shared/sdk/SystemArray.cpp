#include "RETypeDB.hpp"

#include "reframework/API.hpp"
#include "SystemArray.hpp"

size_t sdk::SystemArray::get_size() {
    static auto system_array_type = sdk::find_type_definition("System.Array");
    static auto get_length_method = system_array_type->get_method("GetLength");

    return (size_t)get_length_method->call_safe<int32_t>(sdk::get_thread_context(), this, 0);
}

::REManagedObject* sdk::SystemArray::get_element(int32_t index) {
    if (index < 0 || index >= get_size()) {
        return nullptr;
    }

    static auto system_array_type = sdk::find_type_definition("System.Array");
    static auto get_element_method = system_array_type->get_method("GetValue(System.Int32)");

    return get_element_method->call_safe<::REManagedObject*>(sdk::get_thread_context(), this, index);
}

void sdk::SystemArray::set_element(int32_t index, ::REManagedObject* value) {
    if (index < 0 || index >= get_size()) {
        throw std::out_of_range("index out of range");
    }

    static auto system_array_type = sdk::find_type_definition("System.Array");
    static auto set_element_method = system_array_type->get_method("SetValue(System.Object, System.Int32)");

    if (set_element_method != nullptr) {
        set_element_method->call_safe<void>(sdk::get_thread_context(), this, value, index);
    } else {
        auto t = this->get_type_definition();

        if (t == nullptr) {
            throw std::runtime_error("SystemArray::set_element: failed to get type definition");
        }

        auto set_method = t->get_method("Set");

        if (set_method != nullptr) {
            static auto system_int32 = sdk::find_type_definition("System.Int32");
            static auto system_object = sdk::find_type_definition("System.Object");
            const auto params = set_method->get_param_types();

            if (params.size() == 2 && params[0] == system_int32) {
                const auto elem_type = params[1];

                // Reject type-confused stores (Int32 box into String[] etc).
                // Same-size value types stay allowed for enum integer interop.
                // System.Object[] is exempt: universal base, accepts anything.
                if (value != nullptr && elem_type != nullptr && elem_type != system_object) {
                    const auto value_type = value->get_type_definition();

                    if (value_type != nullptr && !value_type->is_a(elem_type)) {
                        const bool payload_compatible = elem_type->is_value_type()
                            && value_type->is_value_type()
                            && elem_type->get_valuetype_size() == value_type->get_valuetype_size();

                        if (!payload_compatible) {
                            throw std::runtime_error("SystemArray::set_element: cannot store "
                                + value_type->get_full_name() + " into a "
                                + elem_type->get_full_name() + " array");
                        }
                    }
                }

                if (elem_type == system_object) {
                    // Ref array: slot holds the pointer, so add_ref the new value
                    // (must outlive the caller's temp Lua ref) and release the old.
                    if (value != nullptr) {
                        value->add_ref();
                    }

                    auto old = get_element(index);
                    set_method->call_safe<void>(sdk::get_thread_context(), this, index, value);
                    if (old != nullptr) {
                        old->release();
                    }
                    return;
                } else {
                    // Otherwise, need to pass an unboxed value.
                    if (elem_type->should_pass_by_pointer()) {
                        if (elem_type->is_value_type()) {
                            // Struct: slot holds an inline payload copy, no ref.
                            set_method->call_safe<void>(sdk::get_thread_context(), this, index, value->get_field_ptr());
                            return;
                        } else {
                            // Ref array by pointer: same ownership as above.
                            if (value != nullptr) {
                                value->add_ref();
                            }

                            auto old = get_element(index);
                            set_method->call_safe<void>(sdk::get_thread_context(), this, index, value);
                            if (old != nullptr) {
                                old->release();
                            }
                            return;
                        }
                    } else {
                        // Inline value array: slot holds a payload copy, no ref.
                        std::vector<void*> invoke_args;
                        invoke_args.push_back((void*)(intptr_t)index);

                        switch (elem_type->get_valuetype_size()) {
                            case 1: {
                                const auto v = *(uint8_t*)value->get_field_ptr();
                                invoke_args.push_back((void*)(intptr_t)v);
                                break;
                            }
                            case 2: {
                                const auto v = *(uint16_t*)value->get_field_ptr();
                                invoke_args.push_back((void*)(intptr_t)v);
                                break;
                            }
                            case 4: {
                                if (elem_type->get_full_name() == "System.Single") {
                                    const auto f = *(float*)value->get_field_ptr();
                                    const auto d = (double)f;
                                    invoke_args.push_back(*(void**)&d);
                                } else {
                                    const auto v = *(uint32_t*)value->get_field_ptr();
                                    invoke_args.push_back((void*)(intptr_t)v);
                                }
                                break;
                            }
                            case 8: {
                                const auto v = *(uint64_t*)value->get_field_ptr();
                                invoke_args.push_back((void*)(uintptr_t)v);
                                break;
                            }
                            default: {
                                throw std::runtime_error("Unhandled valuetype size for SystemArray::set_element");
                            }
                        }

                        set_method->invoke(this, invoke_args);
                        return;
                    }
                }
            }
        }

        throw std::runtime_error("SystemArray::set_element: failed to find SetValue or Set method");
    }
}

std::vector<::REManagedObject*> sdk::SystemArray::get_elements() {
    std::vector<::REManagedObject*> elements{};
    const auto size = get_size();

    for (size_t i = 0; i < size; i++) {
        elements.push_back(get_element(i));
    }

    return elements;
}

