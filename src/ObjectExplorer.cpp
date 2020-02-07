#include <sstream>
#include <fstream>
#include <forward_list>

#include <windows.h>

#include "utility/String.hpp"
#include "utility/Scan.hpp"

#include "REFramework.hpp"
#include "ObjectExplorer.hpp"

ObjectExplorer::ObjectExplorer()
{
    m_type_name.reserve(256);
    m_object_address.reserve(256);
}

void ObjectExplorer::on_draw_ui() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_Once);

    if (!ImGui::CollapsingHeader(get_name().data())) {
        return;
    }

    if (m_do_init) {
        populate_classes();
        populate_enums();
    }

    auto curtime = std::chrono::system_clock::now();

    // List of globals to choose from
    if (ImGui::CollapsingHeader("Singletons")) {
        if (curtime > m_next_refresh) {
            g_framework->get_globals()->safe_refresh();
            m_next_refresh = curtime + std::chrono::seconds(1);
        }

        // make a copy, we want to sort by name
        auto singletons = g_framework->get_globals()->get_objects();

        // first loop, sort
        std::sort(singletons.begin(), singletons.end(), [](REManagedObject** a, REManagedObject** b) {
            auto a_type = utility::re_managed_object::safe_get_type(*a);
            auto b_type = utility::re_managed_object::safe_get_type(*b);

            if (a_type == nullptr || a_type->name == nullptr) {
                return true;
            }

            if (b_type == nullptr || b_type->name == nullptr) {
                return false;
            }

            return std::string_view{ a_type->name } < std::string_view{ b_type->name };
        });

        // Display the nodes
        for (auto obj : singletons) {
            auto t = utility::re_managed_object::safe_get_type(*obj);

            if (t == nullptr || t->name == nullptr) {
                continue;
            }

            ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_Once);

            auto made_node = ImGui::TreeNode(t->name);
            context_menu(*obj);

            if (made_node) {
                handle_address(*obj);
                ImGui::TreePop();
            }
        }
    }

    if (ImGui::CollapsingHeader("Types")) {
        std::vector<uint8_t> fake_type{ 0 };

        for (const auto& name : m_sorted_types) {
            fake_type.clear();

            auto t = get_type(name);

            if (t == nullptr) {
                continue;
            }

            if (t->size >= fake_type.capacity()) {
                fake_type.reserve(t->size);
            }

            handle_type((REManagedObject*)fake_type.data(), t);
        }
    }

    if (m_do_init || ImGui::InputText("Type Name", m_type_name.data(), 256)) {
        m_displayed_types.clear();

        if (auto t = get_type(m_type_name.data())) {
            m_displayed_types.push_back(t);
        }
        else {
            // Search the list for a partial match instead
            for (auto i = std::find_if(m_sorted_types.begin(), m_sorted_types.end(), [this](const auto& a) { return a.find(m_type_name.data()) != std::string::npos; });
                i != m_sorted_types.end();
                i = std::find_if(i + 1, m_sorted_types.end(), [this](const auto& a) { return a.find(m_type_name.data()) != std::string::npos; }))
            {
                if (auto t = get_type(*i)) {
                    m_displayed_types.push_back(t);
                }
            }
        }
    }

    ImGui::InputText("REObject Address", m_object_address.data(), 16, ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsHexadecimal);

    if (m_object_address[0] != 0) {
        handle_address(std::stoull(m_object_address, nullptr, 16));
    }

    std::vector<uint8_t> fake_type{ 0 };

    for (auto t : m_displayed_types) {
        fake_type.clear();
        fake_type.reserve(t->size);
        
        handle_type((REManagedObject*)fake_type.data(), t);
    }

    m_do_init = false;
}

