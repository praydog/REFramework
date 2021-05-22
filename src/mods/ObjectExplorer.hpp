#pragma once

#include <unordered_set>
#include <imgui/imgui.h>
#include <nlohmann/json.hpp>

#include "utility/Address.hpp"
#include "Mod.hpp"

#ifdef RE8
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
    REMethodDefinition* m{};
    REMethodImpl* m_impl{};
    const char* name{};

    std::vector<std::shared_ptr<ParsedParams>> params{};
    std::shared_ptr<ParsedParams> return_val{};
};

struct ParsedField {
    std::shared_ptr<ParsedType> owner{};
    std::shared_ptr<ParsedType> type{};
    REField* f{};
    REFieldImpl* f_impl{};
    const char* name{};
    uint32_t offset_from_fieldptr{};
    uint32_t offset_from_base{};
};

struct ParsedProperty {
    std::shared_ptr<ParsedType> owner{};
    std::shared_ptr<ParsedMethod> getter{};
    std::shared_ptr<ParsedMethod> setter{};

    REProperty* p{};
    REPropertyImpl* p_impl{};
    const char* name{};
};

struct ParsedType {
    std::shared_ptr<ParsedType> owner{}; // declaring class
    std::shared_ptr<ParsedType> super{}; // parent class we inherit from

    REClassInfo* t{nullptr};

    const char* name_space{"Unknown"};
    const char* name{"Unknown"};
    std::string full_name{};

    std::vector<REMethodDefinition*> methods{};
    std::vector<REMethodImpl*> method_impls{};
    std::vector<REField*> fields{};
    std::vector<REFieldImpl*> field_impls{};
    std::vector<REProperty*> props{};
    std::vector<REPropertyImpl> prop_impls{};

    std::vector<std::shared_ptr<ParsedField>> parsed_fields;
    std::vector<std::shared_ptr<ParsedMethod>> parsed_methods;
    std::vector<std::shared_ptr<ParsedProperty>> parsed_props;
};
} // namespace detail
#endif

class ObjectExplorer : public Mod {
public:
    ObjectExplorer();

    std::string_view get_name() const override { return "ObjectExplorer"; };

    void on_draw_ui() override;

private:
#ifdef RE8
    std::shared_ptr<detail::ParsedType> init_type_min(nlohmann::json& il2cpp_dump, RETypeDB* tdb, uint32_t i);
    std::shared_ptr<detail::ParsedType> init_type(nlohmann::json& il2cpp_dump, RETypeDB* tdb, uint32_t i);
    std::string generate_full_name(RETypeDB* tdb, uint32_t i);
#endif
    void generate_sdk();

    void handle_address(Address address, int32_t offset = -1, Address parent = nullptr, Address real_address = nullptr);
    void handle_game_object(REGameObject* game_object);
    void handle_component(REComponent* component);
    void handle_transform(RETransform* transform);
    void handle_type(REManagedObject* obj, REType* t);

    void display_enum_value(std::string_view name, int64_t value);
    void display_methods(REManagedObject* obj, REType* type_info);
    void display_fields(REManagedObject* obj, REType* type_info);
    void attempt_display_field(REManagedObject* obj, VariableDescriptor* desc, REType* type_info);
    int32_t get_field_offset(REManagedObject* obj, VariableDescriptor* desc, REType* type_info);

    bool widget_with_context(void* address, std::function<bool()> widget);
    void context_menu(void* address);
    void make_same_line_text(std::string_view text, const ImVec4& color);

    void make_tree_offset(REManagedObject* object, uint32_t offset, std::string_view name);
    bool is_managed_object(Address address) const;

    void populate_classes();
    void populate_enums();

    std::string get_full_enum_value_name(std::string_view enum_name, int64_t value);
    std::string get_enum_value_name(std::string_view enum_name, int64_t value);
    REType* get_type(std::string_view type_name);

    template <typename T, typename... Args>
    bool stretched_tree_node(T id, Args... args) {
        auto& style = ImGui::GetStyle();
        auto style_bak = ImGui::GetStyle();

        style.ItemSpacing.x *= 100;
        bool made_node = ImGui::TreeNode(id, args...);
        style.ItemSpacing.x = style_bak.ItemSpacing.x;

        return made_node;
    }

    inline static const ImVec4 VARIABLE_COLOR{ 100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 255 / 255.0f };
    inline static const ImVec4 VARIABLE_COLOR_HIGHLIGHT{ 1.0f, 1.0f, 1.0f, 1.0f };

    std::string m_type_name{ "via.typeinfo.TypeInfo" };
    std::string m_object_address{ "0" };
    std::chrono::system_clock::time_point m_next_refresh;
    std::chrono::system_clock::time_point m_next_refresh_natives{};

    std::unordered_map<VariableDescriptor*, int32_t> m_offset_map;

    struct EnumDescriptor {
        std::string name;
        int64_t value;
    };

    std::unordered_multimap<std::string, EnumDescriptor> m_enums;
    std::unordered_map<std::string, REType*> m_types;
    std::vector<std::string> m_sorted_types;

    // Types currently being displayed
    std::vector<REType*> m_displayed_types;

    bool m_do_init{ true };
};

