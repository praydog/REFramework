#include <sstream>
#include <fstream>
#include <forward_list>
#include <deque>
#include <algorithm>
#include <regex>
#include <json.hpp>

#include <windows.h>

#include <utility/String.hpp>
#include <utility/Scan.hpp>
#include <utility/Module.hpp>
#include <utility/Memory.hpp>
#include <utility/ImGui.hpp>
#include "sdk/Renderer.hpp"
#include "sdk/MotionFsm2Layer.hpp"

#include "../mods/ScriptRunner.hpp"

#include "HookManager.hpp"

#include "Genny.hpp"
#include "GennyIda.hpp"

#include <sdk/REGameObject.hpp>
#include "REFramework.hpp"
#include "ObjectExplorer.hpp"

using json = nlohmann::json;

#ifdef TDB_DUMP_ALLOWED
std::unordered_map<std::string, std::shared_ptr<detail::ParsedType>> g_stypedb{};
std::unordered_map<uint32_t, std::shared_ptr<detail::ParsedType>> g_itypedb{};
std::unordered_map<uint32_t, std::shared_ptr<detail::ParsedType>> g_fqntypedb{};

std::unordered_map<uint32_t, std::shared_ptr<detail::ParsedParams>> g_iparamdb{};
std::unordered_map<uint32_t, std::shared_ptr<detail::ParsedMethod>> g_imethoddb{};
#endif

constexpr std::string_view TYPE_INFO_NAME = "REType";
constexpr std::string_view TYPE_DEFINITION_NAME = "sdk::RETypeDefinition";

std::unordered_set<std::string> g_class_set{};

#if TDB_VER < 69
// via.typeinfo.TypeCode doesn't exist in older games...
std::array<const char*, 84> g_typecode_names{
    "Undefined",
    "Object",
    "Action",
    "Struct",
    "NativeObject",
    "Resource",
    "UserData",
    "Bool",
    "C8",
    "C16",
    "S8",
    "U8",
    "S16",
    "U16",
    "S32",
    "U32",
    "S64",
    "U64",
    "F32",
    "F64",
    "String",
    "MBString",
    "Enum",
    "Uint2",
    "Uint3",
    "Uint4",
    "Int2",
    "Int3",
    "Int4",
    "Float2",
    "Float3",
    "Float4",
    "Float3x3",
    "Float3x4",
    "Float4x3",
    "Float4x4",
    "Half2",
    "Half4",
    "Mat3",
    "Mat4",
    "Vec2",
    "Vec3",
    "Vec4",
    "VecU4",
    "Quaternion",
    "Guid",
    "Color",
    "DateTime",
    "AABB",
    "Capsule",
    "TaperedCapsule",
    "Cone",
    "Line",
    "LineSegment",
    "OBB",
    "Plane",
    "PlaneXZ",
    "Point",
    "Range",
    "RangeI",
    "Ray",
    "RayY",
    "Segment",
    "Size",
    "Sphere",
    "Triangle",
    "Cylinder",
    "Ellipsoid",
    "Area",
    "Torus",
    "Rect",
    "Rect3D",
    "Frustum",
    "KeyFrame",
    "Uri",
    "GameObjectRef",
    "RuntimeType",
    "Sfix",
    "Sfix2",
    "Sfix3",
    "Sfix4",
    "Position",
    "F16",
    "End",
};
#endif

static std::unordered_map<std::string, std::string> g_valuetype_typedefs {
    { "System.Void", "void" },
    { "System.Char", "wchar_t" },
    { "System.Byte", "uint8_t" },
    { "System.SByte", "int8_t" },
    { "System.Int16", "int16_t" },
    { "System.UInt16", "uint16_t" },
    { "System.Int32", "int32_t" },
    { "System.UInt32", "uint32_t" },
    { "System.Int64", "int64_t" },
    { "System.UInt64", "uint64_t" },
    { "System.Single", "float" },
    { "System.Double", "double" },
    { "System.Boolean", "bool" },
};

static std::unordered_set<std::string> g_builtin_types {
    "void",
    "char",
    "wchar_t",
    "uint8_t",
    "int8_t",
    "int16_t",
    "uint16_t",
    "int32_t",
    "uint32_t",
    "int64_t",
    "uint64_t",
    "float",
    "double",
    "bool"
};

struct BitReader {
    BitReader(const void* d)
        : data{(uint8_t*)d} {}

    static constexpr uint64_t make_bitmask(int32_t nbits) {
        if (nbits < 0) {
            return 0;
        }

        if (nbits >= 64) {
            nbits = 63;
        }

        return ((uint64_t)1 << nbits) - 1;
    }

    template<typename T = uint64_t>
    T read(int32_t nbits) {
        constexpr auto CHUNK_BYTES = sizeof(uint8_t);
        constexpr auto CHUNK_BITS = CHUNK_BYTES * 8;

        const auto bidx = (uint8_t)(bit_index % CHUNK_BITS);
        const auto byte_index = (int32_t)floor((float)bit_index / (float)CHUNK_BITS) * CHUNK_BYTES;

        auto window = *(T*)&data[byte_index];
        auto out = (window >> bidx) & (T)make_bitmask(nbits);

        bit_index += nbits;

        return out;
    }

    uint8_t read_byte() { return read<uint8_t>(sizeof(uint8_t) * 8); }
    uint16_t read_short() { return read<uint16_t>(sizeof(uint16_t) * 8); }
    uint32_t read_int() { return read<uint32_t>(sizeof(uint32_t) * 8); }
    uint64_t read_int64() { return read<uint64_t>(sizeof(uint64_t) * 8); }

    void seek(int32_t index) { bit_index = index; }

    const uint8_t* data{};
    int32_t bit_index{0};
};

std::vector<std::string> split(const std::string& s, const std::string& token) {
    std::vector<std::string> out{};

    size_t prev = 0;
    for (auto i = s.find(token); i != std::string::npos; i = s.find(token, i + 1)) {
        out.emplace_back(s.substr(prev, i - prev));
        prev = i + 1;
    }

    out.emplace_back(s.substr(prev, std::string::npos));

    return out;
}

// remove stuf like "<", ">" from typenames, replace with "_"
// anything that would be invalid in a C++ identifier or directory name
std::string clean_typename(std::string_view name) {
    std::string out{};

    for (auto&& c : name) {
        if (c == '<' || c == '>' || c == ' ' || c == ',') {
            out += '_';
        } else {
            out += c;
        }
    }

    return out;
}

genny::Class* class_from_name(genny::Namespace* g, std::string class_name) {
    class_name = clean_typename(class_name);

    if (class_name.length() >= 100) {
        // hash the rest of the string using utility::hash and std::to_string instead
        auto class_name_piece = class_name.substr(0, 100);
        auto class_name_hash = utility::hash(class_name.substr(100));
        class_name = class_name_piece + std::to_string(class_name_hash);
    }

    auto namespaces = split(class_name, ".");
    auto new_ns = g;

    if (namespaces.size() > 1) {
        std::string potential_class_name{""};

        bool is_actually_class = false;

        for (auto ns = namespaces.begin(); ns != namespaces.end() - 1; ++ns) {
            if (ns != namespaces.begin()) {
                potential_class_name += ".";
            }

            potential_class_name += *ns;

            if (g_class_set.count(potential_class_name) > 0) {
                class_from_name(g, potential_class_name);
                is_actually_class = true;
            } else {
                new_ns = new_ns->namespace_(*ns);
            }
        }

        if (is_actually_class) {
            auto final_class = class_from_name(g, potential_class_name);

            return final_class->class_(namespaces.back());
        }
    }

    return new_ns->class_(namespaces.back());
}

genny::Enum* enum_from_name(genny::Namespace* g, const std::string& enum_name) {
    auto namespaces = split(enum_name, ".");
    auto new_ns = g;

    if (namespaces.size() > 1) {
        std::string potential_class_name{""};

        bool is_actually_class = false;

        for (auto ns = namespaces.begin(); ns != namespaces.end() - 1; ++ns) {
            if (ns != namespaces.begin()) {
                potential_class_name += ".";
            }

            potential_class_name += *ns;

            if (g_class_set.count(potential_class_name) > 0) {
                class_from_name(g, potential_class_name);
                is_actually_class = true;
            } else {
                new_ns = new_ns->namespace_(*ns);
            }
        }

        if (is_actually_class) {
            auto final_class = class_from_name(g, potential_class_name);

            return final_class->enum_(namespaces.back());
        }
    }

    return new_ns->enum_(namespaces.back());
}

std::shared_ptr<ObjectExplorer>& ObjectExplorer::get() {
    static auto instance = std::make_shared<ObjectExplorer>();

    return instance;
}

ObjectExplorer::ObjectExplorer()
{
    m_type_name.reserve(256);
    m_method_address.reserve(256);
    m_type_member.reserve(256);
    m_type_field.reserve(256);
    m_object_address.reserve(256);
    m_add_component_name.reserve(256);
}

void ObjectExplorer::on_draw_dev_ui() {
    ImGui::SetNextItemOpen(false, ImGuiCond_::ImGuiCond_Once);

    if (m_do_init) {
        init();
        m_do_init = false;
    }

    if (!m_do_init && !ImGui::CollapsingHeader(get_name().data())) {
        return;
    }
    if (ImGui::Button("Dump SDK")) {
        std::thread t(&ObjectExplorer::generate_sdk, this);
        t.detach();
    }

    if (m_dumping_sdk) {
        const char* overlay = nullptr;
        float progress = m_sdk_dump_progress;

        switch (m_sdk_dump_stage) {
        case SdkDumpStage::NONE:
            progress = 0.0f;
            break;
        case SdkDumpStage::DUMP_INITIALIZATION:
            overlay = "Initializing Dump...";
            progress = static_cast<float>(ImGui::GetTime()) * -0.35f;
            break;
        case SdkDumpStage::DUMP_TYPES: 
            overlay = "Dumping Types...";
            break;
        case SdkDumpStage::DUMP_RSZ:
            overlay = "Dumping RSZ...";
            break;
        case SdkDumpStage::DUMP_METHODS:
            overlay = "Dumping Methods...";
            break;
        case SdkDumpStage::DUMP_FIELDS:
            overlay = "Dumping Fields...";
            break;
        case SdkDumpStage::DUMP_PROPERTIES:
            overlay = "Dumping Properties...";
            break;
        case SdkDumpStage::DUMP_RSZ_2:
            overlay = "Adjusting RSZ...";
            break;
        case SdkDumpStage::DUMP_DESERIALIZER_CHAIN:
            overlay = "Dumping Deserializer Chains...";
            break;
        case SdkDumpStage::DUMP_NON_TDB_TYPES:
            overlay = "Dumping Non-TDB Types...";
            break;
        case SdkDumpStage::GENERATE_SDK:
            overlay = "Generating IDA SDK...";
            progress = static_cast<float>(ImGui::GetTime()) * -0.35f;
            break;
        default: 
            progress = 0.0f;
            break;
        }

        imgui::progress_bar(progress, {}, overlay);
    }

    auto curtime = std::chrono::system_clock::now();

    // List of globals to choose from
    if (ImGui::CollapsingHeader("Singletons")) {
        if (curtime > m_next_refresh) {
            reframework::get_globals()->safe_refresh();
            m_next_refresh = curtime + std::chrono::seconds(1);
        }

        // make a copy, we want to sort by name
        auto singletons_unordered = reframework::get_globals()->get_objects();
        std::vector<REManagedObject*> singletons{ singletons_unordered.begin(), singletons_unordered.end() };

        // first loop, sort
        std::sort(singletons.begin(), singletons.end(), [](REManagedObject* a, REManagedObject* b) {
            auto a_type = utility::re_managed_object::safe_get_type(a);
            auto b_type = utility::re_managed_object::safe_get_type(b);

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
            auto t = utility::re_managed_object::safe_get_type(obj);

            if (t == nullptr || t->name == nullptr) {
                continue;
            }

            ImGui::SetNextItemOpen(false, ImGuiCond_::ImGuiCond_Once);

            auto made_node = ImGui::TreeNode(t->name);
            context_menu(obj);

            if (made_node) {
                handle_address(obj);
                ImGui::TreePop();
            }
        }
    }

    if (ImGui::CollapsingHeader("Native Singletons")) {
        auto& native_singletons = reframework::get_globals()->get_native_singleton_types();

        // Display the nodes
        for (auto t : native_singletons) {
            if (curtime > m_next_refresh_natives) {
                reframework::get_globals()->safe_refresh_native();
                m_next_refresh_natives = curtime + std::chrono::seconds(1);
            }

            auto obj = utility::re_type::get_singleton_instance(t);

            if (obj != nullptr) {
                handle_type((REManagedObject*)obj, t);
            }
        }
    }

    if (ImGui::CollapsingHeader("Renderer")) {
        auto root_layer = sdk::renderer::get_root_layer();
        ImGui::Text("Root layer: 0x%p", root_layer);

        if (root_layer != nullptr) {
            auto [scene_parent, scene_layer] = root_layer->find_layer_recursive("via.render.layer.Scene");

            if (scene_layer != nullptr) {
                ImGui::Text("Scene layer: 0x%p", *scene_layer);
            }

            if (ImGui::TreeNode("Root Layer")) {
                handle_address((void*)root_layer);
                ImGui::TreePop();
            }

            if (scene_layer != nullptr && ImGui::TreeNode("Scene Layer")) {
                handle_address((void*)*scene_layer);
                ImGui::TreePop();
            }
        }
    }

    if (ImGui::CollapsingHeader("Types")) {
        std::vector<uint8_t> fake_type{ 0 };

        for (const auto& name : m_sorted_types) {
            auto t = get_type(name);

            if (t == nullptr) {
                continue;
            }
            
            if (t->size > fake_type.size()) {
                fake_type.resize(t->size);
            }

            memset(fake_type.data(), 0, t->size);\
            handle_type((REManagedObject*)fake_type.data(), t);
        }
    }

    ImGui::Checkbox("Search using Regex", &m_search_using_regex);

    if (m_do_init || ImGui::InputText("Type Name", m_type_name.data(), 256)) {
        m_displayed_types.clear();

        if (auto t = get_type(m_type_name.data())) {
            m_displayed_types.push_back(t);
        }
        else {
            std::function<bool(const std::string&)> search_algo{};

            if (m_search_using_regex) {
                search_algo = [this](const auto& a) { return std::regex_search(a, std::regex{ m_type_name.data() }); };
            } else {
                search_algo = [this](const auto& a) { return a.find(m_type_name.data()) != std::string::npos; };
            }

            try {
                // Search the list for a partial match instead
                for (auto i = std::find_if(m_sorted_types.begin(), m_sorted_types.end(), search_algo);
                    i != m_sorted_types.end();
                    i = std::find_if(i + 1, m_sorted_types.end(), search_algo))
                {
                    if (auto t = get_type(*i)) {
                        m_displayed_types.push_back(t);
                    }
                }
            } catch (...) {

            }
        }
    }

    if (m_do_init || ImGui::InputText("Method Signature", m_type_member.data(), 256)) {
        m_displayed_types.clear();
        m_type_field[0] = '\0';

        if (!std::string_view{m_type_member.data()}.empty()) {
            for (auto i = std::find_if(m_sorted_types.begin(), m_sorted_types.end(), [this](const auto& a) { return is_filtered_type(a); });
                i != m_sorted_types.end();
                i = std::find_if(i + 1, m_sorted_types.end(), [this](const auto& a) { return is_filtered_type(a); })) {
                if (auto t = get_type(*i)) {
                    m_displayed_types.push_back(t);
                }
            }
        }
    }

    if (m_do_init || ImGui::InputText("Field Signature", m_type_field.data(), 256)) {
        m_displayed_types.clear();
        m_type_member[0] = '\0';

        if (!std::string_view{m_type_field.data()}.empty()) {
            for (auto i = std::find_if(m_sorted_types.begin(), m_sorted_types.end(), [this](const auto& a) { return is_filtered_type(a); });
                i != m_sorted_types.end();
                i = std::find_if(i + 1, m_sorted_types.end(), [this](const auto& a) { return is_filtered_type(a); })) {
                if (auto t = get_type(*i)) {
                    m_displayed_types.push_back(t);
                }
            }
        }
    }

    if (ImGui::InputText("Method Address", m_method_address.data(), 17, ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsHexadecimal)) {
        m_displayed_method = nullptr;

        try {
            if (m_method_address[0] != 0) {
                const auto method_address = std::stoull(m_method_address, nullptr, 16);

                if (auto it = m_method_map.find(method_address); it != m_method_map.end()) {
                    m_displayed_method = it->second;
                }
            }
        } catch (...) {
            ImGui::Text("Invalid address");
        }
    }

    ImGui::InputText("REObject Address", m_object_address.data(), 17, ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Create new game object"))  {
        auto& pinned = m_pinned_objects.emplace_back();
        const auto gameobject_t = sdk::find_type_definition("via.GameObject");
        const auto create_method = gameobject_t->get_method("create(System.String)");

        auto new_obj = create_method->call<::REGameObject*>(sdk::get_thread_context(), sdk::VM::create_managed_string(L"ObjectExplorerObject"));

        static uint32_t id = 0;

        pinned.address = new_obj;
        pinned.name = gameobject_t->get_name();
        pinned.path = std::to_string(id++);
    }

    if (m_object_address[0] != 0) {
        handle_address(std::stoull(m_object_address, nullptr, 16));
    }

    if (m_displayed_method != nullptr) {
        attempt_display_method(nullptr, *m_displayed_method, true);
    }

    std::vector<uint8_t> fake_type{ 0 };

    for (auto t : m_displayed_types) {
        if (t->size > fake_type.size()) {
            fake_type.resize(t->size);
        }
        
        memset(fake_type.data(), 0, t->size);
        handle_type((REManagedObject*)fake_type.data(), t);
    }

    m_do_init = false;
}

void ObjectExplorer::on_frame() {
    if (!m_pinned_objects.empty()) {
        bool open = true;

        // on_frame is just going to be a way to display
        // the pinned objects in a separate window

        ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_::ImGuiCond_Once);
        if (ImGui::Begin("Pinned objects", &open)) {
            display_pins();

            ImGui::End();
        }

        if (!open) {
            m_pinned_objects.clear();
        }
    }

    if (!m_hooked_methods.empty()) {
        bool open = true;

        // on_frame is just going to be a way to display
        // the pinned objects in a separate window

        ImGui::SetNextWindowSize(ImVec2(400, 800), ImGuiCond_::ImGuiCond_Once);
        if (ImGui::Begin("Hooked methods", &open)) {
            display_hooks();

            ImGui::End();
        }

        if (!open) {
            for (auto& h : m_hooked_methods) {
                g_hookman.remove(h.method, h.hook_id);
            }

            m_hooked_methods.clear();
        }
    }

    if (!m_frame_jobs.empty()) {
        std::scoped_lock _{m_job_mutex};

        for (auto& job : m_frame_jobs) {
            job();
        }

        m_frame_jobs.clear();
    }
}