void ObjectExplorer::handle_address(Address address, int32_t offset, Address parent) {
    if (!is_managed_object(address)) {
        return;
    }

    auto object = address.as<REManagedObject*>();
    
    if (parent == nullptr) {
        parent = address;
    }

    bool made_node = false;
    auto is_game_object = utility::re_managed_object::is_a(object, "via.GameObject");

    if (offset != -1) {
        ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_Once);

        made_node = stretched_tree_node(parent.get(offset), "0x%X:", offset);
        auto is_hovered = ImGui::IsItemHovered();
        auto additional_text = std::string{};

        context_menu(object);

        if (is_game_object) {
            additional_text = utility::re_string::get_string(address.as<REGameObject*>()->name);
        }
        else {
            // Change name based on VMType
            switch (utility::re_managed_object::get_vm_type(object)) {
            case via::clr::VMObjType::Array:
            {
                auto arr = (REArrayBase*)object;
                std::string name{};
                name += "Array<";
                name += arr->containedType != nullptr ? arr->containedType->type->name : "";
                name += ">";

                additional_text = name;
                break;
            }

            case via::clr::VMObjType::String:
                additional_text = "String";
                break;
            case via::clr::VMObjType::Delegate:
                additional_text = "Delegate";
                break;
            case via::clr::VMObjType::ValType:
                additional_text = "ValType";
                break;
            case via::clr::VMObjType::Object:
                additional_text = object->info->classInfo->type->name;
                break;
            case via::clr::VMObjType::NULL_:
            default:
                additional_text = "NULL_OBJECT";
                break;
            }
        }

        if (is_hovered) {
            make_same_line_text(additional_text, VARIABLE_COLOR_HIGHLIGHT);
        }
        else {
            make_same_line_text(additional_text, VARIABLE_COLOR);
        }
    }

    if (made_node || offset == -1) {
        if (is_game_object) {
            handle_game_object(address.as<REGameObject*>());
        }

        if (utility::re_managed_object::is_a(object, "via.Component")) {
            handle_component(address.as<REComponent*>());
        }

        handle_type(object, utility::re_managed_object::get_type(object));

        if (ImGui::TreeNode(object, "AutoGenerated Types")) {
            auto type_info = object->info->classInfo->type;
            auto size = utility::re_managed_object::get_size(object);

            for (auto i = (uint32_t)sizeof(void*); i < size; i += sizeof(void*)) {
                auto ptr = Address(object).get(i).to<REManagedObject*>();

                handle_address(ptr, i, object);
            }

            ImGui::TreePop();
        }
    }

    if (made_node && offset != -1) {
        ImGui::TreePop();
    }
}

void ObjectExplorer::handle_game_object(REGameObject* game_object) {
    ImGui::Text("Name: %s", utility::re_string::get_string(game_object->name).c_str());
    make_tree_offset(game_object, offsetof(REGameObject, transform), "Transform");
    make_tree_offset(game_object, offsetof(REGameObject, folder), "Folder");
}

void ObjectExplorer::handle_component(REComponent* component) {
    make_tree_offset(component, offsetof(REComponent, ownerGameObject), "Owner");
    make_tree_offset(component, offsetof(REComponent, childComponent), "ChildComponent");
    make_tree_offset(component, offsetof(REComponent, prevComponent), "PrevComponent");
    make_tree_offset(component, offsetof(REComponent, nextComponent), "NextComponent");
}

void ObjectExplorer::handle_transform(RETransform* transform) {

}

void ObjectExplorer::handle_type(REManagedObject* obj, REType* t) {
    if (obj == nullptr || t == nullptr) {
        return;
    }

    auto count = 0;

    for (auto type_info = t; type_info != nullptr; type_info = type_info->super) {
        auto name = type_info->name;

        if (name == nullptr) {
            continue;
        }

        if (!widget_with_context(type_info, [&name]() { return ImGui::TreeNode(name); })) {
            break;
        }

        const auto is_real_object = utility::re_managed_object::is_managed_object(obj);

        // Topmost type
        if (type_info == t && is_real_object) {
            ImGui::Text("Size: 0x%X", utility::re_managed_object::get_size(obj));
        }
        // Super types
        else {
            ImGui::Text("Size: 0x%X", type_info->size);
        }

        ++count;

        // Display type flags
        if (type_info->classInfo != nullptr) {
            if (stretched_tree_node("TypeFlags")) {
                display_enum_value("via.clr.TypeFlag", (int64_t)type_info->classInfo->typeFlags);
                ImGui::TreePop();
            }
        }

        display_methods(obj, type_info);
        display_fields(obj, type_info);
    }

    for (auto i = 0; i < count; ++i) {
        ImGui::TreePop();
    }
}

