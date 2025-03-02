#pragma once

#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <string>
#include <imgui.h>
#include <json.hpp>
#include <asmjit/asmjit.h>
#include <asmjit/x86/x86assembler.h>

#include "utility/Address.hpp"
#include "Tool.hpp"
#include "HookManager.hpp"

#include <sdk/TDBVer.hpp>

#define TDB_DUMP_ALLOWED

namespace sdk {
struct RETypeDefinition;

namespace renderer {
class RenderLayer;
}

namespace behaviortree {
class BehaviorTree;
class CoreHandle;
class TreeNode;
}
}

namespace genny {
class Class;
}

#ifdef TDB_DUMP_ALLOWED
namespace detail {
struct ParsedParams;
struct ParsedMethod;
struct ParsedField;
struct ParsedProperty;
struct ParsedType;

struct ParsedParams {
    std::shared_ptr<ParsedMethod> owner{};
    std::shared_ptr<ParsedType> type{};
    const char* name;

    bool by_ref : 1;
    bool by_ptr : 1;
};

struct ParsedMethod {
    std::shared_ptr<ParsedType> owner{};
    sdk::REMethodDefinition* m{};
#if TDB_VER >= 69
    sdk::REMethodImpl* m_impl{};
#endif
    const char* name{};

    std::vector<std::shared_ptr<ParsedParams>> params{};
    std::shared_ptr<ParsedParams> return_val{};
};

struct ParsedField {
    std::shared_ptr<ParsedType> owner{};
    std::shared_ptr<ParsedType> type{};
    sdk::REField* f{};
#if TDB_VER >= 69
    sdk::REFieldImpl* f_impl{};
#endif
    const char* name{};
    uint32_t offset_from_fieldptr{};
    uint32_t offset_from_base{};
};

struct ParsedProperty {
    std::shared_ptr<ParsedType> owner{};
    std::shared_ptr<ParsedMethod> getter{};
    std::shared_ptr<ParsedMethod> setter{};

    sdk::REProperty* p{};

#if TDB_VER >= 69
    sdk::REPropertyImpl* p_impl{};
#endif
    const char* name{};
};

struct ParsedType {
    std::shared_ptr<ParsedType> owner{}; // declaring class
    std::shared_ptr<ParsedType> super{}; // parent class we inherit from

    sdk::RETypeDefinition* t{nullptr};
    genny::Class* genny_t{nullptr};

    const char* name_space{"Unknown"};
    const char* name{"Unknown"};
    std::string full_name{};

    std::vector<sdk::REMethodDefinition*> methods{};
    std::vector<sdk::REField*> fields{};
    std::vector<sdk::REProperty*> props{};

#if TDB_VER >= 69
    std::vector<sdk::REMethodImpl*> method_impls{};
    std::vector<sdk::REPropertyImpl> prop_impls{};
    std::vector<sdk::REFieldImpl*> field_impls{};
#endif

    std::vector<std::shared_ptr<ParsedField>> parsed_fields;
    std::vector<std::shared_ptr<ParsedMethod>> parsed_methods;
    std::vector<std::shared_ptr<ParsedProperty>> parsed_props;
};
} // namespace detail
#endif

class ObjectExplorer : public Tool {
public:
    static std::shared_ptr<ObjectExplorer>& get();

public:
    ObjectExplorer();

    std::string_view get_name() const override { return "ObjectExplorer"; };

    void on_draw_dev_ui() override;
    void on_frame() override;
    void on_lua_state_created(sol::state& lua) override;

    void handle_address(Address address, int32_t offset = -1, Address parent = nullptr, Address real_address = nullptr);
    
private:
    void display_pins();
    void display_hooks();

#ifdef TDB_DUMP_ALLOWED
    std::shared_ptr<detail::ParsedType> init_type_min(nlohmann::json& il2cpp_dump, sdk::RETypeDB* tdb, uint32_t i);
    std::shared_ptr<detail::ParsedType> init_type(nlohmann::json& il2cpp_dump, sdk::RETypeDB* tdb, uint32_t i);
    std::string generate_full_name(sdk::RETypeDB* tdb, uint32_t i);
    void export_deserializer_chain(nlohmann::json& il2cpp_dump, sdk::RETypeDB* tdb, REType* t, std::optional<std::string> real_name = std::nullopt);
#endif
    void generate_sdk(bool skip_sdkgenny);
    void report_sdk_dump_progress(float progress);

    void handle_game_object(REGameObject* game_object);
    void handle_component(REComponent* component);
    void handle_transform(RETransform* transform);
    void handle_render_layer(sdk::renderer::RenderLayer* layer);
    void handle_behavior_tree(sdk::behaviortree::BehaviorTree* bhvt);
    void handle_behavior_tree_core_handle(sdk::behaviortree::BehaviorTree* bhvt, sdk::behaviortree::CoreHandle* bhvt_core_handle, uint32_t tree_idx);
    void handle_behavior_tree_node(sdk::behaviortree::BehaviorTree* bhvt, sdk::behaviortree::TreeNode* node, uint32_t tree_idx);
    void handle_type(REManagedObject* obj, REType* t);

