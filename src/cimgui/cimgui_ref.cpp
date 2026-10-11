#include <imgui.h>
#ifdef IMGUI_ENABLE_FREETYPE
#include <misc/freetype/imgui_freetype.h>
#endif
#include <imgui_internal.h>
#include "cimgui.h"
#include "utility/PersistentTreeState.hpp"

CIMGUI_API bool igTreeNode_Str(const char* label)
{
    const auto* id = label != nullptr ? label : "";
    return reframework::ui::persistent_tree_item(
        reframework::ui::TreeStateSource::CImGui,
        id,
        [label]() { return ImGui::TreeNode(label); });
}
CIMGUI_API bool igTreeNode_StrStr(const char* str_id,const char* fmt,...)
{
    va_list args;
    va_start(args, fmt);
    bool ret = reframework::ui::persistent_tree_item(
        reframework::ui::TreeStateSource::CImGui,
        str_id != nullptr ? str_id : "",
        [&]() { return ImGui::TreeNodeV(str_id,fmt,args); });
    va_end(args);
    return ret;
}
#ifdef CIMGUI_VARGS0
CIMGUI_API bool igTreeNode_StrStr0(const char* str_id,const char* fmt)
{
    return igTreeNode_StrStr(str_id,fmt);
}
#endif
CIMGUI_API bool igTreeNodeV_Str(const char* str_id,const char* fmt,va_list args)
{
    return reframework::ui::persistent_tree_item(
        reframework::ui::TreeStateSource::CImGui,
        str_id != nullptr ? str_id : "",
        [&]() { return ImGui::TreeNodeV(str_id,fmt,args); });
}
CIMGUI_API bool igTreeNodeEx_Str(const char* label,ImGuiTreeNodeFlags flags)
{
    const bool can_persist = (flags & ImGuiTreeNodeFlags_Leaf) == 0;
    return reframework::ui::persistent_tree_item(
        reframework::ui::TreeStateSource::CImGui,
        label != nullptr ? label : "",
        [label, flags]() { return ImGui::TreeNodeEx(label,flags); },
        can_persist);
}
CIMGUI_API bool igTreeNodeEx_StrStr(const char* str_id,ImGuiTreeNodeFlags flags,const char* fmt,...)
{
    va_list args;
    va_start(args, fmt);
    const bool can_persist = (flags & ImGuiTreeNodeFlags_Leaf) == 0;
    bool ret = reframework::ui::persistent_tree_item(
        reframework::ui::TreeStateSource::CImGui,
        str_id != nullptr ? str_id : "",
        [&]() { return ImGui::TreeNodeExV(str_id,flags,fmt,args); },
        can_persist);
    va_end(args);
    return ret;
}
#ifdef CIMGUI_VARGS0
CIMGUI_API bool igTreeNodeEx_StrStr0(const char* str_id,ImGuiTreeNodeFlags flags,const char* fmt)
{
    return igTreeNodeEx_StrStr(str_id,flags,fmt);
}
#endif
CIMGUI_API bool igTreeNodeExV_Str(const char* str_id,ImGuiTreeNodeFlags flags,const char* fmt,va_list args)
{
    const bool can_persist = (flags & ImGuiTreeNodeFlags_Leaf) == 0;
    return reframework::ui::persistent_tree_item(
        reframework::ui::TreeStateSource::CImGui,
        str_id != nullptr ? str_id : "",
        [&]() { return ImGui::TreeNodeExV(str_id,flags,fmt,args); },
        can_persist);
}
CIMGUI_API bool igCollapsingHeader_TreeNodeFlags(const char* label,ImGuiTreeNodeFlags flags)
{
    return reframework::ui::persistent_tree_item(
        reframework::ui::TreeStateSource::CImGui,
        label != nullptr ? label : "",
        [label, flags]() { return ImGui::CollapsingHeader(label,flags); },
        (flags & ImGuiTreeNodeFlags_Leaf) == 0);
}
CIMGUI_API bool igCollapsingHeader_BoolPtr(const char* label,bool* p_visible,ImGuiTreeNodeFlags flags)
{
    return reframework::ui::persistent_tree_item(
        reframework::ui::TreeStateSource::CImGui,
        label != nullptr ? label : "",
        [label, p_visible, flags]() { return ImGui::CollapsingHeader(label,p_visible,flags); },
        (flags & ImGuiTreeNodeFlags_Leaf) == 0);
}