void ObjectExplorer::display_enum_value(std::string_view name, int64_t value) {
    auto first_found = get_enum_value_name(name, (int64_t)value);

    if (!first_found.empty()) {
        ImGui::Text("%i: ", value);
        ImGui::SameLine();
        ImGui::TextColored(VARIABLE_COLOR, "%s", first_found.c_str());
    }
    // Assume it's a set of flags then
    else {
        ImGui::Text("%i", value);

        std::vector<std::string> names{};

        // Check which bits are set and have enum names
        for (auto i = 0; i < 32; ++i) {
            if (auto bit = (value & ((int64_t)1 << i)); bit != 0) {
                auto value_name = get_enum_value_name(name, bit);

                if (value_name.empty()) {
                    continue;
                }

                names.push_back(value_name);
            }
        }

        // Sort and print names
        std::sort(names.begin(), names.end());
        for (const auto& value_name : names) {
            ImGui::TextColored(VARIABLE_COLOR, "%s", value_name.c_str());
        }
    }
}

void ObjectExplorer::display_methods(REManagedObject* obj, REType* type_info) {
    volatile auto methods = type_info->fields->methods;

    if (methods == nullptr || *methods == nullptr) {
        return;
    }

    auto num_methods = type_info->fields->num;

    if (ImGui::TreeNode(methods, "Methods: %i", num_methods)) {
        for (auto i = 0; i < num_methods; ++i) {
            volatile auto top = (*methods)[i];

            if (top == nullptr || *top == nullptr) {
                continue;
            }
            
            auto& holder = **top;
            auto descriptor = holder.descriptor;

            if (descriptor == nullptr || descriptor->name == nullptr) {
                continue;
            }

            auto ret = descriptor->returnTypeName != nullptr ? std::string{ descriptor->returnTypeName } : std::string{ "undefined" };
            auto made_node = widget_with_context(descriptor, [&]() { return stretched_tree_node(descriptor, "%s", ret.c_str()); });
            auto tree_hovered = ImGui::IsItemHovered();

            // Draw the variable name with a color
            if (tree_hovered) {
                make_same_line_text(descriptor->name, VARIABLE_COLOR_HIGHLIGHT);
            }
            else {
                make_same_line_text(descriptor->name, VARIABLE_COLOR);
            }

            if (made_node) {
                ImGui::Text("Address: 0x%p", descriptor);
                ImGui::Text("Function: 0x%p", descriptor->functionPtr);

                auto t2 = get_type(ret);

                if (t2 == nullptr || t2 == type_info) {
                    ImGui::Text("Type: %s", ret.c_str());
                }
                else {
                    std::vector<uint8_t> fake_object{};
                    fake_object.reserve(t2->size);
                    fake_object.clear();

                    handle_type((REManagedObject*)fake_object.data(), t2);
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

void ObjectExplorer::display_fields(REManagedObject* obj, REType* type_info) {
    if (type_info->fields == nullptr || type_info->fields->variables == nullptr || type_info->fields->variables->data == nullptr) {
        return;
    }

    const auto is_real_object = utility::re_managed_object::is_managed_object(obj);
    auto descriptors = type_info->fields->variables->data->descriptors;

    if (ImGui::TreeNode(type_info->fields, "Fields: %i", type_info->fields->variables->num)) {
        for (auto i = descriptors; i != descriptors + type_info->fields->variables->num; ++i) {
            auto variable = *i;

            if (variable == nullptr) {
                continue;
            }

            auto made_node = widget_with_context(variable->function, [&]() { return stretched_tree_node(variable, "%s", variable->typeName); });
            auto tree_hovered = ImGui::IsItemHovered();

            // Draw the variable name with a color
            if (tree_hovered) {
                make_same_line_text(variable->name, VARIABLE_COLOR_HIGHLIGHT);
            }
            else {
                make_same_line_text(variable->name, VARIABLE_COLOR);
            }

            // Display the field offset
            if (is_real_object) {
                auto offset = get_field_offset(obj, variable);

                if (offset != 0) {
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, "0x%X", offset);
                }
            }

            // Info about the field
            if (made_node) {
                if (is_real_object) {
                    attempt_display_field(obj, variable);
                }

                if (ImGui::TreeNode(variable, "Additional Information")) {
                    ImGui::Text("Address: 0x%p", variable);
                    ImGui::Text("Function: 0x%p", variable->function);

                    // Display type information
                    if (variable->typeName != nullptr) {
                        auto t2 = get_type(variable->typeName);

                        if (t2 == nullptr || t2 == type_info) {
                            ImGui::Text("Type: %s", variable->typeName);
                        }
                        else {
                            std::vector<uint8_t> fake_object{};
                            fake_object.reserve(t2->size);
                            fake_object.clear();

                            handle_type((REManagedObject*)fake_object.data(), t2);
                        }
                    }

                    auto type_kind = variable->flags & 0x1F;

                    ImGui::Text("TypeKind: %i (%s)", type_kind, get_enum_value_name("via.reflection.TypeKind", (int64_t)type_kind).c_str());
                    ImGui::Text("VarType: %i", variable->variableType);

                    if (variable->staticVariableData != nullptr) {
                        ImGui::Text("GlobalIndex: %i", variable->staticVariableData->variableIndex);
                    }

                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

void ObjectExplorer::attempt_display_field(REManagedObject* obj, VariableDescriptor* desc) {
    if (desc->function == nullptr) {
        return;
    }

    auto make_tree_addr = [this](void* addr) {
        if (widget_with_context(addr, [&]() { return ImGui::TreeNode(addr, "Variable: 0x%p", addr); })) {
            if (is_managed_object(addr)) {
                handle_address(addr);
            }

            ImGui::TreePop();
        }
    };

    auto type_name = std::string{ desc->typeName };
    auto ret = utility::hash(type_name);
    auto get_value_func = (void* (*)(VariableDescriptor*, REManagedObject*, void*))desc->function;

    char data[0x100]{ 0 };
    auto type_kind = (via::reflection::TypeKind)(desc->flags & 0x1F);

    // 0x10 == pointer, i think?
    if (type_kind != via::reflection::TypeKind::Class || desc->staticVariableData == nullptr) {
        get_value_func(desc, obj, &data);

        auto field_offset = get_field_offset(obj, desc);

        // yay for compile time string hashing
        switch (ret) {
            // signed 32
        case "u64"_fnv:
            ImGui::Text("%llu", *(int64_t*)&data);

            break;
        case "u32"_fnv:
            ImGui::Text("%i", *(int32_t*)&data);

            if (field_offset != 0) {
                auto& int_val = *Address{ obj }.get(field_offset).as<int32_t*>();

                ImGui::SliderInt("Set Value", &int_val, int_val - 1, int_val + 1);
            }

            break;
        case "s32"_fnv:
            ImGui::Text("%i", *(int32_t*)&data);

            if (field_offset != 0) {
                auto& int_val = *Address{ obj }.get(field_offset).as<int32_t*>();

                ImGui::SliderInt("Set Value", &int_val, int_val - 1, int_val + 1);
            }

            break;
        case "System.Nullable`1<System.Single>"_fnv:
        case "f32"_fnv: 
        {
            ImGui::Text("%f", *(float*)&data);

            if (field_offset != 0) {
                auto& float_val = *Address{ obj }.get(field_offset).as<float*>();

                ImGui::SliderFloat("Set Value", &float_val, float_val - 1.0f, float_val + 1.0f);
            }
            
            break;
        }
        case "System.Nullable`1<System.Boolean>"_fnv:
        case "bool"_fnv:
            if (*(bool*)&data) {
                ImGui::Text("true");
            }
            else {
                ImGui::Text("false");
            }

            if (field_offset != 0) {
                auto& bool_val = *Address{ obj }.get(field_offset).as<bool*>();

                ImGui::Checkbox("Set Value", &bool_val);
            }

            break;
        case "c16"_fnv:
            if (*(wchar_t**)&data == nullptr) {
                break;
            }

            ImGui::Text("%s", utility::narrow(*(wchar_t**)&data).c_str());
            break;
        case "c8"_fnv:
            if (*(char**)&data == nullptr) {
                break;
            }

            ImGui::Text("%s", *(char**)&data);
            break;
        case "System.Nullable`1<via.vec2>"_fnv:
        case "via.vec2"_fnv:
        {
            auto& vec = *(Vector2f*)&data;
            ImGui::Text("%f %f", vec.x, vec.y);

            if (field_offset != 0) {
                auto& vec_val = *Address{ obj }.get(field_offset).as<Vector2f*>();
                auto largest_val = vec_val.x;

                if (vec_val.y > largest_val) {
                    largest_val = vec_val.y;
                }

                ImGui::SliderFloat2("Set Value", (float*)&vec_val, largest_val - 1.0f, largest_val + 1.0f);
            }

            break;
        }
        case "System.Nullable`1<via.vec3>"_fnv:
        case "via.Float3"_fnv:
        case "via.vec3"_fnv:
        {
            auto& vec = *(Vector3f*)&data;
            ImGui::Text("%f %f %f", vec.x, vec.y, vec.z);

            if (field_offset != 0) {
                auto& vec_val = *Address{ obj }.get(field_offset).as<Vector3f*>();
                auto largest_val = vec_val.x;

                if (vec_val.y > largest_val) {
                    largest_val = vec_val.y;
                }

                if (vec_val.z > largest_val) {
                    largest_val = vec_val.z;
                }
                
                ImGui::SliderFloat3("Set Value", (float*)&vec_val, largest_val - 1.0f, largest_val + 1.0f);
            }

            break;
        }
        case "via.Quaternion"_fnv:
        {
            auto& quat = *(glm::quat*)&data;
            ImGui::Text("%f %f %f %f", quat.x, quat.y, quat.z, quat.w);

            if (field_offset != 0) {
                auto& vec_val = *Address{ obj }.get(field_offset).as<Vector4f*>();
                auto largest_val = vec_val.x;

                if (vec_val.y > largest_val) {
                    largest_val = vec_val.y;
                }

                if (vec_val.z > largest_val) {
                    largest_val = vec_val.z;
                }

                if (vec_val.w > largest_val) {
                    largest_val = vec_val.w;
                }

                ImGui::SliderFloat4("Set Value", (float*)&vec_val, largest_val - 1.0f, largest_val + 1.0f);
            }

            break;
        }
        case "via.string"_fnv:
            ImGui::Text("%s", utility::re_string::get_string(*(REString*)&data).c_str());
            break;
        default: 
        {
            if (type_kind == via::reflection::TypeKind::Enum) {
                auto value = *(int32_t*)&data;
                display_enum_value(type_name, (int64_t)value);

                if (field_offset != 0) {
                    auto& int_val = *Address{ obj }.get(field_offset).as<int32_t*>();

                    ImGui::SliderInt("Set Value", &int_val, int_val - 1, int_val + 1);
                }
            }
            else {
                make_tree_addr(*(void**)&data);
            }

            break;
        }
        }
    }
    // Pointer... usually
    else {
        get_value_func(desc, obj, &data);
        make_tree_addr(*(void**)&data);
    }
}

int32_t ObjectExplorer::get_field_offset(REManagedObject* obj, VariableDescriptor* desc) {
    if (desc->typeName == nullptr || desc->function == nullptr || m_offset_map.find(desc) != m_offset_map.end()) {
        return m_offset_map[desc];
    }

    auto ret = utility::hash(std::string{ desc->typeName });

    // These usually modify the object state, not what we want.
    if (ret == "undefined"_fnv) {
        return m_offset_map[desc];
    }

    static int32_t prev_reference_count = 0;
    auto thread_context = sdk::get_thread_context();

    if (thread_context != nullptr) {
        prev_reference_count = thread_context->referenceCount;
    }

    // Set up our "translator" to throw on any exception,
    // Particularly access violations.
    // Kind of gross but it's necessary for some fields,
    // because the field function may access the thing we modified, which may actually be a pointer,
    // and we need to handle it.
    _set_se_translator([](uint32_t code, EXCEPTION_POINTERS* exc) {
        switch (code) {
        case EXCEPTION_ACCESS_VIOLATION:
        {
            spdlog::info("ObjectExplorer: Attempting to handle access violation.");

            auto thread_context = sdk::get_thread_context();

            // This counter needs to be dealt with, it will end up causing a crash later on.
            // We also need to "destruct" whatever object this is.
            if (thread_context != nullptr) {
                auto& reference_count = thread_context->referenceCount;
                auto count_delta = reference_count - prev_reference_count;

                spdlog::error("{}", reference_count);
                if (count_delta >= 1) {
                    --reference_count;

                    static void* (*func1)(void*) = nullptr;
                    static void* (*func2)(void*) = nullptr;
                    static void* (*func3)(void*) = nullptr;

                    // Get our function pointers
                    if (func1 == nullptr) {
                        spdlog::info("Locating funcs");
                        
                        // Version 1
                        //auto ref = utility::scan(g_framework->getModule().as<HMODULE>(), "48 83 78 18 00 74 ? 48 89 D9 E8 ? ? ? ? 48 89 D9 E8 ? ? ? ?");

                        // Version 2 Dec 17th, 2019 game.exe+0x20437C (works on old version too)
                        auto ref = utility::scan(g_framework->get_module().as<HMODULE>(), "48 83 78 18 00 74 ? 48 ? ? E8 ? ? ? ? 48 ? ? E8 ? ? ? ?");

                        if (!ref) {
                            spdlog::error("We're going to crash");
                            break;
                        }

                        func1 = Address{ utility::calculate_absolute(*ref + 11) }.as<decltype(func1)>();
                        func2 = Address{ utility::calculate_absolute(*ref + 19) }.as<decltype(func2)>();
                        func3 = Address{ utility::calculate_absolute(*ref + 27) }.as<decltype(func3)>();

                        spdlog::info("F1 {:x}", (uintptr_t)func1);
                        spdlog::info("F2 {:x}", (uintptr_t)func2);
                        spdlog::info("F3 {:x}", (uintptr_t)func3);
                    }

                    // Perform object cleanup that was missed because an exception occurred.
                    if (thread_context->unkPtr != nullptr && thread_context->unkPtr->unkPtr != nullptr) {
                        func1(thread_context);
                    }

                    func2(thread_context);
                    func3(thread_context);
                }
                else if (count_delta == 0) {
                    spdlog::info("No fix necessary");
                }
            }
            else {
                spdlog::info("thread context was null. A crash may occur.");
            }
        }
        default:
            break;
        }

        throw std::exception(std::to_string(code).c_str());
    });

    struct BitTester {
        BitTester(uint8_t* old_value)
            : ptr{ old_value }
        {
            old = *old_value;
        }

        ~BitTester() {
            *ptr = old;
        }

        bool is_value_same(const uint8_t* buf) const {
            return buf[0] == ptr[0];
        }

        uint8_t* ptr;
        uint8_t old;
    };

    const auto get_value_func = (void* (*)(VariableDescriptor*, REManagedObject*, void*))desc->function;
    const auto class_size = utility::re_managed_object::get_size(obj);
    const auto size = 1;

    // Copy the object so we don't cause a crash by replacing
    // data that's being used by the game
    std::vector<uint8_t> object_copy;
    object_copy.reserve(class_size);
    memcpy(object_copy.data(), obj, class_size);

    // Compare data
    for (int32_t i = sizeof(REManagedObject); i + size <= (int32_t)class_size; i += 1) {
        auto ptr = object_copy.data() + i;
        bool same = true;

        BitTester tester{ ptr };

        // Compare data twice, first run no modifications,
        // second run, slightly modify the data to double check if it's what we want.
        for (int32_t k = 0; k < 2; ++k) {
            std::array<uint8_t, 0x100> data{ 0 };

            // Attempt to get the field value.
            try {
                get_value_func(desc, (REManagedObject*)object_copy.data(), data.data());
            }
            // Access violation occurred. Good thing we handle it.
            catch (const std::exception&) {
                same = false;
                break;
            }

            // Check if result is the same at our offset.
            if (!tester.is_value_same(data.data())) {
                same = false;
                break;
            }

            // Modify the data for our second run.
            *ptr ^= 1;
        }

        if (same) {
            m_offset_map[desc] = i;
            break;
        }
    }

    return m_offset_map[desc];
}

bool ObjectExplorer::widget_with_context(void* address, std::function<bool()> widget) {
    auto ret = widget();
    context_menu(address);

    return ret;
}

void ObjectExplorer::context_menu(void* address) {
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::Selectable("Copy")) {
            std::stringstream ss;
            ss << std::hex << (uintptr_t)address;

            ImGui::SetClipboardText(ss.str().c_str());
        }

        // Log component hierarchy to disk
        if (is_managed_object(address) && utility::re_managed_object::is_a((REManagedObject*)address, "via.Component") && ImGui::Selectable("Log Hierarchy")) {
            auto comp = (REComponent*)address;

            for (auto obj = comp; obj; obj = obj->childComponent) {
                auto t = utility::re_managed_object::safe_get_type(obj);

                if (t != nullptr) {
                    if (obj->ownerGameObject == nullptr) {
                        spdlog::info("{:s} ({:x})", t->name, (uintptr_t)obj);
                    }
                    else {
                        auto owner = obj->ownerGameObject;
                        spdlog::info("[{:s}] {:s} ({:x})", utility::re_string::get_string(owner->name), t->name, (uintptr_t)obj);
                    }
                }

                if (obj->childComponent == comp) {
                    break;
                }
            }
        }

        ImGui::EndPopup();
    }
}

void ObjectExplorer::make_same_line_text(std::string_view text, const ImVec4& color) {
    if (text.empty()) {
        return;
    }

    ImGui::SameLine();
    ImGui::TextColored(color, "%s", text.data());
}

void ObjectExplorer::make_tree_offset(REManagedObject* object, uint32_t offset, std::string_view name) {
    auto ptr = Address(object).get(offset).to<void*>();

    if (ptr == nullptr) {
        return;
    }

    auto made_node = ImGui::TreeNode((uint8_t*)object + offset, "0x%X: %s", offset, name.data());

    context_menu(ptr);

    if (made_node) {
        handle_address(ptr);
        ImGui::TreePop();
    }
}

bool ObjectExplorer::is_managed_object(Address address) const {
    return utility::re_managed_object::is_managed_object(address);
}

void ObjectExplorer::populate_classes() {
    auto& type_list = *g_framework->get_types()->get_raw_types();
    spdlog::info("TypeList: {:x}", (uintptr_t)&type_list);

    // I don't know why but it can extend past the size.
    for (auto i = 0; i < type_list.numAllocated; ++i) {
        auto t = (*type_list.data)[i];

        if (t == nullptr || IsBadReadPtr(t, sizeof(REType))) {
            continue;
        }

        if (t->name == nullptr) {
            continue;
        }

        auto name = std::string{ t->name };

        if (name.empty()) {
            continue;
        }

        spdlog::info("{:s}", name);
        m_sorted_types.push_back(name);
        m_types[name] = t;
    }

    std::sort(m_sorted_types.begin(), m_sorted_types.end());
}

void ObjectExplorer::populate_enums() {
    std::ofstream out_file("Enums_Internal.hpp");


    auto ref = utility::scan(g_framework->get_module().as<HMODULE>(), "66 C7 40 18 01 01 48 89 05 ? ? ? ?");
    auto& l = *(std::map<uint64_t, REEnumData>*)(utility::calculate_absolute(*ref + 9));
    spdlog::info("EnumList: {:x}", (uintptr_t)&l);

    spdlog::info("Size: {}", l.size());

    for (auto& elem : l) {
        spdlog::info(" {:x}[ {} {} ]", (uintptr_t)&elem, elem.first, elem.second.name);

        std::string name = elem.second.name;
        std::string nspace = name.substr(0, name.find_last_of("."));
        name = name.substr(name.find_last_of(".") + 1);

        for (auto pos = nspace.find("."); pos != std::string::npos; pos = nspace.find(".")) {
            nspace.replace(pos, 1, "::");
        }


        out_file << "namespace " << nspace << " {" << std::endl;
        out_file << "    enum " << name << " {" << std::endl;

        for (auto node = elem.second.values; node != nullptr; node = node->next) {
            if (node->name == nullptr) {
                continue;
            }

            spdlog::info("     {} = {}", node->name, node->value);
            out_file << "        " << node->name << " = " << node->value << "," << std::endl;

            m_enums.emplace(elem.second.name, EnumDescriptor{ node->name, node->value });
        }

        out_file << "    };" << std::endl;
        out_file << "}" << std::endl;
    }
}

std::string ObjectExplorer::get_enum_value_name(std::string_view enum_name, int64_t value) {
    auto values = m_enums.equal_range(enum_name.data());

    for (auto i = values.first; i != values.second; ++i) {
        if (i->second.value == value) {
            return i->second.name;
        }
    }

    return "";
}

REType* ObjectExplorer::get_type(std::string_view type_name) {
    if (type_name.empty()) {
        return nullptr;
    }

    if (auto i = m_types.find(type_name.data()); i != m_types.end()) {
        return i->second;
    }

    return nullptr;
}
