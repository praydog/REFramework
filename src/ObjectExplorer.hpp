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

    void attemptDisplayField(REManagedObject* obj, VariableDescriptor* desc);
    int32_t getFieldOffset(REManagedObject* obj, VariableDescriptor* desc);

    bool widgetWithContext(void* address, std::function<bool()> widget);
    void contextMenu(void* address);
    void makeSameLineText(std::string_view text, const ImVec4& color);

    void makeTreeOffset(REManagedObject* object, uint32_t offset, std::string_view name);
    bool isManagedObject(Address address) const;

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

    std::string m_objectAddress{ "0" };
    std::chrono::system_clock::time_point m_nextRefresh;

    std::unordered_map<VariableDescriptor*, int32_t> m_offsetMap;
};