    void display_enum_value(std::string_view name, int64_t value);
    void display_reflection_methods(REManagedObject* obj, REType* type_info);
    void display_reflection_properties(REManagedObject* obj, REType* type_info);
    void display_native_methods(REManagedObject* obj, sdk::RETypeDefinition* tdef);
    void display_native_fields(REManagedObject* obj, sdk::RETypeDefinition* tdef);
    void populate_method_meta_info(sdk::REMethodDefinition& m);
    void attempt_display_method(REManagedObject* obj, sdk::REMethodDefinition& m, bool use_full_name = false);
    void attempt_display_field(REManagedObject* obj, VariableDescriptor* desc, REType* type_info);
    void display_data(void* data, void* real_data, std::string type_name, bool is_enum = false, bool managed_str = false, const sdk::RETypeDefinition* override_def = nullptr);
    int32_t get_field_offset(REManagedObject* obj, VariableDescriptor* desc, REType* type_info);

    bool widget_with_context(void* address, std::function<bool()> widget);
    bool widget_with_context(void* address, const std::string& name, std::function<bool()> widget);
    void context_menu(void* address, std::optional<std::string> name = std::nullopt, std::optional<std::function<void()>> additional_context = std::nullopt);
    void hook_method(sdk::REMethodDefinition* method, std::optional<std::string> name);
    void hook_all_methods(sdk::RETypeDefinition* type);
    void method_context_menu(sdk::REMethodDefinition* method, std::optional<std::string> name, ::REManagedObject* obj = nullptr);
    void make_same_line_text(std::string_view text, const ImVec4& color);

    void make_tree_offset(REManagedObject* object, uint32_t offset, std::string_view name, std::function<void()> widget = nullptr);
    bool is_managed_object(Address address) const;

    void populate_classes();
    void populate_enums();
    void init();

    std::string get_full_enum_value_name(std::string_view enum_name, int64_t value);
    std::string get_enum_value_name(std::string_view enum_name, int64_t value);
    REType* get_type(std::string_view type_name);

    uintptr_t get_original_va(void* ptr);

    bool is_filtered_type(std::string name);
    bool is_filtered_method(sdk::REMethodDefinition& m);
    bool is_filtered_field(sdk::REField& f);

    template <typename T, typename... Args>
    bool stretched_tree_node(T id, Args... args) {
        auto& style = ImGui::GetStyle();
        auto style_bak = ImGui::GetStyle();

        style.ItemSpacing.x *= 100;
        bool made_node = ImGui::TreeNode(id, args...);
        style.ItemSpacing.x = style_bak.ItemSpacing.x;

        return made_node;
    }

    std::string build_path() const {
        std::string path{};

        for (auto& p : m_current_path) {
            if (!path.empty()) {
                path += ".";
            }

            path += p;
        }
        
        return path;
    }

    HookManager::PreHookResult pre_hooked_method_internal(std::vector<uintptr_t>& args, std::vector<sdk::RETypeDefinition*>& arg_tys, uintptr_t ret_addr, sdk::REMethodDefinition* method);
    static HookManager::PreHookResult pre_hooked_method(std::vector<uintptr_t>& args, std::vector<sdk::RETypeDefinition*>& arg_tys, uintptr_t ret_addr, sdk::REMethodDefinition* method);

    void post_hooked_method_internal(uintptr_t& ret_val, sdk::RETypeDefinition*& ret_ty, uintptr_t ret_addr, sdk::REMethodDefinition* method);
    static void post_hooked_method(uintptr_t& ret_val, sdk::RETypeDefinition*& ret_ty, uintptr_t ret_addr, sdk::REMethodDefinition* method);

    struct PinnedObject {
        Address address{};
        std::string name{};
        std::string path{};
    };

    struct HooksContext {
        enum class SortMethod : uint8_t {
            NONE,
            CALL_COUNT,
            CALL_TIME_LAST,
            CALL_TIME_DELTA,
            CALL_TIME_TOTAL,
            METHOD_NAME,
            NUMBER_OF_CALLERS,
            NUMBER_OF_THREADS_CALLED_FROM
        };

        static inline constexpr std::array<const char*, 8> s_sort_method_names {
            "None",
            "Call Count",
            "Call Time (Last)",
            "Call Time (Delta)",
            "Call Time (Total)",
            "Method Name",
            "Number of Callers",
            "Number of Threads Called From"
        };

        std::recursive_mutex mtx{};
        bool hide_uncalled_methods{false};
        SortMethod sort_method{SortMethod::NONE};
    } m_hooks_context{};

