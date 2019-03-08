#pragma once

#include <unordered_set>

#include <imgui/imgui.h>

#include "utility/Address.hpp"
#include "Mod.hpp"

class ObjectExplorer : public Mod {
public:
    ObjectExplorer();

    std::string_view getName() const override { return "ObjectExplorer"; };

    void onDrawUI() override;

private:
    void handleAddress(Address address, int32_t offset = -1, Address parent = nullptr);
    void handleGameObject(REGameObject* gameObject);
    void handleComponent(REComponent* component);
    void handleTransform(RETransform* transform);
    void handleType(REManagedObject* obj, REType* t);

    void displayEnumValue(std::string_view name, int64_t value);
    void displayMethods(REManagedObject* obj, REType* typeInfo);
    void displayFields(REManagedObject* obj, REType* typeInfo);
    void attemptDisplayField(REManagedObject* obj, VariableDescriptor* desc);
    int32_t getFieldOffset(REManagedObject* obj, VariableDescriptor* desc);

    bool widgetWithContext(void* address, std::function<bool()> widget);
    void contextMenu(void* address);
    void makeSameLineText(std::string_view text, const ImVec4& color);

    void makeTreeOffset(REManagedObject* object, uint32_t offset, std::string_view name);
    bool isManagedObject(Address address) const;

    void populateClasses();
    void populateEnums();

    std::string getEnumValueName(std::string_view enumName, int64_t value);
    REType* getType(std::string_view typeName);

    template <typename T, typename... Args>
    bool stretchedTreeNode(T id, Args... args) {
        auto& style = ImGui::GetStyle();
        auto styleBak = ImGui::GetStyle();

        style.ItemSpacing.x *= 100;
        bool madeNode = ImGui::TreeNode(id, args...);
        style.ItemSpacing.x = styleBak.ItemSpacing.x;

        return madeNode;
    }

    inline static const ImVec4 VARIABLE_COLOR{ 100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 255 / 255.0f };
    inline static const ImVec4 VARIABLE_COLOR_HIGHLIGHT{ 1.0f, 1.0f, 1.0f, 1.0f };

    std::string m_typeName{ "via.typeinfo.TypeInfo" };
    std::string m_objectAddress{ "0" };
    std::chrono::system_clock::time_point m_nextRefresh;

    std::unordered_map<VariableDescriptor*, int32_t> m_offsetMap;

    struct EnumDescriptor {
        std::string name;
        int64_t value;
    };

    std::unordered_multimap<std::string, EnumDescriptor> m_enums;
    std::unordered_map<std::string, REType*> m_types;
    std::vector<std::string> m_sortedTypes;

    // Types currently being displayed
    std::vector<REType*> m_displayedTypes;

    bool m_doInit{ true };
};