void ObjectExplorer::display_pins() {
    for (auto& pinned_obj : m_pinned_objects) {
        const auto made_node = ImGui::TreeNode((pinned_obj.path + "." + pinned_obj.name).c_str());
        context_menu(pinned_obj.address);

        if (made_node) {
            handle_address(pinned_obj.address);
            ImGui::TreePop();
        }
    }
}

void ObjectExplorer::display_hooks() {
    std::scoped_lock _{m_hooks_context.mtx};

    ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
    if (ImGui::TreeNode("Options")) {
        ImGui::Checkbox("Hide uncalled methods", &m_hooks_context.hide_uncalled_methods);

        ImGui::SameLine();

        if (ImGui::Button("Reset Stats")) {
            for (auto& h : m_hooked_methods) {
                h.reset_stats();
            }
        }

        // Combobox of the sort method instead
        if (ImGui::BeginCombo("Sort by", HooksContext::s_sort_method_names[(uint8_t)m_hooks_context.sort_method])) {
            for (int i = 0; i < HooksContext::s_sort_method_names.size(); i++) {
                const bool is_selected = (m_hooks_context.sort_method == (HooksContext::SortMethod)i);

                if (ImGui::Selectable(HooksContext::s_sort_method_names[i], is_selected)) {
                    m_hooks_context.sort_method = (HooksContext::SortMethod)i;
                }

                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        ImGui::TreePop();
    }

    std::vector<HookedMethod*> hooks_to_iterate{};
    for (auto& h : m_hooked_methods) {
        if (m_hooks_context.hide_uncalled_methods && h.stats.call_count == 0) {
            continue;
        }

        hooks_to_iterate.push_back(&h);
    }

    switch (m_hooks_context.sort_method) {
    case HooksContext::SortMethod::CALL_COUNT:
        std::sort(hooks_to_iterate.begin(), hooks_to_iterate.end(), [](const auto& a, const auto& b) {
            return a->stats.call_count > b->stats.call_count;
        });
        break;
    case HooksContext::SortMethod::CALL_TIME_LAST:
        std::sort(hooks_to_iterate.begin(), hooks_to_iterate.end(), [](const auto& a, const auto& b) {
            return a->stats.last_call_time > b->stats.last_call_time;
        });
        break;
    case HooksContext::SortMethod::CALL_TIME_DELTA:
        std::sort(hooks_to_iterate.begin(), hooks_to_iterate.end(), [](const auto& a, const auto& b) {
            return a->stats.last_call_delta > b->stats.last_call_delta;
        });
        break;
    case HooksContext::SortMethod::CALL_TIME_TOTAL:
        std::sort(hooks_to_iterate.begin(), hooks_to_iterate.end(), [](const auto& a, const auto& b) {
            return a->stats.total_call_time > b->stats.total_call_time;
        });
        break;
    case HooksContext::SortMethod::METHOD_NAME:
        std::sort(hooks_to_iterate.begin(), hooks_to_iterate.end(), [](const auto& a, const auto& b) {
            return a->name < b->name;
        });
        break;
    case HooksContext::SortMethod::NUMBER_OF_CALLERS:
        std::sort(hooks_to_iterate.begin(), hooks_to_iterate.end(), [](const auto& a, const auto& b) {
            return a->callers.size() > b->callers.size();
        });
        break;
    case HooksContext::SortMethod::NUMBER_OF_THREADS_CALLED_FROM:
        std::sort(hooks_to_iterate.begin(), hooks_to_iterate.end(), [](const auto& a, const auto& b) {
            return a->stats.thread_ids.size() > b->stats.thread_ids.size();
        });
        break;
    default:
        break;
    };

    for (auto& hp : hooks_to_iterate) {
        auto& h = *hp;
        ImGui::PushID(h.method);

        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        const auto made_node = ImGui::TreeNode(h.name.c_str());
        method_context_menu(h.method, h.name);

        if (made_node) {
            ImGui::Checkbox("Skip function call", &h.skip);
            ImGui::TextWrapped("Call count: %i", h.stats.call_count);

            ImGui::SameLine();
            const float delta_ms = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(h.stats.last_call_delta).count();
            const float total_ms = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(h.stats.total_call_time).count();
            ImGui::TextWrapped("Time (ms): Delta %f, Total %f", delta_ms, total_ms);

            if (ImGui::TreeNode("Info")) {
                ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
                if (ImGui::TreeNode("Callers")) {
                    // sort callers combo
                    if (ImGui::BeginCombo("Sort Callers by", HookedMethod::s_sort_callers_names[(uint8_t)h.sort_callers_method])) {
                        for (int i = 0; i < HookedMethod::s_sort_callers_names.size(); i++) {
                            const bool is_selected = (h.sort_callers_method == (HookedMethod::SortCallersMethod)i);

                            if (ImGui::Selectable(HookedMethod::s_sort_callers_names[i], is_selected)) {
                                h.sort_callers_method = (HookedMethod::SortCallersMethod)i;
                            }

                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }

                        ImGui::EndCombo();
                    }

                    std::vector<sdk::REMethodDefinition*> callers_to_iterate{};

                    for (auto& caller : h.callers) {
                        callers_to_iterate.push_back(caller);
                    }

                    switch (h.sort_callers_method) {
                    case HookedMethod::SortCallersMethod::CALL_COUNT:
                        std::sort(callers_to_iterate.begin(), callers_to_iterate.end(), [&h](const auto& a, const auto& b) {
                            return h.stats.callers_context[a].call_count > h.stats.callers_context[b].call_count;
                        });
                        break;
                    case HookedMethod::SortCallersMethod::METHOD_NAME:
                        std::sort(callers_to_iterate.begin(), callers_to_iterate.end(), [&h](const auto& a, const auto& b) {
                            return a->get_name() < b->get_name();
                        });
                        break;
                    default:
                        break;
                    };

                    for (auto& caller : callers_to_iterate) {
                        const auto& context = h.stats.callers_context[caller];
                        const auto declaring_type = caller->get_declaring_type();
                        //auto method_name = declaring_type != nullptr ? declaring_type->get_full_name() + "." + caller->get_name() : caller->get_name();
                        //method_name += " [" + std::to_string(context.call_count) + "]";
                        const auto call_count = std::string("[") + std::to_string(context.call_count) + "]";

                        ImGui::TextUnformatted(call_count.c_str());
                        ImGui::SameLine();
                        this->attempt_display_method(nullptr, *caller, true);
                    }

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Return Addresses")) {
                    for (auto addr : h.return_addresses) {
                        ImGui::Text("0x%p", addr);

                        if (ImGui::IsItemClicked()) {
                            ImGui::SetClipboardText((std::stringstream{} << std::hex << addr).str().c_str());
                        }
                    }
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Thread IDs")) {
                    for (auto tid : h.stats.thread_ids) {
                        ImGui::Text("%i", tid);
                    }
                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }

        ImGui::PopID();
    }
}

void ObjectExplorer::on_lua_state_created(sol::state& lua) {
    // lua bindings for ObjectExplorer might sound weird,
    // but it's actually pretty useful for displaying objects
    // that may be found during scripting
    lua.new_usertype<ObjectExplorer>("ObjectExplorer",
        "handle_address", [](ObjectExplorer* e, sol::object addr) {
            uintptr_t real_addr = 0;

            if (addr.is<uintptr_t>()) {
                real_addr = addr.as<uintptr_t>();
            } else if (addr.is<::REManagedObject*>()) {
                real_addr = (uintptr_t)addr.as<::REManagedObject*>();
            } else {
                real_addr = (uintptr_t)addr.as<void*>();
            }

            e->handle_address(real_addr);
        }
    );

    lua["object_explorer"] = this;
}

#ifdef TDB_DUMP_ALLOWED
std::shared_ptr<detail::ParsedType> ObjectExplorer::init_type(nlohmann::json& il2cpp_dump, sdk::RETypeDB* tdb, uint32_t i) {
    if (g_itypedb.find(i) != g_itypedb.end()) {
        return g_itypedb[i];
    }

    auto desc = init_type_min(il2cpp_dump, tdb, i);

    g_itypedb[i] = desc;
    g_fqntypedb[desc->t->get_fqn_hash()] = desc;

    return desc;
}

std::string ObjectExplorer::generate_full_name(sdk::RETypeDB* tdb, uint32_t i) {
    if (i == 0 || i >= tdb->numTypes) {
        return "";
    }

    auto& raw_t = (*tdb->types)[i];
    auto full_name = raw_t.get_full_name();

    spdlog::info("{:s}", full_name);

    return full_name;
}

std::shared_ptr<detail::ParsedType> ObjectExplorer::init_type_min(json& il2cpp_dump, sdk::RETypeDB* tdb, uint32_t i) {
    auto& t = *tdb->get_type(i);
    auto br = BitReader{&t};

    auto desc = std::make_shared<detail::ParsedType>();

    desc->t = &t;
    desc->name_space = t.get_namespace();
    desc->name = t.get_name();

    return desc;
}

void ObjectExplorer::export_deserializer_chain(nlohmann::json& il2cpp_dump, sdk::RETypeDB* tdb, REType* t, std::optional<std::string> real_name) {
    /*const auto is_clr_type = (((uint8_t)t->flags >> 5) & 1) != 0;

    if (is_clr_type) {
        return;
    }*/

    std::string full_name{};

    // Export info about native deserializers for the python script
    if (!real_name) {
        auto tdef = utility::re_type::get_type_definition(t);
        full_name = t->classInfo != nullptr ? generate_full_name(tdb, tdef->get_index()) : t->name;
    }
    else {
        full_name = *real_name;
    }

    auto& type_entry = il2cpp_dump[full_name];

    // already done it
    if (type_entry.contains("deserializer_chain") || type_entry.contains("RSZ")) {
        return;
    }

    std::deque<nlohmann::json> chain_raw{};

    for (auto super = t; super != nullptr; super = (sdk::RETypeCLR*)super->super) {
        if (super->fields == nullptr || super->fields->deserializer == nullptr) {
            continue;
        }

        const auto deserializer = super->fields->deserializer;
        const auto deserializer_normalized = get_original_va(deserializer);

        json des_entry{};

        auto tdef = utility::re_type::get_type_definition(super);
        
        des_entry["address"] = (std::stringstream{} << "0x" << std::hex << deserializer_normalized).str();
        des_entry["name"] = super->classInfo != nullptr ? generate_full_name(tdb, tdef->get_index()) : super->name;

        // push in reverse order so it can be parsed easier (parent -> all the way back to this type)
        chain_raw.push_front(des_entry);
    }

    // dont create an empty entry
    if (chain_raw.empty()) {
        return;
    }

    type_entry["deserializer_chain"] = chain_raw;
}
#endif

void ObjectExplorer::generate_sdk() {
    // enums
    //auto ref = utility::scan(g_framework->get_module().as<HMODULE>(), "66 C7 40 18 01 01 48 89 05 ? ? ? ?");
    //auto& l = *(std::map<uint64_t, REEnumData>*)(utility::calculate_absolute(*ref + 9));

    m_dumping_sdk = true;
    m_sdk_dump_stage = SdkDumpStage::DUMP_INITIALIZATION;
    uint32_t k = 0;
    auto n_types = 0ull;

    genny::Sdk sdk{};
    auto g = sdk.global_ns();

    g->type("int8_t")->size(1);
    g->type("int16_t")->size(2);
    g->type("int32_t")->size(4);
    g->type("int64_t")->size(8);
    g->type("wchar_t")->size(2);
    g->type("uint8_t")->size(1);
    g->type("uint16_t")->size(2);
    g->type("uint32_t")->size(4);
    g->type("uint64_t")->size(8);
    g->type("float")->size(4);
    g->type("double")->size(8);
    g->type("bool")->size(1);
    g->type("char")->size(1);
    g->type("int")->size(4);
    g->type("void")->size(0);
    //g->type("void*")->size(8);

    json il2cpp_dump{};

#ifdef TDB_DUMP_ALLOWED
    auto tdb = (sdk::RETypeDB*)reframework::get_types()->get_type_db();

    // Types
    for (uint32_t i = 0; i < tdb->numTypes; ++i) {
        init_type(il2cpp_dump, tdb, i);
    }

    for (uint32_t i = 0; i < tdb->numTypes; ++i) {
        auto desc = init_type(il2cpp_dump, tdb, i);

        desc->full_name = generate_full_name(tdb, i);
        g_stypedb[desc->full_name] = desc;
    }

    m_sdk_dump_stage = SdkDumpStage::DUMP_TYPES;

    // Finish off initialization of types
    for (uint32_t i = 0; i < tdb->numTypes; ++i) {
        m_sdk_dump_progress = static_cast<float>(i) / tdb->numTypes;

        auto desc = init_type(il2cpp_dump, tdb, i);
        auto& t = *desc->t;

        auto tdef = desc->t;

        auto& type_entry = (il2cpp_dump[desc->full_name] = {});
        const auto crc = t.get_crc_hash();
        const auto fqn = t.get_fqn_hash();
        const auto type_info = t.get_type();

        type_entry = {
            {"address", (std::stringstream{} << std::hex << get_original_va(&t)).str()},
            {"id", i},
            {"fqn", (std::stringstream{} << std::hex << fqn).str()},
            {"crc", (std::stringstream{} << std::hex << crc).str()},
            {"size", (std::stringstream{} << std::hex << t.get_size()).str()},
        };

        if (tdef->declaring_typeid != 0) {
            desc->owner = init_type(il2cpp_dump, tdb, tdef->declaring_typeid);
        }

        if (tdef->parent_typeid != 0) {
            desc->super = init_type(il2cpp_dump, tdb, tdef->parent_typeid);
            type_entry["parent"] = desc->super->full_name;
        }

        if (auto type_flags_str = get_full_enum_value_name("via.clr.TypeFlag", t.type_flags); !type_flags_str.empty()) {
            type_entry["flags"] = type_flags_str;
        }

        if (type_info != nullptr && type_info->name != nullptr) {
            if (type_info->name != desc->full_name) {
                type_entry["native_typename"] = type_info->name;
            }
        }

        type_entry["name_hierarchy"] = t.get_name_hierarchy();
        type_entry["is_generic_type"] = t.is_generic_type();
        type_entry["is_generic_type_definition"] = t.is_generic_type_definition();

#if TDB_VER >= 71
        if (tdef->element_typeid_TBD != 0) {
            type_entry["element_type_name"] = init_type(il2cpp_dump, tdb, tdef->element_typeid_TBD)->full_name;
        }
#elif TDB_VER >= 69
        if (tdef->element_typeid != 0) {
            type_entry["element_type_name"] = init_type(il2cpp_dump, tdb, tdef->element_typeid)->full_name;
        }
#endif

        if (auto gtd = t.get_generic_type_definition(); gtd != nullptr) {
            type_entry["generic_type_definition"] = gtd->get_full_name();
        }

        const auto generics = t.get_generic_argument_types();

        if (!generics.empty()) {
            for (auto gt : generics) {
                if (gt != nullptr) {
                    type_entry["generic_arg_types"].push_back({
                        {"type", gt->get_full_name()},
                        {"typeid", gt->get_index()}
                    });
                } else {
                    type_entry["generic_arg_types"].push_back({
                        {"type", "unknown"},
                        {"typeid", 0}
                    });
                }
            }
        }
    }

    m_sdk_dump_stage = SdkDumpStage::DUMP_RSZ;

    // Initialize RSZ
    // Dont do it in init_type because it calls init_type
    for (uint32_t i = 0; i < tdb->numTypes; ++i) {
        m_sdk_dump_progress = static_cast<float>(i) / tdb->numTypes;

        auto pt = init_type(il2cpp_dump, tdb, i);
        auto& t = *pt->t;

        const auto type_info = t.get_type();

        if (type_info == nullptr) {
            continue;
        }

        if (!utility::re_type::is_clr_type(type_info)) {
            export_deserializer_chain(il2cpp_dump, tdb, type_info, pt->full_name);
            continue;
        }

        auto clr_t = (sdk::RETypeCLR*)type_info;
        auto& deserialize_list = clr_t->deserializers;

        for (const auto& sequence : deserialize_list) {
            const auto code = sequence.get_code();
            const auto size = sequence.get_size();
            const auto align = sequence.get_align();
            const auto depth = sequence.get_depth();
            const auto is_array = sequence.is_array();
            const auto is_static = sequence.is_static();
            
            auto rsz_entry = json{};
            

            rsz_entry["type"] = generate_full_name(tdb, (sequence.get_native_type())->get_index());
#if TDB_VER >= 69
            rsz_entry["code"] = get_enum_value_name("via.typeinfo.TypeCode", code);
#else
            rsz_entry["code"] = g_typecode_names[code];
#endif
            rsz_entry["code_id"] = code;
            rsz_entry["align"] = align;
            rsz_entry["size"] = (std::stringstream{} << "0x" << std::hex << (uint32_t)size).str();
            rsz_entry["depth"] = depth;
            rsz_entry["array"] = is_array;
            rsz_entry["static"] = is_static;
            rsz_entry["offset_from_fieldptr"] = (std::stringstream{} << "0x" << std::hex << sequence.offset).str();

#if TDB_VER <= 49
            rsz_entry["potential_name"] = sequence.prop->name;
#endif

            il2cpp_dump[pt->full_name]["RSZ"].emplace_back(rsz_entry);
        }

        // do this because it empty
        if (!il2cpp_dump[pt->full_name].contains("RSZ")) {
            export_deserializer_chain(il2cpp_dump, tdb, type_info, pt->full_name);
        }
    }

    m_sdk_dump_stage = SdkDumpStage::DUMP_METHODS;

    // Methods
    for (uint32_t i = 0; i < tdb->numMethods; ++i) {
        m_sdk_dump_progress = static_cast<float>(i) / tdb->numMethods;

        auto& m = *tdb->get_method(i);

#if TDB_VER >= 69
        auto type_id = (uint32_t)m.declaring_typeid;
        auto impl_id = (uint32_t)m.impl_id;
        auto param_list = (uint32_t)m.get_param_index();
#else
        const auto type_id = (uint32_t)m.declaring_typeid;
        const auto param_list = m.params;
#endif

        if (g_itypedb.find(type_id) == g_itypedb.end()) {
            continue;
        }

        auto& desc = g_itypedb[type_id];

        desc->methods.push_back(&m);

#if TDB_VER >= 69
        auto& impl = (*tdb->methodsImpl)[impl_id];
        desc->method_impls.push_back(&impl);
        const auto name_offset = impl.name_offset;
#else
        const auto name_offset = m.name_offset;
#endif

        const auto name = tdb->get_string(name_offset);

        // Create an easier to deal with structure
        auto& pm = desc->parsed_methods.emplace_back(std::make_shared<detail::ParsedMethod>());

        pm->m = &m;
        pm->name = name;
        pm->owner = desc;

#if TDB_VER >= 69
        pm->m_impl = &impl;
        const auto vtable_index = impl.vtable_index;
        const auto impl_flags = impl.impl_flags;
        const auto method_flags = impl.flags;
#else
        const auto vtable_index = m.vtable_index;
        const auto impl_flags = m.impl_flags;
        const auto method_flags = m.flags;
#endif

        g_imethoddb[i] = pm;

        //spdlog::info("{:s}.{:s}: 0x{:x}", desc->t->type->name, name, (uintptr_t)m.function);

        auto& type_entry = il2cpp_dump[desc->full_name];
        auto& method_entry = type_entry["methods"][pm->name + std::to_string(i)];

        method_entry["id"] = i;
        method_entry["function"] = (std::stringstream{} << std::hex << get_original_va(m.get_function())).str();

        if (auto impl_flags_str = get_full_enum_value_name("via.clr.MethodImplFlag", impl_flags); !impl_flags_str.empty()) {
            method_entry["impl_flags"] = impl_flags_str;
        }

        if (auto flags_str = get_full_enum_value_name("via.clr.MethodFlag", method_flags); !flags_str.empty()) {
            method_entry["flags"] = flags_str;
        }

        if (vtable_index >= 0) {
            method_entry["vtable_index"] = vtable_index;
        }

        // Parameters
#if TDB_VER >= 69
        auto param_ids = Address{ tdb->bytePool }.get(param_list).as<sdk::ParamList*>();
        const auto num_params = param_ids->numParams;
        const auto invoke_id = param_ids->invokeID;
#else
#if TDB_VER >= 66
        auto param_ids = tdb->get_data<sdk::REMethodParamDef>(param_list);
#else
        auto params_data = tdb->get_data<sdk::REMethodParamDef>(param_list);
        auto param_ids = params_data->params;
#endif
        const auto num_params = (uint8_t)m.get_num_params();
        const auto invoke_id = (uint16_t)m.invoke_id;
#endif

        // Invoke wrapper for arbitrary amount of arguments, so we can just pass it on the VM stack/context as an array
        method_entry["invoke_id"] = invoke_id;

        auto parse_param = [&](uint32_t param_index, bool is_return = false) {
#if TDB_VER >= 69
            auto& p = (*tdb->params)[param_index];

            const auto attributes_index = (uint16_t)p.attributes_id;
            const auto init_data_index = (uint16_t)p.init_data_index;
            const auto name_index = (uint32_t)p.name_offset;
            const auto modifier = (uint8_t)p.modifier;
            const auto param_type_id = (uint32_t)p.type_id;
            const auto flags = (uint16_t)p.flags;
#else
            auto& p = param_ids[param_index];

            const auto param_type_id = (uint32_t)p.param_typeid;
            const auto name_index = p.name_offset;
            const auto flags = p.flags;
#endif

            if (auto it = g_itypedb.find(param_type_id); it == g_itypedb.end()) {
                return json{};
            }

            auto& param_type = g_itypedb[param_type_id];

            auto pdesc = std::make_shared<detail::ParsedParams>();
            g_iparamdb[param_index] = pdesc;

            auto param_name = Address{tdb->stringPool}.get(name_index).as<const char*>();

            pdesc->owner = pm;
            pdesc->type = param_type;
            pdesc->name = param_name;

            if (is_return) {
                pm->return_val = pdesc;
            }
            else {
                pm->params.emplace_back(pdesc);
            }

            auto param_entry = json{
                {"type", pdesc->type->full_name},
                {"name", pdesc->name},
            };

            if (auto param_flags = get_full_enum_value_name("via.clr.ParamFlag", flags); !param_flags.empty()) {
                param_entry["flags"] = param_flags;
            }

#if TDB_VER >= 69
            if (auto param_modifier = get_full_enum_value_name("via.clr.ParamModifier", modifier); !param_modifier.empty()) {
                param_entry["modifier"] = param_modifier;
            }
#endif

            return param_entry;
        };

        const auto return_type = m.get_return_type();
        const auto return_type_name = return_type != nullptr ? return_type->get_full_name() : "";

        // Parse return type
#if TDB_VER >= 69
        method_entry["returns"] = parse_param(param_ids->returnType, true);
#else
        method_entry["returns"] = json{
            {"type", return_type_name},
            {"name", ""},
        };
#endif

        // Parse all params
        for (auto f = 0; f < num_params; ++f) {
#if TDB_VER >= 69
            const auto param_index = param_ids->params[f];
            if (param_index >= tdb->numParams) {
                break;
            }
#else
            const auto param_index = f;
#endif

            auto param_entry = parse_param(param_index);

            method_entry["params"].emplace_back(param_entry);
        }

        // Generate sdkgenny methods
        if (pm->owner != nullptr && pm->m != nullptr) {
            auto c = pm->owner->genny_t != nullptr ? pm->owner->genny_t : class_from_name(g, pm->owner->full_name);
            pm->owner->genny_t = c;

            auto real_return_type = return_type;
            auto real_return_type_name = return_type_name;

            if (return_type != nullptr && return_type->is_enum()) {
                const auto underlying_type = return_type->get_underlying_type();

                if (underlying_type != nullptr) {
                    real_return_type = underlying_type;
                    real_return_type_name = underlying_type->get_full_name();
                }
            } 
            
            // get the typedef of the C# valuetype to C++
            if (real_return_type != nullptr && real_return_type->is_value_type()) {
                if (auto it = g_valuetype_typedefs.find(real_return_type_name); it != g_valuetype_typedefs.end()) {
                    real_return_type_name = it->second;
                }
            } else if (real_return_type != nullptr && real_return_type_name == "System.Void") {
                real_return_type_name = "void";
            }

            auto retc = class_from_name(g, real_return_type_name != "" ? real_return_type_name : "void");

            bool is_ptr = false;

            if (real_return_type != nullptr && real_return_type->should_pass_by_pointer()) {
                is_ptr = true;
            }

            std::stringstream cpp_ret_stream{};
            retc->generate_typename_for(cpp_ret_stream, nullptr);

            const auto cpp_ret_name = cpp_ret_stream.str();

            genny::Function* f = nullptr;

            std::stringstream os{};

            //os << "auto t = get_type_definition();\n";
            os << "static auto m = sdk::RETypeDB::get()->get_method(" << pm->m->get_index() << ");\n";

            auto param_names = pm->m->get_param_names();
            auto param_types = pm->m->get_param_types();

            f = c->function(clean_typename(pm->name) + std::to_string(pm->m->get_index()));

            auto ret_in_first_param = false;

            if (real_return_type != nullptr && real_return_type->is_value_type() && is_ptr && retc->name() != "void") {
                ret_in_first_param = true;
                f = f->returns(retc);

                os << cpp_ret_name << " out;\n";
                os << "return m->call<" << cpp_ret_name << ">(&out, sdk::get_thread_context()";

                if (!pm->m->is_static()) {
                    os << ", this";
                }

                if (param_names.size() > 0){
                    for (auto param_name : param_names) {
                        os << ", " << param_name;
                    }
                }

                os << ");\n";
            } else if (is_ptr) {
                f = f->returns(retc->ptr());

                os << "return m->call<" << cpp_ret_name << "*>(sdk::get_thread_context()";

                if (!pm->m->is_static()) {
                    os << ", this";
                }

                if (param_names.size() > 0){
                    for (auto param_name : param_names) {
                        os << ", " << param_name;
                    }
                }

                os << ");\n";
            } else {
                f = f->returns(retc);
                os << "return m->call<" << cpp_ret_name << ">(sdk::get_thread_context()";

                if (!pm->m->is_static()) {
                    os << ", this";
                }

                if (param_names.size() > 0){
                    for (auto param_name : param_names) {
                        os << ", " << param_name;
                    }
                }

                os << ");\n";
            }

            f->procedure(os.str());

            // Method params
            for (auto i = 0; i < param_types.size(); ++i) {
                auto param_type = param_types[i];
                auto param_name = param_names[i];

                if (param_type == nullptr) {
                    continue;
                }

                auto param_type_name = param_type->get_full_name();

                if (param_type->is_enum()) {
                    const auto underlying_type = param_type->get_underlying_type();

                    if (underlying_type != nullptr) {
                        param_type_name = underlying_type->get_full_name();
                        param_type = underlying_type;
                    }
                }

                if (param_type->should_pass_by_pointer()) {
                    f->param(clean_typename(param_name))->type(class_from_name(g, param_type_name)->ptr());
                } else {
                    genny::Type* genny_param_type = nullptr;

                    if (param_type->is_value_type()) {
                        if (auto it = g_valuetype_typedefs.find(param_type_name); it != g_valuetype_typedefs.end()) {
                            genny_param_type = g->type(it->second);
                        }
                    } else if (param_type_name == "System.Void") {
                        genny_param_type = g->type("void");
                    } else {
                        genny_param_type = class_from_name(g, param_type_name);
                    }

                    f->param(clean_typename(param_name))->type(genny_param_type);
                }
            }
        }
    }

    spdlog::info("FIELDS BEGIN");
    m_sdk_dump_stage = SdkDumpStage::DUMP_FIELDS;

    auto dummy_constant = g->class_("__DummyClass__")->constant("__DummyConstant__")->type("int32_t");

    // Fields
    for (uint32_t i = 0; i < tdb->numFields; ++i) {
        m_sdk_dump_progress = static_cast<float>(i) / tdb->numFields;

        auto& f = (*tdb->fields)[i];

#if TDB_VER >= 69
        const auto type_id = (uint32_t)f.declaring_typeid;
        const auto impl_id = (uint32_t)f.impl_id;
        const auto offset = (uint32_t)f.get_offset_from_fieldptr();
#else
        const auto type_id = (uint32_t)f.declaring_typeid;
        const auto offset = f.offset;
#endif

        if (g_itypedb.find(type_id) == g_itypedb.end()) {
            continue;
        }

        auto& desc = g_itypedb[type_id];

        desc->fields.push_back(&f);

#if TDB_VER >= 69
        auto& impl = (*tdb->fieldsImpl)[impl_id];
        desc->field_impls.push_back(&impl);


        auto br_impl = BitReader{&impl};

        /*const auto field_attr_id = (uint16_t)br_impl.read_short();
        const auto field_flags = (uint16_t)br_impl.read_short();
        const auto field_type = (uint32_t)br_impl.read(18);
        const auto init_data_low = (uint16_t)br_impl.read(14);
        const auto name_offset = (uint32_t)br_impl.read(30);
        const auto init_data_high = (uint8_t)br_impl.read(2);
        const auto name = Address{tdb->stringPool}.get(name_offset).as<const char*>();
        const auto init_data_index = init_data_low | (init_data_high << 14);*/

        const auto field_attr_id = impl.attributes_id;
        const auto field_flags = impl.flags;
        const auto field_type = f.get_type() != nullptr ? f.get_type()->get_index() : 0;
        const auto name_offset = impl.name_offset;
        const auto init_data_index = f.get_init_data_index();
        const auto name = tdb->get_string(name_offset);
#else
        const auto name_offset = f.name_offset;
        const auto name = Address{ tdb->stringPool }.get(name_offset).as<const char*>();
        const auto field_type = (uint32_t)f.field_typeid;
        const auto field_flags = f.flags;
#if TDB_VER >= 66
        const auto init_data_index = f.init_data_index;
#endif
#endif

#if TDB_VER >= 66
        const auto init_data_offset = init_data_index != 0 ? (*tdb->initData)[init_data_index] : 0;
#else
        const auto init_data_offset = f.init_data_offset;
#endif

        // Create an easier to deal with structure
        auto& pf = desc->parsed_fields.emplace_back(std::make_shared<detail::ParsedField>());

        pf->f = &f;
        pf->name = name;
        pf->owner = desc;
        pf->offset_from_fieldptr = offset;
        pf->offset_from_base = pf->offset_from_fieldptr;
        pf->type = g_itypedb[field_type];

#if TDB_VER >= 69
        pf->f_impl = &impl;
#endif

        // Resolve the offset to be from the base class
        pf->offset_from_base += pf->owner->t->get_fieldptr_offset();
        genny::Constant* cs = dummy_constant;

        // Use sdkgenny to generate the fields now
        if (pf->owner != nullptr) {
            auto c = pf->owner->genny_t != nullptr ? pf->owner->genny_t : class_from_name(g, pf->owner->full_name);
            auto t = pf->type;

            pf->owner->genny_t = c;

            if (pf->type != nullptr && pf->type->t != nullptr) {
                const bool is_owner_value_type = pf->owner->t->is_value_type();
                const bool is_value_type = pf->type->t->is_value_type();
                const bool is_enum = pf->type->t->is_enum();

                auto valuetype_typedef = pf->type->full_name;

                if (is_enum) {
                    const auto underlying_type = pf->type->t->get_underlying_type();

                    if (underlying_type != nullptr) {
                        valuetype_typedef = underlying_type->get_full_name();
                    }
                }

                // Non-statics first
                if ((field_flags & (uint16_t)via::clr::FieldFlag::Static) == 0) {
                    auto v = c->variable(pf->name)->offset(pf->offset_from_base);

                    if (is_value_type) {
                        if (auto it = g_valuetype_typedefs.find(valuetype_typedef); it != g_valuetype_typedefs.end()) {
                            valuetype_typedef = it->second;
                            v->type(g->type(valuetype_typedef));

                            /*if (is_enum) {
                                v->comment(std::stringstream{} << "ENUM: " << pf->type->full_name);
                            }*/
                        } else if (pf->type->t != pf->owner->t) {
                            v->type(class_from_name(g, valuetype_typedef));
                        } else {
                            v->type(g->type("uint8_t")->array_(pf->type->t->get_size()));
                        }
                    } else {
                        if (pf->type->full_name == "System.Void") {
                            v->type(g->type("void"));
                        } else {
                            v->type(class_from_name(g, pf->type->full_name)->ptr());
                        }
                    }
                } else { // Statics
                    genny::StaticFunction* f = nullptr;
                    genny::Type* return_f = nullptr;

                    if (is_value_type) {
                        if (auto it = g_valuetype_typedefs.find(valuetype_typedef); it != g_valuetype_typedefs.end()) {
                            valuetype_typedef = it->second;
                            return_f = g->type(valuetype_typedef);

                            if ((field_flags & (uint16_t)via::clr::FieldFlag::Literal) != 0) {
                                cs = c->constant(pf->name)->type(return_f);
                            } else {
                                f = c->static_function(pf->name);
                            }
                        } else {
                            return_f = g->type("uint8_t")->ptr();

                            f = c->static_function(pf->name);
                            f->returns(return_f);
                        }
                    } else {
                        if ((field_flags & (uint16_t)via::clr::FieldFlag::Literal) != 0) {
                            switch (utility::hash(pf->type->full_name)) {
                            case "System.String"_fnv:
                                return_f = g->type("char")->ptr();
                                cs = c->constant(pf->name)->type(return_f);
                                break;
                            default:
                                return_f = class_from_name(g, pf->type->full_name)->ptr();
                                f = c->static_function(pf->name);
                                f->returns(return_f);

                                break;
                            }
                        }
                        else {
                            if (pf->type->full_name == "System.Void") {
                                return_f = g->type("void");
                            } else {
                                return_f = class_from_name(g, pf->type->full_name)->ptr();
                            }

                            f = c->static_function(pf->name);
                            f->returns(return_f);
                        }
                    }

                    if (f != nullptr) {
                        std::stringstream full_return_type_name{};
                        return_f->generate_typename_for(full_return_type_name, nullptr);
                        std::stringstream os{};
                        os << "return *sdk::get_static_field<" << full_return_type_name.str() << ">(\"" << pf->owner->full_name << "\", \"" << pf->name << "\", false);";

                        f->procedure(os.str());
                    }
                }
            }
        }
        
        const auto field_type_name = (pf->type != nullptr) ? pf->type->full_name : "";

        //spdlog::info("{:s} {:s}.{:s}: 0x{:x}", field_type_name, desc->t->type->name, name, pf->offset_from_base);

        auto& type_entry = il2cpp_dump[desc->full_name];
        auto& field_entry = type_entry["fields"][pf->name];

        field_entry = {
            {"id", i},
            {"type", field_type_name},
            {"offset_from_base", (std::stringstream{} << "0x" << std::hex << pf->offset_from_base).str()},
            {"offset_from_fieldptr", (std::stringstream{} << "0x" << std::hex << pf->offset_from_fieldptr).str()},
#if TDB_VER >= 66
            {"init_data_index", init_data_index}
#endif
        };

        if (auto field_flags_str = get_full_enum_value_name("via.clr.FieldFlag", field_flags); !field_flags_str.empty()) {
            field_entry["flags"] = field_flags_str;
        }

        if (init_data_offset != 0) {
            auto init_data = &(*tdb->bytePool)[init_data_offset];

            // WACKY
            if (init_data_offset < 0) {
                init_data = &((uint8_t*)tdb->stringPool)[init_data_offset * -1];
            }

            auto init_data_type = pf->type;
            auto full_name{ init_data_type->full_name };

            // edge case
            if (pf->type->super != nullptr && pf->type->super->full_name == "System.Enum") {
                switch (pf->type->t->get_size() - pf->type->super->t->get_size()) {
                case 1:
                    full_name = "System.Byte";
                    break;
                case 2:
                    full_name = "System.UInt16";
                    break;
                case 4:
                    full_name = "System.UInt32";
                    break;
                case 8:
                    full_name = "System.UInt64";
                    break;
                }
            }

            switch (utility::hash(full_name)) {
            case "System.Boolean"_fnv:
                field_entry["default"] = *(bool*)init_data;
                cs->integer(*(bool*)init_data);
                break;
            case "System.Char"_fnv:
                field_entry["default"] = *(wchar_t*)init_data;
                cs->integer(*(wchar_t*)init_data);
                break;
            case "System.Byte"_fnv:
                field_entry["default"] = *(uint8_t*)init_data;
                cs->integer(*(uint8_t*)init_data);
                break;
            case "System.SByte"_fnv:
                field_entry["default"] = *(int8_t*)init_data;
                cs->integer(*(int8_t*)init_data);
                break;
            case "System.UInt16"_fnv:
                field_entry["default"] = *(uint16_t*)init_data;
                cs->integer(*(uint16_t*)init_data);
                break;
            case "System.Int16"_fnv:
                field_entry["default"] = *(int16_t*)init_data;
                cs->integer(*(int16_t*)init_data);
                break;
            case "System.UInt32"_fnv:
                field_entry["default"] = *(uint32_t*)init_data;
                cs->integer(*(uint32_t*)init_data);
                break;
            case "System.Int32"_fnv:
                field_entry["default"] = *(int32_t*)init_data;
                cs->integer(*(int32_t*)init_data);
                break;
            case "System.UInt64"_fnv:
                field_entry["default"] = *(uint64_t*)init_data;
                cs->integer(*(uint64_t*)init_data);
                break;
            case "System.Int64"_fnv:
                field_entry["default"] = *(int64_t*)init_data;
                cs->integer(*(int64_t*)init_data);
                break;
            case "System.Single"_fnv:
                field_entry["default"] = *(float*)init_data;
                cs->real(*(float*)init_data);
                break;
            case "System.Double"_fnv:
                field_entry["default"] = *(double*)init_data;
                cs->real(*(double*)init_data);
                break;
            case "System.String"_fnv:
                field_entry["default"] = (char*)init_data;
                cs->string((char*)init_data);
                break;
            default:
                field_entry["default"] = "REFRAMEWORK_UNIMPLEMENTED_INIT_TYPE";
                break;
            }
        }
    }
    
    spdlog::info("PROPERTIES BEGIN");
    m_sdk_dump_stage = SdkDumpStage::DUMP_PROPERTIES;

    // properties
    for (uint32_t i = 0; i < tdb->numProperties; ++i) {
        m_sdk_dump_progress = static_cast<float>(i) / tdb->numProperties;

        auto& p = (*tdb->properties)[i];

        const auto getter_id = (uint32_t)p.getter;
        const auto setter_id = (uint32_t)p.setter;

        std::shared_ptr<detail::ParsedMethod> getter{};
        std::shared_ptr<detail::ParsedMethod> setter{};
        std::shared_ptr<detail::ParsedMethod> prop_method{};

        if (auto it = g_imethoddb.find(getter_id); it != g_imethoddb.end()) {
            prop_method = it->second;
            getter = it->second;
        }
        
        if (auto it = g_imethoddb.find(setter_id); it != g_imethoddb.end()) {
            prop_method = it->second;
            setter = it->second;
        }

        // uhhhh
        if (prop_method == nullptr) {
            continue;
        }

        auto& desc = prop_method->owner;

#if TDB_VER >= 69
        const auto impl_id = (uint32_t)p.impl_id;
        auto& impl = (*tdb->propertiesImpl)[impl_id];
        auto name = Address{tdb->stringPool}.get(impl.name_offset).as<const char*>();
#else
        auto name = Address{ tdb->stringPool }.get(p.name_offset).as<const char*>();
#endif

        // ha ha
        auto& pp = desc->parsed_props.emplace_back(std::make_shared<detail::ParsedProperty>());

        pp->name = name;
        pp->owner = desc;
        pp->p = &p;
        pp->getter = getter;
        pp->setter = setter;

#if TDB_VER >= 69
        pp->p_impl = &impl;
#endif

        auto getter_name = pp->getter != nullptr ? pp->getter->name : "";
        auto setter_name = pp->setter != nullptr ? pp->setter->name : "";

        //spdlog::info("{:s}.{:s}", desc->name, name);

        auto& type_entry = il2cpp_dump[desc->full_name];

        type_entry["properties"][pp->name] = {
            {"id", i},
            {"getter", getter_name},
            {"setter", setter_name},
        };

        //spdlog::info("{:s} {:s}.{:s}: 0x{:x}", field_type_name, desc->t->type->name, name, pf->offset_from_base);
    }


    // Try and guess what the field names are for the RSZ entries
    // In RE7, the deserializer points to the reflection property,
    // so we can just grab the name from there instead of comparing field offsets.
    
#if TDB_VER > 49
    m_sdk_dump_stage = SdkDumpStage::DUMP_RSZ_2;
    k = 0;
    n_types = m_sorted_types.size();
    for (auto& t : g_itypedb) {
        m_sdk_dump_progress = static_cast<float>(k++) / n_types;

        auto tdef = t.second->t;

        if (tdef == nullptr || tdef->get_index() == 0) {
            continue;
        }

        auto& t_json = il2cpp_dump[t.second->full_name];

        if (!t_json.contains("RSZ")) {
            continue;
        }

        int32_t i = 0;

        for (auto& rsz_entry : t_json["RSZ"]) {
            const auto is_rsz_static = rsz_entry["static"].get<bool>();
            const auto rsz_offset = std::stoul(std::string{ rsz_entry["offset_from_fieldptr"] }, nullptr, 16);
            auto fieldptr_adjustment = 0;

            auto depth_t = t.second;
            const auto depth = rsz_entry["depth"].get<int32_t>();

            // Get the topmost one because of depth
            for (auto d = 0; d < depth; ++d) {
                if (!depth_t->t->has_fieldptr_offset() || depth_t->super == nullptr) {
                    break;
                }

                const auto field_ptr = depth_t->t->get_fieldptr_offset();

                depth_t = depth_t->super;

                if (!depth_t->t->has_fieldptr_offset()) {
                    break;
                }

                const auto field_ptr2 = depth_t->t->get_fieldptr_offset();

                fieldptr_adjustment += field_ptr - field_ptr2;
            }

            for (auto& f : depth_t->parsed_fields) {
                const auto is_field_static = f->f->is_static();

                if (is_field_static != is_rsz_static) {
                    continue;
                }

                //const auto field_offset = is_field_static ? f->offset_from_fieldptr : (f->offset_from_fieldptr + fieldptr_adjustment);
                const auto field_offset = f->offset_from_fieldptr + fieldptr_adjustment;

                if (field_offset == rsz_offset) {
                    rsz_entry["potential_name"] = f->name;
                    break;
                }
            }
        }
    }
#endif
#endif
    m_sdk_dump_stage = SdkDumpStage::DUMP_DESERIALIZER_CHAIN;
    k = 0;
    n_types = m_sorted_types.size();
    // First pass, gather all valid class names
    for (const auto& name : m_sorted_types) {
        m_sdk_dump_progress = static_cast<float>(k++) / n_types;

        auto t = get_type(name);

        if (t == nullptr || t->name == nullptr) {
            continue;
        }

#ifdef TDB_DUMP_ALLOWED
        export_deserializer_chain(il2cpp_dump, tdb, t);

        auto& entry = il2cpp_dump[t->name];

        if (!entry.contains("fqn")) {
            entry["fqn"] = (std::stringstream{} << std::hex << t->classIndex).str();
        }
        
        if (!entry.contains("crc")) {
            entry["crc"] = (std::stringstream{} << std::hex << t->typeCRC).str();
        }
#endif

        if (t->fields == nullptr) {
            continue;
        }

        // template classes we dont want
        if (std::string{ t->name }.find_first_of("`<>") != std::string::npos) {
            continue;
        }

        g_class_set.insert(t->name);
    }

#if TDB_VER > 49
    m_sdk_dump_stage = SdkDumpStage::DUMP_NON_TDB_TYPES;
    k = 0;
    n_types = m_sorted_types.size();
    for (const auto& name : m_sorted_types) {
        m_sdk_dump_progress = static_cast<float>(k++) / n_types;

        auto t = get_type(name);

        if (t == nullptr || t->name == nullptr) {
            continue;
        }

        if (t->fields == nullptr) {
            continue;
        }

        // template classes we dont want
        if (std::string{ t->name }.find_first_of("`<>") != std::string::npos) {
            continue;
        }

        const auto is_singleton = utility::re_type::is_singleton(t);

        auto c = class_from_name(g, t->name);
        c->size(t->size);

        // make get_type static function
        {
            auto m = c->static_function("get_type_info")->returns(g->type(TYPE_INFO_NAME)->ptr());

            std::stringstream os{};
            os << "return reframework::get_types()->get(\"" << t->name << "\");";

            m->procedure(os.str());
        }

        // make get_type_definition static function
        {
            auto m = c->static_function("get_type_definition")->returns(g->type(TYPE_DEFINITION_NAME)->ptr());

            std::stringstream os{};
            os << "static auto t = sdk::find_type_definition(\"" << t->name << "\");\n";
            os << "return t;";

            m->procedure(os.str());
        }

        // make get_singleton_instance static function
        if (is_singleton) {
            auto m = c->static_function("get_singleton_instance")->returns(c->ptr());

            std::stringstream os{};

            os << "return (" << c->ptr()->usable_name() << ")utility::re_type::get_singleton_instance(get_type_info());";

            m->procedure(os.str());
        }

        auto parent_c = c;

        // generate inheritance
        for (auto super = t->super; super != nullptr; super = super->super) {
            if (super == nullptr || super->name == nullptr) {
                continue;
            }

            if (super->fields == nullptr) {
                continue;
            }

            // template classes we dont want
            if (std::string{super->name}.find_first_of("`<>") != std::string::npos) {
                continue;
            }

            auto s = class_from_name(g, super->name);
            s->size(super->size);

            parent_c->parent(s);
            parent_c = s;
        }

        auto fields = t->fields;
        auto num_methods = fields->num;
        auto methods = fields->methods;

// BORKED RIGHT NOW
#if TDB_VER > 49
        // Generate Methods
        if (fields->methods != nullptr) {
            for (auto i = 0; i < num_methods; ++i) try {
                auto top = (*methods)[i];

                if (top == nullptr) {
                    continue;
                }

                auto& holder = **top;
                auto descriptor = holder.descriptor;

                if (descriptor == nullptr || descriptor->name == nullptr || descriptor->functionPtr == nullptr) {
                    continue;
                }

                // auto ret = descriptor->returnTypeName != nullptr ? std::string{descriptor->returnTypeName} : std::string{"undefined"};

                // reflection methods are kind of shit so ignore them for now.
                /*std::string ret{"void"};

                auto m = c->function(descriptor->name);

                std::ostringstream os{};
                os << "// " << (descriptor->returnTypeName != nullptr ? descriptor->returnTypeName : "") << "\n";

                if (!is_singleton) {
                    os << "return utility::re_managed_object::call_method((REManagedObject*)this, \"" << descriptor->name << "\", *args);";
                }
                else {
                    os << "return utility::re_managed_object::call_method((REManagedObject*)this, utility::re_type::get_method_desc(get_type_info(), \"" << descriptor->name << "\"), *args);";
                }

                m->param("args")->type(g->type("void**"));
                m->procedure(os.str())->returns(g->type("std::unique_ptr<utility::re_managed_object::ParamWrapper>"));*/

#ifdef TDB_DUMP_ALLOWED
// BORKED RIGHT NOW
                json json_params{};

                for (auto f = 0; f < descriptor->numParams; ++f) {
                    auto& param_d = (*descriptor->params)[f];
                    auto param_t = g_itypedb[(*descriptor->params)[f].typeIndex];
                    auto param_typename = (param_t != nullptr && param_d.typeIndex) != 0 ? param_t->full_name : param_d.typeName;

                    json_params.push_back({
                        {"type", param_typename},
                        {"name", param_d.paramName},
                        {"typeindex", param_d.typeIndex}
                    });
                }

                auto return_t = g_itypedb[descriptor->typeIndex];
                auto return_name = (return_t != nullptr && descriptor->typeIndex != 0) ? return_t->full_name : descriptor->returnTypeName;

                il2cpp_dump[t->name]["reflection_methods"][descriptor->name] = {
                    {"function", (std::stringstream{} << "0x" << std::hex << get_original_va(descriptor->functionPtr)).str()},
                    {"returns", return_name},
                    {"params", json_params},
                    {"typeindex", descriptor->typeIndex}
                };
#endif
            } catch(...) {
                continue; // unexplained crash
            }
        }
#endif

        // Generate Properties
        if (fields->variables != nullptr && fields->variables->data != nullptr) {
            auto descriptors = fields->variables->data->descriptors;
            auto reflection_property_index = 0;
            for (auto i = descriptors; i != descriptors + fields->variables->num; ++i) {
                auto variable = *i;

                if (variable == nullptr) {
                    continue;
                }

                /*genny::Function* m = nullptr;

                std::ostringstream os{};
                os << "// " << (variable->typeName != nullptr ? variable->typeName : "") << "\n";

                if (utility::reflection_property::is_static(variable)) {
                    m = c->static_function(variable->name);

                    os << "auto tbl = sdk::VM::get()->get_static_tbl_for_type(*(uint32_t*)get_type_info()->classInfo & 0x3FFF);\n";
                    os << "if (tbl == nullptr) {\n return nullptr;\n}\n";
                    os << "return utility::re_managed_object::get_field<sdk::DummyData>((REManagedObject*)tbl, utility::re_type::get_field_desc(get_type_info(), \"" << variable->name << "\"));\n";

                    m->procedure(os.str());
                }
                else {
                    m = c->function(variable->name);

                    if (!is_singleton) {
                        os << "return utility::re_managed_object::get_field<sdk::DummyData>((REManagedObject*)this, " << "\"" << variable->name << "\");";
                    }
                    else {
                        os << "return utility::re_managed_object::get_field<sdk::DummyData>((REManagedObject*)this, utility::re_type::get_field_desc(get_type_info(), " << "\"" << variable->name << "\"));";
                    }

                    m->procedure(os.str());
                }*/

                auto dummy_type = g->namespace_("sdk")->struct_("DummyData")->size(0x100);
                //m->returns(dummy_type);

#ifdef TDB_DUMP_ALLOWED
                auto field_t = g_fqntypedb[variable->typeFqn];
                auto field_t_name = (field_t != nullptr && variable->typeFqn != 0) ? field_t->full_name : variable->typeName;

                auto& prop_entry = il2cpp_dump[t->name]["reflection_properties"][variable->name];

                prop_entry = {
                    {"getter", (std::stringstream{} << "0x" << std::hex << get_original_va(variable->function)).str()},
                    {"type", field_t_name},
                    {"order", reflection_property_index++},
                };
#endif
            
#if defined(RE8) || defined(MHRISE)
                // Property attributes
                if (variable->attributes != 0 && variable->attributes != -1) {
                    for (auto attr = (REAttribute*)((uintptr_t)&variable->attributes + variable->attributes); attr != nullptr && !IsBadReadPtr(attr, sizeof(REAttribute)) && attr->info != nullptr; attr = attr->next) {
                        auto type_func = (REType* (*)())attr->info->getType;

                        prop_entry["attributes"].emplace_back(
                            json{ {"name", type_func()->name } }
                        );
                    }
                }
#endif
            }
        }
    }
#endif

    // don't.
    /*for (auto& it : this->m_enums) {
        // template classes we dont want
        if (std::string{it.first}.find_first_of("`<>") != std::string::npos) {
            continue;
        }

        auto e = enum_from_name(g, it.first);

        e->type(g->type("uint64_t"));
        e->value(it.second.name, it.second.value);
    }*/

    try {
        std::ofstream{ REFramework::get_persistent_dir("il2cpp_dump.json") } << il2cpp_dump.dump(4, ' ', false, json::error_handler_t::ignore) << std::endl;
    } catch(std::exception& e) {
        spdlog::info("Failed to dump il2cpp_dump.json: {}", e.what());
    }

    /*spdlog::info("Generating SDK...");

    sdk.include("REFramework.hpp");
    sdk.include("sdk/ReClass.hpp");
    sdk.include("cstdint");
    sdk.generate("sdk");*/

    spdlog::info("Generating IDA SDK...");
    m_sdk_dump_stage = SdkDumpStage::GENERATE_SDK;
    
    genny::ida::transform(sdk);
    sdk.generate("sdk_ida");

    // Free a couple gigabytes of no longer used memory
    g_stypedb.clear();
    g_itypedb.clear();
    g_fqntypedb.clear();
    g_iparamdb.clear();
    g_imethoddb.clear();

    m_dumping_sdk = false;
}

void ObjectExplorer::report_sdk_dump_progress(float progress) {
    m_sdk_dump_progress = progress;
}

void ObjectExplorer::handle_address(Address address, int32_t offset, Address parent, Address real_address) {
    // Because this can be called from Lua.
    if (m_do_init) {
        init();
        m_do_init = false;
    }

    if (!is_managed_object(address)) {
        return;
    }

    if (real_address == nullptr) {
        real_address = address;
    }

    auto object = address.as<REManagedObject*>();
    
    if (parent == nullptr) {
        parent = address;
    }

    bool made_node = false;
    const auto is_game_object = utility::re_managed_object::is_a(object, "via.GameObject");
    const auto is_bhvt = utility::re_managed_object::is_a(object, "via.behaviortree.BehaviorTree");
    const auto obj_typedef = utility::re_managed_object::get_type_definition(object);

    if (obj_typedef != nullptr) {
        m_current_path.emplace_back(obj_typedef->get_name());
    }


    if (offset != -1) {
        ImGui::SetNextItemOpen(false, ImGuiCond_::ImGuiCond_Once);

        made_node = stretched_tree_node(parent.get(offset), "0x%X:", offset);
        auto is_hovered = ImGui::IsItemHovered();
        auto additional_text = std::string{};
        auto additional_text2 = std::string{};

        context_menu(real_address);

        if (is_game_object) {
            additional_text = utility::re_game_object::get_name(address.as<REGameObject*>());
        }
        else {
            // Change name based on VMType
            switch (utility::re_managed_object::get_vm_type(object)) {
            case via::clr::VMObjType::Array:
            {
                auto arr = (REArrayBase*)object;
                std::string name{};
                name += "Array<";

                const auto contained_type = utility::re_array::get_contained_type(arr);
                
                name += contained_type != nullptr ? contained_type->get_full_name() : "";

                name += ">";

                additional_text = name;
                break;
            }

            case via::clr::VMObjType::String: {
                additional_text = "String";

                auto t = utility::re_managed_object::get_type(object);

                if (t != nullptr) {
                    auto type_name = std::string{t->name};
                    auto ret = utility::hash(type_name);

                    switch (ret) {
                    case "System.String"_fnv: {
                        auto str = (SystemString*)((uintptr_t)utility::re_managed_object::get_field_ptr(object) - sizeof(REManagedObject));

                        if (str->size > 0) {
                            additional_text2 = utility::narrow(str->data);
                        }

                        break;
                    }
                    default:
                        additional_text2 = "NATIVE_STRING";
                        break;
                    }
                }

                break;
            }
            case via::clr::VMObjType::Delegate: {
                additional_text = "Delegate";
                break;
            }
            case via::clr::VMObjType::ValType:
                additional_text = "ValType";
                break;
            case via::clr::VMObjType::Object: {
                auto t = utility::re_managed_object::get_type(object);

                if (t != nullptr) {
                    additional_text = t->name;

                    auto type_name = std::string{t->name};
                    auto ret = utility::hash(type_name);

                    switch (ret) {
                    case "System.String"_fnv: {
                        auto str = (SystemString*)((uintptr_t)utility::re_managed_object::get_field_ptr(object) - sizeof(REManagedObject));

                        if (str->size > 0) {
                            additional_text2 = utility::narrow(str->data);
                        }

                        break;
                    }
                    default:
                        break;
                    }
                }

                break;
            }
            case via::clr::VMObjType::NULL_:
            default:
                additional_text = "NULL_OBJECT";
                break;
            }
        }

        if (is_hovered) {
            make_same_line_text(additional_text, VARIABLE_COLOR_HIGHLIGHT);
            make_same_line_text(additional_text2, { 1.0f, 0.0f, 1.0f, 1.0f });
        }
        else {
            make_same_line_text(additional_text, VARIABLE_COLOR);
            make_same_line_text(additional_text2, { 1.0f, 0.0f, 0.0f, 1.0f });
        }
    }

    if (made_node || offset == -1) {
        if (is_bhvt) {
            handle_behavior_tree(address.as<sdk::behaviortree::BehaviorTree*>());
        }

        if (is_game_object) {
            handle_game_object(address.as<REGameObject*>());
        }

        if (utility::re_managed_object::is_a(object, "via.Component")) {
            handle_component(address.as<REComponent*>());
        }

        if (utility::re_managed_object::is_a(object, "via.render.RenderLayer")) {
            handle_render_layer(address.as<sdk::renderer::RenderLayer*>());
        }

        handle_type(object, utility::re_managed_object::get_type(object));

        if (utility::re_managed_object::get_vm_type(object) == via::clr::VMObjType::Array) {
            auto arr = (REArrayBase*)object;

            const auto made_array_entries = ImGui::TreeNode(real_address.get(sizeof(REArrayBase)), "Array Entries");

            ImGui::SameLine();
            ImGui::Text("[%d]", arr->numElements);

            if (made_array_entries) {
                const bool entry_is_val = utility::re_array::get_contained_type(arr)->get_vm_obj_type() == via::clr::VMObjType::ValType;

                if (entry_is_val) {
                    for (auto i = 0; i < arr->numElements; ++i) {
                        auto elem = utility::re_array::get_element<void*>(arr, i);
                        REManagedObject fake_obj{};

                        const auto contained_type = utility::re_array::get_contained_type(arr);

                        fake_obj.info = contained_type->get_managed_vt();
                        auto real_size = contained_type->get_size();

                        std::vector<uint8_t> copied_obj{};
                        copied_obj.resize(real_size);

                        memcpy(&copied_obj[0], &fake_obj, sizeof(REManagedObject));
                        memcpy(&copied_obj[sizeof(REManagedObject)], elem, real_size - sizeof(REManagedObject));
                        real_size = utility::re_managed_object::get_size((REManagedObject*)copied_obj.data());
                        copied_obj.resize(real_size);

                        memcpy(&copied_obj[sizeof(REManagedObject)], elem, real_size - sizeof(REManagedObject));
                        handle_address(copied_obj.data(), i, arr, elem);
                    }
                }
                else {
                    for (auto i = 0; i < arr->numElements; ++i) {
                        handle_address(utility::re_array::get_element<void*>(arr, i), i, arr);
                    }
                }

                ImGui::TreePop();
            }
        }

        if (ImGui::TreeNode(real_address.ptr(), "AutoGenerated Types")) {
            auto size = utility::re_managed_object::get_size(object);

            for (auto i = (uint32_t)sizeof(void*); i < size; i += sizeof(void*)) {
                auto ptr = Address(object).get(i).to<REManagedObject*>();

                handle_address(ptr, i, real_address.ptr());
            }

            ImGui::TreePop();
        }
    }

    if (made_node && offset != -1) {
        ImGui::TreePop();
    }

    if (obj_typedef != nullptr) {
        m_current_path.pop_back();
    }
}

void ObjectExplorer::handle_game_object(REGameObject* game_object) {
    ImGui::PushID((void*)game_object);

    if (ImGui::InputText("Add Component", m_add_component_name.data(), 256, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue)) {
        const auto tdef = sdk::find_type_definition(m_add_component_name.data());

        if (tdef != nullptr) {
            auto typeof_component = tdef->get_runtime_type();

            if (typeof_component != nullptr) {
                if (tdef->is_a("via.Component")) {
                    const auto new_comp = sdk::call_object_func<::REComponent*>(game_object, "createComponent(System.Type)", sdk::get_thread_context(), game_object, typeof_component);

                    if (new_comp != nullptr) {
                        auto& pinned = m_pinned_objects.emplace_back();

                        pinned.address = new_comp;
                        pinned.name = tdef->get_name();
                        pinned.path = build_path();
                        m_add_component_name = "Successfully added component";
                    } else {
                        m_add_component_name = "Failed to add component";
                    }
                } else {
                    m_add_component_name = "Type is not a component";
                }
            } else {
                m_add_component_name = "Invalid Type";
            }
        } else {
            m_add_component_name = "Invalid Type";
        }

        m_add_component_name.reserve(256);
    }

    auto game_object_name = utility::re_game_object::get_name(game_object);

    ImGui::Text("Name: %s", game_object_name.c_str());
    make_tree_offset(game_object, offsetof(REGameObject, transform), "Transform");
    make_tree_offset(game_object, offsetof(REGameObject, folder), "Folder");

    ImGui::PopID();
}

void ObjectExplorer::handle_component(REComponent* component) {
    auto display_component_preview = [&](REComponent* comp) {
        if (comp != nullptr && comp->ownerGameObject != nullptr) {
            auto prev_name = utility::re_managed_object::get_type_name(comp);
            auto prev_gameobject_name = utility::re_game_object::get_name(comp->ownerGameObject);

            auto tree_hovered = ImGui::IsItemHovered();

            // Draw the variable name with a color
            if (tree_hovered) {
                make_same_line_text(prev_name, VARIABLE_COLOR_HIGHLIGHT);
                make_same_line_text(prev_gameobject_name, VARIABLE_COLOR_HIGHLIGHT);
            }
            else {
                make_same_line_text(prev_name, VARIABLE_COLOR);
                make_same_line_text(prev_gameobject_name, VARIABLE_COLOR);
            }
        }
    };

    if (ImGui::Button("Destroy Component")) {
        sdk::call_object_func<void*>(component, "destroy", sdk::get_thread_context(), component);
    }

    make_tree_offset(component, offsetof(REComponent, ownerGameObject), "Owner", [&](){  display_component_preview(component); });
    //make_tree_offset(component, offsetof(REComponent, childComponent), "ChildComponent");

    auto children_offset = offsetof(REComponent, childComponent);
    auto children_ptr = Address(component).get(children_offset).to<void*>();

    // Draw children
    if (children_ptr != nullptr) {
        auto made_node = ImGui::TreeNode((uint8_t*)component + children_offset, "0x%X: ChildComponents", children_offset);
        context_menu(children_ptr);

        if (made_node) {
            int32_t count = 0;

            // Iterate the children
            for (auto child = component->childComponent; child != nullptr && child != component; child = child->childComponent) {
                // uh oh
                if (!utility::re_managed_object::is_managed_object(child)) {
                    continue;
                }

                auto child_name = utility::re_managed_object::get_type_name(child);

                made_node = widget_with_context(child, [&]() { return stretched_tree_node(child, "%i", count++); });
                auto tree_hovered = ImGui::IsItemHovered();

                // Draw the variable name with a color
                if (tree_hovered) {
                    make_same_line_text(child_name, VARIABLE_COLOR_HIGHLIGHT);
                }
                else {
                    make_same_line_text(child_name, VARIABLE_COLOR);
                }

                if (made_node) {
                    handle_address(child);
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }

    make_tree_offset(component, offsetof(REComponent, prevComponent), "PrevComponent", [&](){ display_component_preview(component->prevComponent); });
    make_tree_offset(component, offsetof(REComponent, nextComponent), "NextComponent", [&](){ display_component_preview(component->nextComponent); });
}

void ObjectExplorer::handle_transform(RETransform* transform) {

}

void ObjectExplorer::handle_render_layer(sdk::renderer::RenderLayer* layer) {
    if (ImGui::Button("Attempt to Clone")) {
        layer->add_layer(utility::re_managed_object::get_type_definition(layer)->get_type(), layer->m_priority)->clone_layers(layer);
    }

    const auto made_node = ImGui::TreeNode(&layer->m_layers, "Child Layers");
    context_menu(&layer->m_layers);

    if (made_node) {
        int32_t count = 0;

        for (auto child_layer : layer->get_layers()) {
            handle_address(child_layer, count++, layer);
        }

        ImGui::TreePop();
    }
}

void ObjectExplorer::handle_behavior_tree(sdk::behaviortree::BehaviorTree* bhvt) {
    const auto made_node = ImGui::TreeNode(&bhvt->trees, "Trees");

    if (made_node) {
        int32_t count = 0;

        for (auto tree : bhvt->get_trees()) {
            handle_behavior_tree_core_handle(bhvt, tree, count++);
        }

        ImGui::TreePop();
    }
}

void ObjectExplorer::handle_behavior_tree_core_handle(sdk::behaviortree::BehaviorTree* bhvt, sdk::behaviortree::CoreHandle* bhvt_core_handle, uint32_t tree_idx) {
    const auto made_node = ImGui::TreeNode(&bhvt_core_handle->core.tree_object, "Nodes");

    if (made_node) {
        int32_t count = 0;
        const auto tree_object = bhvt_core_handle->get_tree_object();

        if (tree_object != nullptr) {
            auto nodes = tree_object->get_nodes();

            std::sort(nodes.begin(), nodes.end(), [](sdk::behaviortree::TreeNode* a, sdk::behaviortree::TreeNode* b) {
                return a->get_full_name() < b->get_full_name();
            });

            for (auto node : tree_object->get_nodes()) {
                handle_behavior_tree_node(bhvt, node, tree_idx);
            }
        }

        ImGui::TreePop();
    }
}

void ObjectExplorer::handle_behavior_tree_node(sdk::behaviortree::BehaviorTree* bhvt, sdk::behaviortree::TreeNode* node, uint32_t tree_idx) {
    ImGui::PushID(node);

    // Activate node
    if (bhvt != nullptr && ImGui::Button("X")) {
        bhvt->set_current_node(node, tree_idx);
    }

    ImGui::SameLine();

    const auto node_name = node->get_name();
    const auto node_full_name = utility::narrow(node->get_full_name());

    const auto made_node = ImGui::TreeNode(node, node_full_name.data());

    if (made_node) {
        ImGui::Text("ID: %u", node->get_id());
        ImGui::Text("Status1: %i", (int32_t)node->get_status1());
        ImGui::Text("Status2: %i", (int32_t)node->get_status2());

        if (ImGui::TreeNode("Children")) {
            for (auto child : node->get_children()) {
                handle_behavior_tree_node(bhvt, child, tree_idx);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Selector")) {
            handle_address(node->get_selector());

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Selector Condition")) {
            handle_address(node->get_selector_condition());

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
}

void ObjectExplorer::handle_type(REManagedObject* obj, REType* t) {
    if (obj == nullptr || t == nullptr) {
        return;
    }

    auto count = 0;
    const bool is_singleton = utility::re_type::is_singleton(t);
    const bool is_real_obj = utility::re_managed_object::is_managed_object(obj);
    bool is_top_type_open = false;

    for (auto type_info = t; type_info != nullptr; type_info = type_info->super) {
        auto name = type_info->name;

        if (name == nullptr) {
            continue;
        }

        auto obj_for_widget = (type_info == t && is_singleton && !is_real_obj) ? utility::re_type::get_singleton_instance(type_info) : t;

        auto made_node = widget_with_context(obj_for_widget, name, [&name]() { return ImGui::TreeNode(name); });

        // top
        if (is_singleton && type_info == t) {
            make_same_line_text("SINGLETON", ImVec4{1.0f, 0.0f, 0.0f, 1.0f}); 
        }

        if (!made_node) {
            break;
        }

        // top
        if (type_info == t) {
            is_top_type_open = true;
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
            if (stretched_tree_node("Type Information")) {
                if (stretched_tree_node("TypeFlags")) {
                    display_enum_value("via.clr.TypeFlag", (int64_t)((sdk::RETypeDefinition*)type_info->classInfo)->get_flags());
                    ImGui::TreePop();
                }

                const auto td = utility::re_type::get_type_definition(type_info);

                if (td != nullptr) {
                    const auto generic_td = td->get_generic_type_definition();

                    if (generic_td != nullptr) {
                        if (stretched_tree_node("Generic Type Definition")) {
                            ImGui::Text("Name: %s", generic_td->get_full_name().c_str()); // just in-case the get_type() returns nullptr.
                            display_native_methods(nullptr, generic_td);
                            display_native_fields(nullptr, generic_td);
                            ImGui::TreePop();
                        }
                    }
                }

                ImGui::TreePop();
            }
        }

        display_reflection_methods(obj, type_info);
        display_reflection_properties(obj, type_info);

        display_native_methods(obj, utility::re_type::get_type_definition(type_info));
        display_native_fields(obj, utility::re_type::get_type_definition(type_info));
    }

    for (auto i = 0; i < count; ++i) {
        // Mimic handle_address
        if (i == count - 1 && is_top_type_open && is_singleton && !is_real_obj) {
            auto singleton_obj = utility::re_type::get_singleton_instance(t);

            if (singleton_obj != nullptr && ImGui::TreeNode(singleton_obj, "AutoGenerated Types")) {
                auto size = t->size;

                for (auto i = (uint32_t)sizeof(void*); i < size; i += sizeof(void*)) {
                    auto ptr = Address(singleton_obj).get(i).to<REManagedObject*>();

                    handle_address(ptr, i, singleton_obj);
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

void ObjectExplorer::display_enum_value(std::string_view name, int64_t value) {

    auto first_found = get_enum_value_name(name, (int64_t)value);
    if (!first_found.empty()) {
        ImGui::Text("%lld: ", value);
        ImGui::SameLine();
        ImGui::TextColored(VARIABLE_COLOR, "%s", first_found.c_str());
    }
    // Assume it's a set of flags then
    else {
        ImGui::Text("%lld", value);

        std::vector<std::string> names{};

        // Check which bits are set and have enum names
        for (auto i = 0; i < 64; ++i) {
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

void ObjectExplorer::display_reflection_methods(REManagedObject* obj, REType* type_info) {
    if (type_info->fields == nullptr) {
        return;
    }

    volatile auto methods = type_info->fields->methods;

    if (methods == nullptr || *methods == nullptr) {
        return;
    }

    auto num_methods = type_info->fields->num;

    if (ImGui::TreeNode(methods, "Reflection Methods: %i", num_methods)) {
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
            auto made_node = widget_with_context(descriptor, descriptor->name, [&]() { return stretched_tree_node(descriptor, "%s", ret.c_str()); });
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

                if (descriptor->functionPtr != nullptr && ImGui::Button("Attempt to call")) {
                    char poop[0x100]{ 0 };
                    utility::re_managed_object::call_method(obj, descriptor->name, poop);
                }

                auto t2 = get_type(ret);

                if (t2 == nullptr || t2 == type_info) {
                    ImGui::Text("Type: %s", ret.c_str());
                }
                else {
                    std::vector<uint8_t> fake_object(t2->size, 0);

                    handle_type((REManagedObject*)fake_object.data(), t2);
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

void ObjectExplorer::display_reflection_properties(REManagedObject* obj, REType* type_info) {
    if (type_info->fields == nullptr || type_info->fields->variables == nullptr || type_info->fields->variables->data == nullptr) {
        return;
    }

    const auto is_real_object = utility::re_managed_object::is_managed_object(obj);
    auto descriptors = type_info->fields->variables->data->descriptors;

    if (ImGui::TreeNode(type_info->fields, "Reflection Properties: %i", type_info->fields->variables->num)) {
        for (auto i = descriptors; i != descriptors + type_info->fields->variables->num; ++i) {
            auto variable = *i;

            if (variable == nullptr) {
                continue;
            }

            auto local_obj = obj;

            auto made_node = widget_with_context(variable->function, variable->name, [&]() { return stretched_tree_node(variable, "%s", variable->typeName); });
            auto tree_hovered = ImGui::IsItemHovered();

            // Draw the variable name with a color
            if (tree_hovered) {
                make_same_line_text(variable->name, VARIABLE_COLOR_HIGHLIGHT);
            }
            else {
                make_same_line_text(variable->name, VARIABLE_COLOR);
            }

            // Display the field offset
            if (is_real_object || utility::re_type::is_singleton(type_info)) {
                if (!is_real_object && utility::re_type::is_singleton(type_info)) {
                    local_obj = (REManagedObject*)utility::re_type::get_singleton_instance(type_info);
                }

                if (local_obj != nullptr) {
                    auto offset = get_field_offset(local_obj, variable, type_info);

                    if (offset != 0) {
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4{1.0f, 0.0f, 0.0f, 1.0f}, "0x%X", offset);
                    }
                }
            }

#if defined(RE8) || defined(MHRISE)
            const auto allowed = is_real_object || utility::reflection_property::is_static(variable) || utility::re_type::is_singleton(type_info);

            // Set the obj to the static table so we can get static variables
            if (utility::reflection_property::is_static(variable)) {
                const auto type_index = ((sdk::RETypeDefinition*)type_info->classInfo)->get_index();

                local_obj = (REManagedObject*)sdk::VM::get()->get_static_tbl_for_type(type_index);

                make_same_line_text("STATIC", ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f });
            }
#else
            const auto allowed = is_real_object || utility::re_type::is_singleton(type_info);
#endif

            // Info about the field
            if (made_node) {
                if (allowed && local_obj != nullptr) {
                    attempt_display_field(local_obj, variable, type_info);
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
                            std::vector<uint8_t> fake_object(t2->size, 0);

                            handle_type((REManagedObject*)fake_object.data(), t2);
                        }
                    }

                    auto prop_flags = utility::reflection_property::get_flags(variable);

                    ImGui::Text("TypeKind: %i (%s)", prop_flags.type_kind, get_enum_value_name("via.reflection.TypeKind", (int64_t)prop_flags.type_kind).c_str());
                    ImGui::Text("Qualifiers: %i", prop_flags.type_qual);
                    ImGui::Text("Attributes: %i", prop_flags.type_attr);
                    
                    ImGui::Text("Size: %i", utility::reflection_property::get_size(variable));
                    ImGui::Text("ManagedStr: %i", prop_flags.managed_str);
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

void ObjectExplorer::display_native_fields(REManagedObject* obj, sdk::RETypeDefinition* tdef) {
//#ifdef TDB_DUMP_ALLOWED
    if (tdef == nullptr) {
        return;
    }

    const auto is_real_object = utility::re_managed_object::is_managed_object(obj);
    auto fields = tdef->get_fields();

    if (fields.size() == 0) {
        return;
    }

    bool owner_is_valuetype = tdef->get_vm_obj_type() == via::clr::VMObjType::ValType;

    auto tdb = reframework::get_types()->get_type_db();

    if (ImGui::TreeNode(*fields.begin(), "TDB Fields: %i", fields.size())) {
        for (auto f : fields) {
            if (!is_filtered_field(*f)) {
                continue;
            }

            const auto field_declaring_type = f->get_declaring_type();
            const auto field_flags = f->get_flags();
            const auto field_type = f->get_type();
            const auto field_type_name = field_type->get_full_name();
            const auto field_name = f->get_name();
            const auto fieldptr_offset = f->get_offset_from_fieldptr();
            const auto is_valuetype = field_type->is_value_type();
            const auto is_enum = field_type->is_enum();
            bool is_managed_str = false;

            auto offset = fieldptr_offset;
            void* data = nullptr;

            std::string final_type_name = field_type_name;

            std::vector<std::string> postfixes{};

            // Handle static fields
            if ((field_flags & (uint16_t)via::clr::FieldFlag::Static) != 0) {
                postfixes.push_back("STATIC");

                // Const
                if ((field_flags & (uint16_t)via::clr::FieldFlag::Literal) != 0) {
                    postfixes.push_back("CONST");

                    data = f->get_init_data();

                    switch (utility::hash(field_type_name)) {
                    case "System.String"_fnv:
                        final_type_name = "c8";
                        break;

                    default:
                        break;
                    }
                }
                else {
                    data = f->get_data_raw();

                    if (data != nullptr && !is_valuetype) {
                        data = *(void**)data;
                    }
                }
            }
            else { // Handle normal field
                if (!owner_is_valuetype || is_real_object) {
                    offset += tdef->get_fieldptr_offset();
                }

                // Draw the offset
                postfixes.push_back((std::stringstream{} << "0x" << std::uppercase << std::hex << offset).str());

                if (obj != nullptr) {
                    data = Address{ obj }.get(offset);

                    if (!is_valuetype) {
                        data = *(void**)data;
                    }
                }
            }

            is_managed_str = final_type_name == "System.String";

            const auto made_node = widget_with_context(data, field_name, [&]() { return stretched_tree_node(f, "%s", field_type_name.c_str()); });
            const auto tree_hovered = ImGui::IsItemHovered();

            // Draw the variable name with a color
            if (tree_hovered) {
                make_same_line_text(field_name, VARIABLE_COLOR_HIGHLIGHT);
            } else {
                make_same_line_text(field_name, VARIABLE_COLOR);
            }

            // draw stuff after the field like the offset
            for (auto postfix : postfixes) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, postfix.c_str());
            }

            // draw the field data
            if (made_node) {
                display_data(data, data, final_type_name, is_enum, is_managed_str, (is_valuetype || is_enum) ? field_type : nullptr);

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
//#else
    //return;
//#endif
}

void ObjectExplorer::populate_method_meta_info(sdk::REMethodDefinition& m) {
    const auto method_ptr = m.get_function();

    if (method_ptr == nullptr) {
        return;
    }

    if (m_method_meta_infos.contains((uintptr_t)method_ptr)) {
        return;
    }

    m_method_meta_infos[(uintptr_t)method_ptr] = std::make_unique<MethodMetaInfo>();

    if (IsBadReadPtr(method_ptr, sizeof(void*))) {
        return;
    }

    std::unordered_set<sdk::REMethodDefinition*> called_functions{};

    // Disassemble the function's instructions looking for calls to other functions, and add them to the list of methods this function calls
    utility::exhaustive_decode((uint8_t*)method_ptr, 5000, [&](utility::ExhaustionContext& ctx) -> utility::ExhaustionResult {
        if (ctx.addr != (uintptr_t)method_ptr) {
            if (auto it = m_method_map.find(ctx.addr); it != m_method_map.end()) {
                called_functions.insert(it->second);
            }
        }

        if (std::string_view{ctx.instrux.Mnemonic}.starts_with("CALL")) {
            const auto resolved_addr = utility::resolve_displacement(ctx.addr);

            if (resolved_addr) {
                if (auto it = m_method_map.find(*resolved_addr); it != m_method_map.end()) {
                    called_functions.insert(it->second);
                }
            }

            // Step over all calls, we don't want to go down a rabbit hole that freezes the program
            return utility::ExhaustionResult::STEP_OVER;
        }

        return utility::ExhaustionResult::CONTINUE;
    });

    for (auto& called_function : called_functions) {
        m_method_meta_infos[(uintptr_t)method_ptr]->called_functions.push_back(called_function);
    }

    std::sort(
        m_method_meta_infos[(uintptr_t)method_ptr]->called_functions.begin(), 
        m_method_meta_infos[(uintptr_t)method_ptr]->called_functions.end(), 
        [](sdk::REMethodDefinition* a, sdk::REMethodDefinition* b) 
    {
        const auto decltype_a = a->get_declaring_type();
        const auto decltype_b = b->get_declaring_type();
        const auto fullname_a = decltype_a != nullptr ? decltype_a->get_full_name() + "." + a->get_name() : a->get_name();
        const auto fullname_b = decltype_b != nullptr ? decltype_b->get_full_name() + "." + b->get_name() : b->get_name();
        
        return fullname_a < fullname_b;
    });
}

void ObjectExplorer::attempt_display_method(REManagedObject* obj, sdk::REMethodDefinition& m, bool use_full_name) {
    populate_method_meta_info(m);

    const auto declaring_type = m.get_declaring_type();
    const auto method_name = (use_full_name && declaring_type != nullptr) ? declaring_type->get_full_name() + "." + m.get_name() : m.get_name();
    const auto method_return_type = m.get_return_type();
    const auto method_return_type_name = method_return_type != nullptr ? method_return_type->get_full_name() : std::string{};
    const auto method_param_types = m.get_param_types();
    const auto method_param_names = m.get_param_names();
    const auto method_virtual_index = m.get_virtual_index();
    const auto method_flags = m.get_flags();
    const auto method_impl_flags = m.get_impl_flags();

    const auto method_ptr = m.get_function();

    // Create a c-style function prototype string from the param types and names
    std::stringstream ss{};
    ss << method_name << "(";

    std::stringstream ss_context{};
    ss_context << method_name << "(";

    for (auto i = 0; i < method_param_types.size(); i++) {
        if (i > 0) {
            ss << ", ";
            ss_context << ", ";
        }
        ss << method_param_types[i]->get_full_name() << " " << method_param_names[i];
        ss_context << method_param_types[i]->get_full_name();
    }

    ss << ")";
    ss_context << ")";
    const auto method_prototype = ss.str();
    const auto method_prototype_context = ss_context.str();

    const auto made_node = stretched_tree_node(&m, "%s", method_return_type_name.c_str());
    method_context_menu(&m, method_prototype_context, obj);
    
    const auto tree_hovered = ImGui::IsItemHovered();

    // Draw the method name with a color
    if (tree_hovered) {
        make_same_line_text(method_prototype, VARIABLE_COLOR_HIGHLIGHT);
    } else {
        make_same_line_text(method_prototype, VARIABLE_COLOR);
    }

    bool is_stub = m_known_stub_methods.find(method_ptr) != m_known_stub_methods.end();
    bool is_ok_method = m_ok_methods.find(method_ptr) != m_ok_methods.end();

    if (method_ptr != nullptr && !is_stub && !is_ok_method) {
        if (utility::is_stub_code((uint8_t*)method_ptr)) {
            m_known_stub_methods.insert(method_ptr);

            is_ok_method = false;
            is_stub = true;
        } else {
            m_ok_methods.insert(method_ptr);
        }
    }
    
    if (method_ptr == nullptr || is_stub) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, "STUB");
    }

    bool is_duplicate = m_function_occurrences[method_ptr] > 5;

    if (is_duplicate) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, "DUPLICATE");
    }

    // draw the method data
    if (made_node) {
        if (ImGui::BeginTable("##method", 4,  ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
            ImGui::TableNextColumn();
            ImGui::Text("Address");

            ImGui::TableNextColumn();
            ImGui::Text("Virtual Index");

            ImGui::TableNextColumn();
            ImGui::Text("Flags");

            ImGui::TableNextColumn();
            ImGui::Text("Impl flags");
            
            // address
            ImGui::TableNextColumn();
            ImGui::Text("0x%p", method_ptr);

            // virtual index
            ImGui::TableNextColumn();
            ImGui::Text("%i", method_virtual_index);

            // flags
            ImGui::TableNextColumn();
            ImGui::Text("%s", get_full_enum_value_name("via.clr.MethodFlag", method_flags).c_str());
            
            // impl flags
            ImGui::TableNextColumn();
            ImGui::Text("%s", get_full_enum_value_name("via.clr.MethodImplFlag", method_impl_flags).c_str());

            ImGui::EndTable();
        }

        if (method_param_types.size() > 0) {
            if (ImGui::BeginTable("##params", 3,  ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
                ImGui::TableNextColumn();
                ImGui::Text("Index");
                ImGui::TableNextColumn();
                ImGui::Text("Type");
                ImGui::TableNextColumn();
                ImGui::Text("Name");

                for (auto i = 0; i < method_param_types.size(); i++) {
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(i).c_str());
                    ImGui::TableNextColumn();

                    const auto param_typedef = method_param_types[i];
                    const auto param_type_full_name = param_typedef->get_full_name();
                    const auto param_type = param_typedef->get_type();

                    if (param_type != nullptr) {
                        std::vector<uint8_t> fake_object(param_typedef->get_size(), 0);

                        this->handle_type((REManagedObject*)fake_object.data(), param_typedef->get_type());
                    } else {
                        ImGui::Text(param_type_full_name.c_str());
                    }

                    ImGui::TableNextColumn();
                    ImGui::TextColored(VARIABLE_COLOR, method_param_names[i]);
                }

                ImGui::EndTable();
            }
        }

        if (ImGui::BeginTable("##calls", 1,  ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
            ImGui::TableNextColumn();
            ImGui::Text("Called Method");

            // Show a list of methods this method calls
            if (m_method_meta_infos.contains((uintptr_t)method_ptr)) {
                for (const auto& called_m : m_method_meta_infos[(uintptr_t)method_ptr]->called_functions) {
                    ImGui::TableNextColumn();
                    attempt_display_method(nullptr, *called_m, true);
                }
            }

            ImGui::EndTable();
        }

        if (ImGui::BeginTable("##disassembly", 3,  ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
            ImGui::TableNextColumn();
            ImGui::Text("Address");

            ImGui::TableNextColumn();
            ImGui::Text("Bytes");

            ImGui::TableNextColumn();
            ImGui::Text("Instruction");

            // Show a short disassembly of the method
            auto ip = (uintptr_t)method_ptr;

            for (auto i = 0; i < 20; i++) {
                const auto decoded = utility::decode_one((uint8_t*)ip);

                if (!decoded) {
                    break;
                }

                char buffer[ND_MIN_BUF_SIZE]{};
                NdToText(&*decoded, 0, sizeof(buffer), buffer);

                ImGui::TableNextColumn();
                ImGui::Text("0x%p", ip);

                ImGui::TableNextColumn();
                // show the bytes
                for (auto j = 0; j < decoded->Length; j++) {
                    if (j > 0) {
                        ImGui::SameLine();
                    }

                    if (ip) ImGui::Text("%02X", ((uint8_t*)ip)[j]);
                }

                ImGui::TableNextColumn();
                ImGui::Text(buffer);

                ip += decoded->Length;

                // check if int3 and stop
                if (std::string_view{decoded->Mnemonic}.starts_with("INT3")) {
                    break;
                }
            }

            ImGui::EndTable();
        }

        ImGui::TreePop();
    }
}

void ObjectExplorer::display_native_methods(REManagedObject* obj, sdk::RETypeDefinition* tdef) {
    if (tdef == nullptr) {
        return;
    }

    const auto is_real_object = utility::re_managed_object::is_managed_object(obj);
    auto methods = tdef->get_methods();
    auto tdb = reframework::get_types()->get_type_db();

    if (methods.size() == 0) {
        return;
    }
    
    if (ImGui::TreeNode(methods.begin(), "TDB Methods: %i", methods.size())) {
        for (auto& m : methods) {
            if (!is_filtered_method(m)) {
                continue;
            }

            attempt_display_method(obj, m);   
        }

        ImGui::TreePop();
    }
}

void ObjectExplorer::attempt_display_field(REManagedObject* obj, VariableDescriptor* desc, REType* type_info) {
    if (desc->function == nullptr) {
        return;
    }

    auto type_name = std::string{ desc->typeName };
    auto ret = utility::hash(type_name);
    auto get_value_func = (void* (*)(VariableDescriptor*, REManagedObject*, void*))desc->function;

    char raw_data[0x100]{ 0 };

    get_value_func(desc, obj, &raw_data);
    
    auto prop_flags = utility::reflection_property::get_flags(desc);
    const auto is_pointer = prop_flags.managed_str || prop_flags.type_attr == 1 || prop_flags.type_attr == 2;

    if (is_pointer && *(void**)&raw_data == nullptr) {
        ImGui::Text("Null pointer");
        return;
    }

    auto data = (is_pointer ? *(char**)&raw_data : (char*)&raw_data);
    auto field_offset = get_field_offset(obj, desc, type_info);

    void* real_data = nullptr;

    if (field_offset != 0) {
        real_data = Address{ obj }.get(field_offset);
    }

    // i don't understand why the game does this.
    if (prop_flags.managed_str != 0 && prop_flags.type_attr == 2 && data != nullptr) {
        data = *(char**)data;
    }

    static ::reframework::InvokeRet dummy_data{};
    static ::reframework::InvokeRet untampered_data{};
    sdk::REMethodDefinition* getter{nullptr};
    sdk::REMethodDefinition* setter{nullptr};

    // For reflection properties where we cant detect the offset
    // but there is a TDB getter and setter for it
    if (real_data == nullptr && obj != nullptr) {
        // Attempt to find a getter/setter for this field
        const auto tdef = utility::re_managed_object::get_type_definition(obj);

        if (tdef != nullptr) {
            getter = tdef->get_method(std::string{"get_"} + desc->name);
            setter = tdef->get_method(std::string{"set_"} + desc->name);

            if (getter != nullptr && setter != nullptr) {
                dummy_data = getter->invoke(obj, {});
                memcpy(&untampered_data, &dummy_data, sizeof(dummy_data));
                real_data = &dummy_data;
            }
        }
    }

    display_data(data, real_data, type_name, prop_flags.type_kind == (uint32_t)via::reflection::TypeKind::Enum, prop_flags.managed_str != 0);

    // TDB alternative setter
    if (getter != nullptr && setter != nullptr) {
        // compare the data
        if (memcmp(&dummy_data, &untampered_data, sizeof(dummy_data)) != 0) {
            const auto result_type = getter->get_return_type();
            const auto should_pass_result_ptr = result_type != nullptr && result_type->is_value_type() && (result_type->get_valuetype_size() > sizeof(void*) || (!result_type->is_primitive() && !result_type->is_enum()));

            if (result_type == nullptr) {
                setter->invoke(obj, dummy_data.ptr);
            } else {
                if (should_pass_result_ptr) {
                    setter->invoke(obj, (void*)dummy_data.bytes.data());
                } else {
                    setter->invoke(obj, dummy_data.ptr);
                }
            }
        }
    }
}

void ObjectExplorer::display_data(void* data, void* real_data, std::string type_name, bool is_enum, bool managed_str, const sdk::RETypeDefinition* override_def) {
    if (data == nullptr) {
        ImGui::Text("Null pointer");
        return;
    }

    constexpr auto min_i8 = std::numeric_limits<int8_t>::min();
    constexpr auto max_i8 = std::numeric_limits<int8_t>::max();
    constexpr auto min_i16 = std::numeric_limits<int16_t>::min();
    constexpr auto max_i16 = std::numeric_limits<int16_t>::max();
    constexpr auto min_int = std::numeric_limits<int32_t>::min();
    constexpr auto max_int = std::numeric_limits<int32_t>::max();
    constexpr auto min_int64 = std::numeric_limits<int64_t>::min();
    constexpr auto max_int64 = std::numeric_limits<int64_t>::max();

    constexpr auto min_u8 = std::numeric_limits<uint8_t>::min();
    constexpr auto max_u8 = std::numeric_limits<uint8_t>::max();
    constexpr auto min_u16 = std::numeric_limits<uint16_t>::min();
    constexpr auto max_u16 = std::numeric_limits<uint16_t>::max();
    constexpr auto min_uint = std::numeric_limits<uint32_t>::min();
    constexpr auto max_uint = std::numeric_limits<uint32_t>::max();
    constexpr auto min_uint64 = std::numeric_limits<uint64_t>::min();
    constexpr auto max_uint64 = std::numeric_limits<uint64_t>::max();

    // uh okay
    constexpr auto min_float = std::numeric_limits<float>::lowest();
    constexpr auto max_float = std::numeric_limits<float>::max();

    constexpr auto min_zero = 0;

    // yay for compile time string hashing
    switch (utility::hash(type_name)) {
    case "via.GameObjectRef"_fnv: {
        static auto object_ref_type = sdk::find_type_definition("via.GameObjectRef");
        auto obj = sdk::call_native_func_easy<REGameObject*>(data, object_ref_type, "get_Target");

        if (obj != nullptr && is_managed_object(obj)) {
            if (widget_with_context(obj, [&]() { return ImGui::TreeNode(obj, "Ref Target: 0x%p", obj); })) {
                if (is_managed_object(obj)) {
                    handle_address(obj);
                }

                ImGui::TreePop();
            }
        }
    } break;

    case "System.UInt64"_fnv:
    case "size_t"_fnv:
    case "u64"_fnv:
        ImGui::Text("0x%llX", *(uint64_t*)data);

        if (real_data != nullptr) {
            auto& int_val = *(uint64_t*)real_data;

            ImGui::DragScalar("Set Value", ImGuiDataType_U64, &int_val, 1.0f, &min_uint64, &max_uint64);
        }

        break;
    case "System.Int64"_fnv:
    case "s64"_fnv:
        ImGui::Text("0x%llX", *(int64_t*)data);

        if (real_data != nullptr) {
            auto& int_val = *(int64_t*)real_data;

            ImGui::DragScalar("Set Value", ImGuiDataType_S64, &int_val, 1.0f, &min_int64, &max_int64);
        }

        break;
    case "System.UInt32"_fnv:
    case "u32"_fnv:
        ImGui::Text("0x%X", *(uint32_t*)data);

        if (real_data != nullptr) {
            auto& int_val = *(uint32_t*)real_data;

            ImGui::DragInt("Set Value", (int*)&int_val, 1.0f, min_uint, max_uint);
        }

        break;
    case "System.Int32"_fnv:
    case "s32"_fnv:
        ImGui::Text("0x%X", *(int32_t*)data);

        if (real_data != nullptr) {
            auto& int_val = *(int32_t*)real_data;

            ImGui::DragInt("Set Value", (int*)&int_val, 1.0f, min_int, max_int);
        }

        break;

    case "System.UInt16"_fnv:
    case "u16"_fnv:
        ImGui::Text("0x%04X", *(uint16_t*)data);

        if (real_data != nullptr) {
            auto& int_val = *(uint16_t*)real_data;

            ImGui::DragScalar("Set Value", ImGuiDataType_U16, &int_val, 1.0f, &min_u16, &max_u16);
        }
        break;

    case "System.Int16"_fnv:
    case "s16"_fnv:
        ImGui::Text("0x%04X", *(int16_t*)data);

        if (real_data != nullptr) {
            auto& int_val = *(int16_t*)real_data;

            ImGui::DragScalar("Set Value", ImGuiDataType_S16, &int_val, 1.0f, &min_i16, &max_i16);
        }
        break;
    case "System.Byte"_fnv:
    case "u8"_fnv:
        ImGui::Text("0x%02X", *(uint8_t*)data);

        if (real_data != nullptr) {
            auto& int_val = *(uint8_t*)real_data;

            ImGui::DragScalar("Set Value", ImGuiDataType_U8, &int_val, 1.0f, &min_u8, &max_u8);
        }

        break;
    case "System.SByte"_fnv:
    case "s8"_fnv:
        ImGui::Text("0x%02X", *(int8_t*)data);

        if (real_data != nullptr) {
            auto& int_val = *(int8_t*)real_data;

            ImGui::DragScalar("Set Value", ImGuiDataType_S8, &int_val, 1.0f, &min_i8, &max_i8);
        }

        break;
    //case "System.Nullable`1<System.Single>"_fnv:
    case "System.Single"_fnv:
    case "f32"_fnv:
    case "float"_fnv: {
        ImGui::Text("%f", *(float*)data);

        if (real_data != nullptr) {
            auto& float_val = *(float*)real_data;

            ImGui::DragFloat("Set Value", &float_val, 0.01f, min_float, max_float);
        }

        break;
    }
    //case "System.Nullable`1<System.Boolean>"_fnv:
    case "System.Boolean"_fnv:
    case "bool"_fnv:
        if (*(bool*)data) {
            ImGui::TextUnformatted("true");
        } else {
            ImGui::TextUnformatted("false");
        }

        if (real_data != nullptr) {
            auto& bool_val = *(bool*)real_data;

            ImGui::Checkbox("Set Value", &bool_val);
        }

        break;
    case "System.Char"_fnv:
    case "c16"_fnv:
        ImGui::Text("%s", utility::narrow((wchar_t*)data).c_str());
        break;
    case "c8"_fnv:
        ImGui::Text("%s", (char*)data);
        break;
    //case "System.Nullable`1<via.vec2>"_fnv:
    case "via.Rect"_fnv:
    case "via.Size"_fnv:
    case "via.Float2"_fnv:
    case "via.vec2"_fnv: {
        auto vec = (Vector2f*)data;

        ImGui::Text("%f %f", vec->x, vec->y);

        if (real_data != nullptr) {
            auto& vec_val = *(Vector2f*)real_data;

            ImGui::DragFloat2("Set Value", (float*)&vec_val, 0.01f, min_float, max_float);
        }

        break;
    }
    //case "System.Nullable`1<via.vec3>"_fnv:
    case "via.Float3"_fnv:
    case "via.vec3"_fnv: {
        auto vec = (Vector3f*)data;

        if (vec != nullptr) {
            ImGui::Text("%f %f %f", vec->x, vec->y, vec->z);
        }

        if (real_data != nullptr) {
            auto& vec_val = *(Vector3f*)real_data;

            ImGui::DragFloat3("Set Value", (float*)&vec_val, 0.01f, min_float, max_float);
        }

        break;
    }
    case "via.Float4"_fnv:
    case "via.vec4"_fnv:
    case "via.Quaternion"_fnv: {
        auto& quat = *(glm::quat*)data;
        ImGui::Text("%f %f %f %f", quat.x, quat.y, quat.z, quat.w);

        if (real_data != nullptr) {
            auto& vec_val = *(Vector4f*)real_data;

            ImGui::DragFloat4("Set Value", (float*)&vec_val, 0.01f, min_float, max_float);
        }

        break;
    }
    case "via.mat4"_fnv: {
        auto& mat = *(glm::mat4*)data;

        for (int i = 0; i < 4; i++) {
            ImGui::Text("%f %f %f %f", mat[i].x, mat[i].y, mat[i].z, mat[i].w);

            if (real_data != nullptr) {
                auto& mat_val = *(glm::mat4*)real_data;

                ImGui::PushID(&mat_val[i]);
                ImGui::DragFloat4("Set Value", (float*)&mat_val[i], 0.01f, min_float, max_float);
                ImGui::PopID();
            }
        }

        break;
    };
    case "System.String"_fnv:
    case "via.string"_fnv: {
        if (managed_str) {
            auto ptr = (SystemString*)data;

            if (ptr != nullptr) {
                ImGui::Text("%s", utility::re_string::get_string(*ptr).c_str());
            } else {
                ImGui::Text("");
            }
        } else {
            ImGui::Text("%s", utility::re_string::get_string(*(REString*)data).c_str());
        }
    } break;
        default: {
            if (is_enum) {
                if (override_def != nullptr) {
                    switch (override_def->get_underlying_type()->get_valuetype_size()) {
                        case 1:
                            display_enum_value(type_name, *(int8_t*)data);
                            if (real_data != nullptr) {
                                auto& int_val = *(int8_t*)real_data;

                                ImGui::DragScalar("Set Value", ImGuiDataType_S8, &int_val, 1.0f, &min_i8, &max_i8);
                            }
                            break;
                        case 2:
                            display_enum_value(type_name, *(int16_t*)data);
                            if (real_data != nullptr) {
                                auto& int_val = *(int16_t*)real_data;

                                ImGui::DragScalar("Set Value", ImGuiDataType_S16, &int_val, 1.0f, &min_i16, &max_i16);
                            }
                            break;
                        case 4:
                            display_enum_value(type_name, *(int32_t*)data);
                            if (real_data != nullptr) {
                                auto& int_val = *(int32_t*)real_data;

                                ImGui::DragInt("Set Value", (int*)&int_val, 1.0f, min_int, max_int);
                            }
                            break;
                        case 8:
                            display_enum_value(type_name, *(int64_t*)data);
                            if (real_data != nullptr) {
                                auto& int_val = *(int64_t*)real_data;

                                ImGui::DragScalar("Set Value", ImGuiDataType_S64, &int_val, 1.0f, &min_int64, &max_int64);
                            }
                            break;
                        default:
                            ImGui::Text("Invalid enum size, falling back to int32");

                            display_enum_value(type_name, *(int32_t*)data);
                            if (real_data != nullptr) {
                                auto& int_val = *(int32_t*)real_data;

                                ImGui::DragInt("Set Value", (int*)&int_val, 1.0f, min_int, max_int);
                            }
                            break;
                    }
                } else {
                    auto value = *(int32_t*)data;
                    display_enum_value(type_name, (int32_t)value);

                    if (real_data != nullptr) {
                        auto& int_val = *(int32_t*)real_data;

                        ImGui::DragInt("Set Value", (int*)&int_val, 1.0f, min_int, max_int);
                    }
                }
            } 
            else {
            
                //auto make_tree_addr = [this](void* addr) {
                //    if (widget_with_context(addr, [&]() { return ImGui::TreeNode(addr, "Variable: 0x%p", addr); })) {
                //        if (is_managed_object(addr)) {
                //            handle_address(addr);
                //        }

                //        ImGui::TreePop();
                //    }
                //};
                //make_tree_addr(data);
                if (widget_with_context(data, type_name, [&]() { return ImGui::TreeNode(data, "Variable: 0x%p", data); })) {
                    if (is_managed_object(data)) {
                        handle_address(data);
                    }

                    ImGui::TreePop();
                }

                if (override_def != nullptr) {
                    const auto override_t = override_def->get_type();

                    if (override_t != nullptr) {
                        handle_type((REManagedObject*)data, override_t);
                    }
                }
            }

            break;
        }
    }
}

int32_t ObjectExplorer::get_field_offset(REManagedObject* obj, VariableDescriptor* desc, REType* type_info) {
    if (desc->typeName == nullptr || desc->function == nullptr || m_offset_map.find(desc) != m_offset_map.end()) {
        return m_offset_map[desc];
    }

    const auto parent_hash = utility::hash(std::string{ type_info->name });
    const auto name_hash = utility::hash(std::string{ desc->name });
    const auto ret_hash = utility::hash(std::string{ desc->typeName });

    // These usually modify the object state, not what we want.
    if (ret_hash == "undefined"_fnv) {
        return m_offset_map[desc];
    }

    // this function modifies some state which causes an unrecoverable exception
    // not very interested to figure out a generic way to fix it right now
    if (parent_hash == "via.wwise.WwiseDriver"_fnv && name_hash == "AudioInput"_fnv) {
        return m_offset_map[desc];
    }

    if (parent_hash == "via.wwise.WwiseManager"_fnv && name_hash == "ObsOclTargetParamList"_fnv) {
        return m_offset_map[desc];
    }

    if (parent_hash == "via.wwise.WwiseManager"_fnv && name_hash == "MaterialObsOclParamList"_fnv) {
        return m_offset_map[desc];
    }

    if (parent_hash == "via.wwise.WwiseContainer"_fnv && name_hash == "TriggerIdByFsm"_fnv) {
        return m_offset_map[desc];
    }

    if (parent_hash == "via.ResourceManager"_fnv && name_hash == "Loading"_fnv) {
        return m_offset_map[desc];
    }

    if (parent_hash == "via.gui.TransformObject"_fnv && name_hash == "WorldMatrix"_fnv) {
        return m_offset_map[desc];
    }

    if (parent_hash == "via.gui.TransformObject"_fnv && name_hash == "GlobalPosition"_fnv) {
        return m_offset_map[desc];
    }

    auto thread_context = sdk::get_thread_context();

    // Set up our "translator" to throw on any exception,
    // Particularly access violations.
    // Kind of gross but it's necessary for some fields,
    // because the field function may access the thing we modified, which may actually be a pointer,
    // and we need to handle it.
    sdk::VMContext::ScopedTranslator scoped_translator{thread_context};

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
    const auto class_size = utility::re_managed_object::is_managed_object(obj) ? utility::re_managed_object::get_size(obj) : type_info->size;
    const auto size = 1;

    // Copy the object so we don't cause a crash by replacing
    // data that's being used by the game
    std::vector<uint8_t> object_copy(class_size);
    memcpy(object_copy.data(), obj, class_size);

    auto first_time = std::chrono::high_resolution_clock::now();

    spdlog::info("{:d} {:s}", GetCurrentThreadId(), desc->name);

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
            catch (const sdk::VMContext::Exception&) {
                thread_context->cleanup_after_exception(scoped_translator.get_prev_reference_count());

                same = false;
                break;
            }

            // Check if result is the same at our offset.
            same = tester.is_value_same(data.data());

            // Check it by dereferencing it now
            if (!same) {
                const bool deref = desc->variableType == 0 && *(void**)data.data() != nullptr && !IsBadReadPtr(*(void**)data.data(), 1);

                if (deref) {
                    same = tester.is_value_same(deref ? *(uint8_t**)data.data() : data.data());
                }
            }

            if (!same) {
                break;
            }

            // Modify the data for our second run.
            *ptr ^= 1;
        }

        auto elapsed_time = std::chrono::high_resolution_clock::now() - first_time;
        const auto pct = (float)i / (float)class_size;

        // this is taking way too long
        if (pct < 50.0f && elapsed_time >= std::chrono::seconds(1)) {
            break;
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

bool ObjectExplorer::widget_with_context(void* address, const std::string& name, std::function<bool()> widget) {
    auto ret = widget();
    context_menu(address, name);

    return ret;
}

void ObjectExplorer::context_menu(void* address, std::optional<std::string> name, std::optional<std::function<void()>> additional_context) {
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::Selectable("Copy Address")) {
            std::stringstream ss;
            ss << std::hex << (uintptr_t)address;

            ImGui::SetClipboardText(ss.str().c_str());
        }

        if (name && ImGui::Selectable("Copy Name")) {
            ImGui::SetClipboardText(name->c_str());
        }

        const auto is_managed_object = utility::re_managed_object::is_managed_object(address);

        if (auto it = std::find_if(m_pinned_objects.begin(), m_pinned_objects.end(), [address](auto& pinned_obj) { return pinned_obj.address == address; }); it != m_pinned_objects.end()) {
            if (ImGui::Selectable("Unpin")) {
                m_pinned_objects.erase(it);
            }
        } else {
            if (ImGui::Selectable("Pin")) {
                auto& pinned = m_pinned_objects.emplace_back();

                const auto type_definition = is_managed_object ? utility::re_managed_object::get_type_definition((REManagedObject*)address) : nullptr;

                pinned.address = address;
                pinned.name = type_definition != nullptr ? type_definition->get_name() : std::to_string((uintptr_t)address);
                pinned.path = build_path();
            }
        }

        // Log component hierarchy to disk
        if (is_managed_object && utility::re_managed_object::is_a((REManagedObject*)address, "via.Component") && ImGui::Selectable("Log Hierarchy")) {
            auto comp = (REComponent*)address;

            for (auto obj = comp; obj; obj = obj->childComponent) {
                auto t = utility::re_managed_object::safe_get_type(obj);

                if (t != nullptr) {
                    if (obj->ownerGameObject == nullptr) {
                        spdlog::info("{:s} ({:x})", t->name, (uintptr_t)obj);
                    }
                    else {
                        auto owner = obj->ownerGameObject;
                        spdlog::info("[{:s}] {:s} ({:x})", utility::re_game_object::get_name(owner), t->name, (uintptr_t)obj);
                    }
                }

                if (obj->childComponent == comp) {
                    break;
                }
            }
        }

        if (is_managed_object && ImGui::Selectable("Hook All Methods")) {
            const auto t = utility::re_managed_object::get_type_definition((REManagedObject*)address);

            if (t != nullptr) {
                hook_all_methods(t);
            }
        }

        if (additional_context) {
            (*additional_context)();
        }

        ImGui::EndPopup();
    }
}

void ObjectExplorer::hook_method(sdk::REMethodDefinition* method, std::optional<std::string> name) {
    auto it = std::find_if(m_hooked_methods.begin(), m_hooked_methods.end(), [method](auto& hook) { return hook.method == method; });

    if (it != m_hooked_methods.end()) {
        return;
    }

    auto& hooked = m_hooked_methods.emplace_back();
    hooked.method = method;

    if (name) {
        hooked.name = method->get_declaring_type()->get_full_name() + "." + *name;
    } else {
        hooked.name = method->get_declaring_type()->get_full_name() + "." + method->get_name();
    }

    using namespace asmjit;
    using namespace asmjit::x86;

    // Pre
    {
        CodeHolder code{};
        code.init(m_jit_runtime.environment());
        Assembler a{&code};

        a.mov(r9, method);
        a.movabs(r10, &ObjectExplorer::pre_hooked_method);
        a.jmp(r10);

        m_jit_runtime.add(&hooked.jitted_function, &code);
    }

    // Post
    {
        CodeHolder code{};
        code.init(m_jit_runtime.environment());
        Assembler a{&code};

        a.mov(r9, method);
        a.movabs(r10, &ObjectExplorer::post_hooked_method);
        a.jmp(r10);

        m_jit_runtime.add(&hooked.jitted_function_post, &code);
    }
    
    using MT = HookManager::PreHookResult(*)(std::vector<uintptr_t>& args, std::vector<sdk::RETypeDefinition*>& arg_tys, uintptr_t ret_addr);
    using MTPost = void (*)(uintptr_t& ret_val, sdk::RETypeDefinition* ret_ty, uintptr_t ret_addr);

    hooked.hook_id = g_hookman.add(method, (MT)hooked.jitted_function, (MTPost)hooked.jitted_function_post);
}

void ObjectExplorer::hook_all_methods(sdk::RETypeDefinition* t) {
    if (t == nullptr) {
        return;
    }

    const auto methods = t->get_methods();

    for (auto& m : methods) {
        const auto method_ptr = m.get_function();
        if (method_ptr == nullptr) {
            continue;
        }

        bool is_stub = m_known_stub_methods.find(method_ptr) != m_known_stub_methods.end();
        bool is_ok_method = m_ok_methods.find(method_ptr) != m_ok_methods.end();

        if (method_ptr != nullptr && !is_stub && !is_ok_method) {
            if (utility::is_stub_code((uint8_t*)method_ptr)) {
                m_known_stub_methods.insert(method_ptr);

                is_ok_method = false;
                is_stub = true;
            } else {
                m_ok_methods.insert(method_ptr);
            }
        }

        bool is_duplicate = m_function_occurrences[method_ptr] > 5;

        if (!is_stub && !is_duplicate) {
            hook_method(&m, {});
        }
    }
}

void ObjectExplorer::method_context_menu(sdk::REMethodDefinition* method, std::optional<std::string> name, ::REManagedObject* obj) {
    auto additional_ctx = [&]() {
        auto it = std::find_if(m_hooked_methods.begin(), m_hooked_methods.end(), [method](auto& hook) { return hook.method == method; });

        if (it == m_hooked_methods.end()) {
            if (ImGui::Selectable("Hook")) {
                std::scoped_lock _{m_job_mutex};

                m_frame_jobs.push_back([this, method, name]() {
                    hook_method(method, name);
                });
            }
        } else {
            if (ImGui::Selectable("Unhook")) {
                std::scoped_lock _{m_job_mutex};

                m_frame_jobs.push_back([this, it]() {
                    g_hookman.remove(it->method, it->hook_id);
                    m_hooked_methods.erase(it);
                });
            }
        }

        if (ImGui::Selectable("Hook All Methods")) {
            const auto declaring_type = method->get_declaring_type();

            if (declaring_type != nullptr) {
                std::scoped_lock _{m_job_mutex};

                m_frame_jobs.push_back([this, declaring_type]() {
                    hook_all_methods(declaring_type);
                });
            }
        }

        // Allow us to call really simple methods with no params
        if (method->get_param_types().size() == 0) {
            if (obj != nullptr || method->is_static()) {
                if (ImGui::Selectable("Call")) {
                    method->invoke(obj, {});
                }
            }
        }
    };

    context_menu(method->get_function(), name, additional_ctx);
}

void ObjectExplorer::make_same_line_text(std::string_view text, const ImVec4& color) {
    if (text.empty()) {
        return;
    }

    ImGui::SameLine();
    ImGui::TextColored(color, "%s", text.data());
}

void ObjectExplorer::make_tree_offset(REManagedObject* object, uint32_t offset, std::string_view name, std::function<void()> widget) {
    auto ptr = Address(object).get(offset).to<void*>();

    if (ptr == nullptr) {
        return;
    }

    auto made_node = ImGui::TreeNode((uint8_t*)object + offset, "0x%X: %s", offset, name.data());

    context_menu(ptr);

    if (widget != nullptr) {
        widget();
    }

    if (made_node) {
        handle_address(ptr);
        ImGui::TreePop();
    }
}

bool ObjectExplorer::is_managed_object(Address address) const {
    return utility::re_managed_object::is_managed_object(address);
}

void ObjectExplorer::populate_classes() {
    auto type_list = reframework::get_types()->get_raw_types();
    spdlog::info("TypeList: {:x}", (uintptr_t)type_list);

    std::unordered_set<REType*> seen{};

    if (type_list != nullptr) try {
        // I don't know why but it can extend past the size.
        for (auto i = 0; i < type_list->numAllocated; ++i) {
            auto t = (*type_list->data)[i];

            if (t == nullptr || IsBadReadPtr(t, sizeof(REType))) {
                continue;
            }

            if (t->name == nullptr) {
                continue;
            }

            if (seen.contains(t)) {
                continue;
            }

            auto name = std::string{ t->name };

            if (name.empty()) {
                continue;
            }

            spdlog::info("{:s}", name);
            m_sorted_types.push_back(name);
            m_types[name] = t;

            seen.insert(t);
        }

        std::sort(m_sorted_types.begin(), m_sorted_types.end());
        return;
    } catch(...) {
        spdlog::error("Unknown exception caught while populating classes, falling back to other method.");
    }

    auto tdb = sdk::RETypeDB::get();

    if (tdb != nullptr) {
        for (auto i = 0; i < tdb->get_num_types(); ++i) {
            const auto t = tdb->get_type(i);

            if (t == nullptr) {
                continue;
            }

            const auto re_type = t->get_type();

            if (re_type == nullptr) {
                continue;
            }

            if (seen.contains(re_type)) {
                continue;
            }

            const auto name = re_type->name;

            if (name == nullptr) {
                continue;
            }

            spdlog::info("{:s}", name);
            m_sorted_types.push_back(name);
            m_types[name] = re_type;

            seen.insert(re_type);
        }
    }
    
    std::sort(m_sorted_types.begin(), m_sorted_types.end());
}

void ObjectExplorer::populate_enums() {
    bool has_enums = false;

#if TDB_VER > 49
    try {
        auto ref = utility::scan(g_framework->get_module().as<HMODULE>(), "66 C7 40 18 01 01 48 89 05 ? ? ? ?");

        if (ref) {
            std::ofstream out_file(REFramework::get_persistent_dir("Enums_Internal.hpp"));

            auto& l = *(std::map<uint64_t, REEnumData>*)(utility::calculate_absolute(*ref + 9));
            spdlog::info("EnumList: {:x}", (uintptr_t)&l);
            spdlog::info("Size: {}", l.size());

            has_enums = l.size() > 0;

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
        } else {
            spdlog::error("Failed to find EnumList");
        }
    } catch(...) {
        has_enums = false;
        m_enums.clear();
        spdlog::error("Unknown exception caught while populating enums, falling back to other method.");
    }
#endif

    if (!has_enums) {
        std::ofstream out_file(REFramework::get_persistent_dir("Enums_Internal.hpp"));

        std::vector<sdk::RETypeDefinition*> enum_types{};
        auto tdb = sdk::RETypeDB::get();

        const auto flags_attribute = tdb->find_type("System.FlagsAttribute");
        const auto flags_attribute_runtime_type = flags_attribute != nullptr ? flags_attribute->get_runtime_type() : nullptr;

        for (uint32_t i = 0; i < tdb->numTypes; ++i) {
            auto t = tdb->get_type(i);
            auto parent_t = t->get_parent_type();

            if (parent_t == nullptr) {
                continue;
            }

            if (parent_t->get_full_name() == "System.Enum") {
                auto full_name = t->get_full_name();
                if (full_name.empty()) {
                    continue;
                }

                // Remove template arguments and other garbage.
                if (auto pos = full_name.find_first_of("`"); pos != std::string::npos) {
                    full_name = full_name.substr(0, pos);
                }
                
                auto nspace = full_name.substr(0, full_name.find_last_of("."));
                const auto name = t->get_name();

                for (auto pos = nspace.find("."); pos != std::string::npos; pos = nspace.find(".")) {
                    nspace.replace(pos, 1, "::");
                }

                out_file << "namespace " << nspace << " {" << std::endl;
                
                if (flags_attribute_runtime_type != nullptr && t->has_attribute(flags_attribute_runtime_type, true)) {
                    out_file << "    // [Flags]" << std::endl;
                }

                out_file << "    enum " << name << " {" << std::endl;

                spdlog::info("ENUM {}", t->get_full_name().c_str());
                enum_types.push_back(t);
                auto fields = t->get_fields();

                for (auto f : fields) {
                    const auto field_flags = f->get_flags();

                    if ((field_flags & (uint16_t)via::clr::FieldFlag::Static) != 0 && (field_flags & (uint16_t)via::clr::FieldFlag::Literal) != 0) {
                        auto raw_data = f->get_data_raw(nullptr, true);
                        int64_t enum_data = 0;

                        switch(t->get_valuetype_size()) {
                            case 1:
                                enum_data = (int64_t)*(int8_t*)raw_data;
                                break;
                            case 2:
                                enum_data = (int64_t)*(int16_t*)raw_data;
                                break;
                            case 4:
                                enum_data = (int64_t)*(int32_t*)raw_data;
                                break;
                            case 8:
                                enum_data = *(int64_t*)raw_data;
                                break;
                            default:
                                spdlog::error("Unknown enum size: {}", t->get_valuetype_size());
                                break;
                        }

                        m_enums.emplace(t->get_full_name(), EnumDescriptor{ f->get_name(), enum_data });

                        // Log
                        spdlog::info(" {} = {}", f->get_name(), enum_data);

                        // Write to file
                        out_file << "        " << f->get_name() << " = " << enum_data << "," << std::endl;
                    }
                }

                out_file << "    };" << std::endl;
                out_file << "}" << std::endl;
            }
        }
    }
}

void ObjectExplorer::init() {
    populate_classes();
    populate_enums();

    if (m_function_occurrences.empty()) {
        const auto tdb = sdk::RETypeDB::get();

        for (auto i = 0; i < tdb->get_num_methods(); ++i) {
            const auto method = tdb->get_method(i);

            if (method == nullptr) {
                continue;
            }

            const auto func = method->get_function();

            if (func == nullptr) {
                continue;
            }

            if (!m_function_occurrences.contains(func)) {
                m_method_map[(uintptr_t)func] = method;
            }

            m_function_occurrences[func]++;
        }
    }
}

std::string ObjectExplorer::get_full_enum_value_name(std::string_view enum_name, int64_t value) {
    std::string out{};

    // Check which bits are set and have enum names
    for (auto i = 0; i < 64; ++i) {
        if (auto bit = (value & ((int64_t)1 << i)); bit != 0) {
            auto value_name = get_enum_value_name(enum_name, bit);

            if (value_name.empty()) {
                continue;
            }

            if (!out.empty()) {
                out += " | ";
            }

            out += value_name;
        }
    }

    return out;
}

std::string ObjectExplorer::get_enum_value_name(std::string_view enum_name, int64_t value) {
    std::lock_guard l{m_enum_mutex};

    if (!m_enums.contains(enum_name.data())) {
        spdlog::info("Unknown enum: {}", enum_name);
        return "";
    }

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

uintptr_t ObjectExplorer::get_original_va(void* ptr) {
    if (ptr == nullptr) {
        return 0;
    }

    // Read a small chunk of the executable from disk so we can get the original virtual address for the imagebase
    if (m_module_chunk.empty()) {
        m_module_chunk.resize(5000000);
        std::ifstream{*utility::get_module_path(g_framework->get_module().as<HMODULE>()), std::ifstream::binary}
            .read((char*)m_module_chunk.data(), m_module_chunk.size());
    }

    return *utility::get_imagebase_va_from_ptr(m_module_chunk.data(), g_framework->get_module(), ptr);
}

bool ObjectExplorer::is_filtered_type(std::string name) {
    auto it = m_types.find(name);

    if (it == m_types.end() || it->second == nullptr) {
        return false;
    }

    auto t = it->second;
    auto tdef = utility::re_type::get_type_definition(t);
    if (tdef == nullptr) {
        return false;
    }

    if (!std::string_view{m_type_member.data()}.empty()) {
        for (auto& m : tdef->get_methods()) {
            if (is_filtered_method(m)) {
                return true;
            }
        }
    }

    if (!std::string_view{m_type_field.data()}.empty()) {
        for (auto f : tdef->get_fields()) {
            if (f != nullptr && is_filtered_field(*f)) {
                return true;
            }
        }
    }

    return false;
}

bool ObjectExplorer::is_filtered_method(sdk::REMethodDefinition& m) try {
    const auto method_name = m.get_name();
    const auto name = std::string_view{m_type_member.data()};
    if (name.empty()) {
        return true;
    }

    if (!m_search_using_regex) {
        if (std::string_view{method_name}.find(name) != std::string_view::npos) {
            return true;
        }
    } else {
        if (std::regex_search(method_name, std::regex{name.data()})) {
            return true;
        }
    }

    const auto method_return_type = m.get_return_type();
    const std::string method_return_type_name = method_return_type != nullptr ? method_return_type->get_full_name() : "";

    if (!m_search_using_regex) {
        if (method_return_type_name.find(name) != std::string::npos) {
            return true;
        }
    } else {
        if (std::regex_search(method_return_type_name, std::regex{name.data()})) {
            return true;
        }
    }

    std::function<bool(std::string_view a)> search_algo_params{};

    if (!m_search_using_regex) {
        search_algo_params = [name](std::string_view a) { return a.find(name) != std::string_view::npos; };
    } else {
        search_algo_params = [name](std::string_view a) { return std::regex_search(a.data(), std::regex{name.data()}); };
    }

    const auto method_param_names = m.get_param_names();
    if (auto i = std::find_if(method_param_names.begin(), method_param_names.end(), search_algo_params);
        i != method_param_names.end()) {
        return true;
    }

    std::function<bool(sdk::RETypeDefinition* a)> search_algo_types{};

    if (!m_search_using_regex) {
        search_algo_types = [name](sdk::RETypeDefinition* a) { return std::string_view{a->get_name()}.find(name) != std::string_view::npos; };
    } else {
        search_algo_types = [name](sdk::RETypeDefinition* a) { return std::regex_search(a->get_name(), std::regex{name.data()}); };
    }

    const auto method_param_types = m.get_param_types();
    if (auto i = std::find_if(method_param_types.begin(), method_param_types.end(), search_algo_types);
        i != method_param_types.end()) {
        return true;
    }

    return false;
} catch (...) {
    return false;
}

bool ObjectExplorer::is_filtered_field(sdk::REField& f) try {
    const auto field_name = f.get_name();
    const auto name = std::string_view{m_type_field.data()};
    if (name.empty()) {
        return true;
    }

    if (!m_search_using_regex) {
        if (std::string_view{field_name}.find(name) != std::string_view::npos) {
            return true;
        }
    } else {
        if (std::regex_search(field_name, std::regex{name.data()})) {
            return true;
        }
    }

    const auto field_type = f.get_type();
    const std::string field_type_name = field_type != nullptr ? field_type->get_full_name() : "";

    if (!m_search_using_regex) {
        if (field_type_name.find(name) != std::string::npos) {
            return true;
        }
    } else {
        if (std::regex_search(field_type_name, std::regex{name.data()})) {
            return true;
        }
    }

    return false;
} catch (...) {
    return false;
}

HookManager::PreHookResult ObjectExplorer::pre_hooked_method_internal(std::vector<uintptr_t>& args, std::vector<sdk::RETypeDefinition*>& arg_tys, uintptr_t ret_addr, sdk::REMethodDefinition* method) {
    auto it = std::find_if(m_hooked_methods.begin(), m_hooked_methods.end(), [method](auto& a) { return a.method == method; });

    if (it == m_hooked_methods.end()) {
        return HookManager::PreHookResult::CALL_ORIGINAL;
    }

    auto& hooked_method = *it;

    std::scoped_lock _{m_hooks_context.mtx};
    ++hooked_method.stats.call_count;

    if (!hooked_method.return_addresses.contains(ret_addr)) {
        spdlog::info("Creating new entry for {}", hooked_method.name);

        hooked_method.return_addresses.insert(ret_addr);
        // Try to locate the containing function from the return address
        sdk::REMethodDefinition* nearest_method{nullptr};
        size_t nearest_distance = UINT64_MAX;

        for (auto& it : m_method_map) {
            if (it.first > ret_addr) {
                continue;
            }

            const auto distance = ret_addr - it.first;
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest_method = it.second;
            }
        }

        if (nearest_method != nullptr) {
            auto method_entry = utility::find_function_entry((uintptr_t)nearest_method->get_function());

            bool added = false;
            auto add_method = [&]() {
                hooked_method.return_addresses_to_methods[ret_addr] = nearest_method;
                hooked_method.callers.insert(nearest_method);

                const auto decl_type = nearest_method->get_declaring_type();

                if (decl_type != nullptr) {
                    spdlog::info("{} {}.{}", hooked_method.name, nearest_method->get_declaring_type()->get_full_name(), nearest_method->get_name());
                } else {
                    spdlog::info("{} {}", hooked_method.name, nearest_method->get_name());
                }

                added = true;
            };

            if (method_entry != nullptr) {
                const auto module_addr = (uintptr_t)utility::get_module_within(ret_addr).value_or(nullptr);
                const auto ret_addr_rva = (uint32_t)(ret_addr - module_addr);
                const auto ret_addr_entry = utility::find_function_entry(ret_addr);

                // First condition isn't as heavy as fully disassembling the function which is the second condition
                if (ret_addr_entry == method_entry ||
                    (ret_addr_rva >= method_entry->BeginAddress && ret_addr_rva <= method_entry->EndAddress) ||
                    (ret_addr_entry != nullptr && ret_addr_entry->BeginAddress >= method_entry->BeginAddress && ret_addr_entry->BeginAddress <= method_entry->EndAddress + 1))
                {
                    add_method();
                } else {
                    // Disassemble all possible code paths to see if we run into the return address
                    utility::exhaustive_decode((uint8_t*)nearest_method->get_function(), 5000, [&](utility::ExhaustionContext& ctx) -> utility::ExhaustionResult {
                        if (ctx.addr == ret_addr) {
                            add_method();
                            return utility::ExhaustionResult::BREAK;
                        }

                        if (std::string_view{ctx.instrux.Mnemonic}.starts_with("CALL")) {
                            return utility::ExhaustionResult::STEP_OVER;
                        }

                        return utility::ExhaustionResult::CONTINUE;
                    });

                    if (!added) {
                        spdlog::info("{} <unknown caller> @ 0x{:x}", hooked_method.name, ret_addr);
                    }
                }
            } else {
                hooked_method.return_addresses_to_methods[ret_addr] = nearest_method;
                hooked_method.callers.insert(nearest_method);
                const auto decl_type = nearest_method->get_declaring_type();

                if (decl_type != nullptr) {
                    spdlog::info("{} {}.{}", hooked_method.name, nearest_method->get_declaring_type()->get_full_name(), nearest_method->get_name());
                } else {
                    spdlog::info("{} {}", hooked_method.name, nearest_method->get_name());
                }
            }
        } else {
            spdlog::info("{} <unknown caller> @ 0x{:x}", hooked_method.name, ret_addr);
        }
    }

    if (auto it2 = hooked_method.return_addresses_to_methods.find(ret_addr); it2 != hooked_method.return_addresses_to_methods.end()) {
        auto caller = it2->second;
        auto& context = hooked_method.stats.callers_context[caller];
        ++context.call_count;
    }

    const auto tid = std::this_thread::get_id();
    const auto now = std::chrono::high_resolution_clock::now();
    hooked_method.stats.last_call_time = now;
    hooked_method.stats.last_call_times[tid] = now;
    hooked_method.stats.thread_ids.insert(tid._Get_underlying_id());

    auto result = HookManager::PreHookResult::CALL_ORIGINAL;

    if (hooked_method.skip) {
        result = HookManager::PreHookResult::SKIP_ORIGINAL;
    }

    return result;
}

HookManager::PreHookResult ObjectExplorer::pre_hooked_method(std::vector<uintptr_t>& args, std::vector<sdk::RETypeDefinition*>& arg_tys, uintptr_t ret_addr, sdk::REMethodDefinition* method) {
    return ObjectExplorer::get()->pre_hooked_method_internal(args, arg_tys, ret_addr, method);
}

void ObjectExplorer::post_hooked_method_internal(uintptr_t& ret_val, sdk::RETypeDefinition*& ret_ty, uintptr_t ret_addr, sdk::REMethodDefinition* method) {
    auto it = std::find_if(m_hooked_methods.begin(), m_hooked_methods.end(), [method](auto& a) { return a.method == method; });

    if (it == m_hooked_methods.end()) {
        return;
    }

    const auto now = std::chrono::high_resolution_clock::now();

    auto& hooked_method = *it;

    std::scoped_lock _{m_hooks_context.mtx};

    // Reset the last call time if this is the first time we're calling it
    // because in between this, there will be a large hitch
    // that the game is not causing.
    if (hooked_method.stats.call_count <= 1) {
        hooked_method.stats.last_call_time = now;
    }

    const auto tid = std::this_thread::get_id();

    hooked_method.stats.last_call_end_time = now;
    const auto delta = now - hooked_method.stats.last_call_times[tid];
    hooked_method.stats.last_call_delta = delta;
    hooked_method.stats.total_call_time += delta;
}

void ObjectExplorer::post_hooked_method(uintptr_t& ret_val, sdk::RETypeDefinition*& ret_ty, uintptr_t ret_addr, sdk::REMethodDefinition* method) {
    ObjectExplorer::get()->post_hooked_method_internal(ret_val, ret_ty, ret_addr, method);
}