    std::recursive_mutex m_job_mutex{};
    std::vector<std::function<void()>> m_frame_jobs{};

    struct HookedMethod {
        enum class SortCallersMethod : uint8_t {
            NONE,
            CALL_COUNT,
            METHOD_NAME
        };
    
        static inline constexpr std::array<const char*, 3> s_sort_callers_names {
            "None",
            "Call Count",
            "Method Name"
        };

        std::string name{};
        sdk::REMethodDefinition* method{nullptr};
        uintptr_t jitted_function{};
        uintptr_t jitted_function_post{};
        bool skip{false};
        size_t hook_id{};
        SortCallersMethod sort_callers_method{SortCallersMethod::NONE};

        // Not considered stats because resetting these causes large fluctuations in the stats
        std::unordered_set<uintptr_t> return_addresses{};
        std::unordered_set<sdk::REMethodDefinition*> callers{};
        std::unordered_map<uintptr_t, sdk::REMethodDefinition*> return_addresses_to_methods{};
        std::unique_ptr<std::recursive_mutex> mtx{std::make_unique<std::recursive_mutex>()};

        struct Stats {
            uint32_t call_count{};
            std::unordered_set<uint32_t> thread_ids{};
            struct CallerContext {
                size_t call_count{};
            };


            using tp = std::chrono::high_resolution_clock::time_point;
            std::unordered_map<sdk::REMethodDefinition*, CallerContext> callers_context{};
            std::chrono::high_resolution_clock::time_point last_call_time{std::chrono::high_resolution_clock::now()};
            std::unordered_map<std::thread::id, tp> last_call_times{};
            std::chrono::high_resolution_clock::time_point last_call_end_time{std::chrono::high_resolution_clock::now()}; // assuming not recursive...
            std::chrono::high_resolution_clock::duration last_call_delta{};
            std::chrono::high_resolution_clock::duration total_call_time{};
        } stats;

        void reset_stats() {
            stats = Stats{};
        }
    };

    // Contains extra information about the method
    struct MethodMetaInfo {
        std::vector<sdk::REMethodDefinition*> called_functions{};
    };

    // Function address -> meta info (not REMethodDefinition, since method defs can share function addresses)
    std::unordered_map<uintptr_t, std::unique_ptr<MethodMetaInfo>> m_method_meta_infos{};

    asmjit::JitRuntime m_jit_runtime;

    std::vector<PinnedObject> m_pinned_objects{};
    std::vector<HookedMethod> m_hooked_methods{};
    std::deque<std::string> m_current_path{};

    inline static const ImVec4 VARIABLE_COLOR{ 100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 255 / 255.0f };
    inline static const ImVec4 VARIABLE_COLOR_HIGHLIGHT{ 1.0f, 1.0f, 1.0f, 1.0f };

    bool m_search_using_regex{false};
    std::string m_type_name{"via.typeinfo.TypeInfo"};
    std::string m_type_member{""};
    std::string m_type_field{""};
    std::string m_method_address{ "0" };
    std::string m_object_address{ "0" };
    std::string m_add_component_name{ "via.Component" };
    std::chrono::system_clock::time_point m_next_refresh;
    std::chrono::system_clock::time_point m_next_refresh_natives{};

    std::unordered_map<VariableDescriptor*, int32_t> m_offset_map;

    struct EnumDescriptor {
        std::string name;
        int64_t value;
    };

    std::unordered_set<void*> m_known_stub_methods{};
    std::unordered_set<void*> m_ok_methods{};
    std::unordered_map<void*, uint32_t> m_function_occurrences{}; // occurrences of re-uses of the function address in other methods
    std::unordered_map<uintptr_t, sdk::REMethodDefinition*> m_method_map{};
    std::unordered_multimap<std::string, EnumDescriptor> m_enums;
    std::unordered_map<std::string, REType*> m_types;
    std::vector<std::string> m_sorted_types;

    std::mutex m_enum_mutex;

    // Types currently being displayed
    std::vector<REType*> m_displayed_types;
    sdk::REMethodDefinition* m_displayed_method{nullptr};

    std::vector<uint8_t> m_module_chunk{};

    bool m_do_init{ true };

    enum class SdkDumpStage {
        NONE = -1,
        DUMP_INITIALIZATION,
        DUMP_TYPES,
        DUMP_RSZ,
        DUMP_METHODS,
        DUMP_FIELDS,
        DUMP_PROPERTIES,
        DUMP_RSZ_2,
        DUMP_DESERIALIZER_CHAIN,
        DUMP_NON_TDB_TYPES,
        GENERATE_SDK
    };

    std::atomic<bool> m_dumping_sdk{ false };
    std::atomic<float> m_sdk_dump_progress{ 0.0f };
    std::atomic<SdkDumpStage> m_sdk_dump_stage{ SdkDumpStage::NONE };
};

