#include "RETypeDB.hpp"

#include "SystemArray.hpp"

int32_t sdk::SystemArray::size() {
    static auto system_array_type = sdk::RETypeDB::get()->find_type("System.Array");
    static auto get_length_method = system_array_type->get_method("GetLength");

    return get_length_method->call<int32_t>(sdk::get_thread_context(), this, 0);
}

::REManagedObject* sdk::SystemArray::get_element(int32_t index) {
    static auto system_array_type = sdk::RETypeDB::get()->find_type("System.Array");
    static auto get_element_method = system_array_type->get_method("GetValue(System.Int32)");

    return get_element_method->call<::REManagedObject*>(sdk::get_thread_context(), this, index);
}

void sdk::SystemArray::set_element(int32_t index, ::REManagedObject* value) {
    static auto system_array_type = sdk::RETypeDB::get()->find_type("System.Array");
    static auto set_element_method = system_array_type->get_method("SetValue(System.Object, System.Int32)");

    set_element_method->call<void>(sdk::get_thread_context(), this, value, index);
}

std::vector<::REManagedObject*> sdk::SystemArray::get_elements() {
    std::vector<::REManagedObject*> elements{};

    for (int32_t i = 0; i < size(); i++) {
        elements.push_back(get_element(i));
    }

    return elements;
}

