// https://github.com/CedricGuillemet/ImGuizmo
// v 1.84 WIP
//
// The MIT License(MIT)
//
// Copyright(c) 2021 Cedric Guillemet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <math.h>
#include <vector>
#include <float.h>
#include <array>
#include "GraphEditor.h"

namespace GraphEditor {

static inline float Distance(const ImVec2& a, const ImVec2& b)
{
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

static inline float sign(float v)
{
    return (v >= 0.f) ? 1.f : -1.f;
}

static ImVec2 GetInputSlotPos(Delegate& delegate, const Node& node, SlotIndex slotIndex, float factor)
{
    ImVec2 Size = node.mRect.GetSize() * factor;
    size_t InputsCount = delegate.GetTemplate(node.mTemplateIndex).mInputCount;
    return ImVec2(node.mRect.Min.x * factor,
                  node.mRect.Min.y * factor + Size.y * ((float)slotIndex + 1) / ((float)InputsCount + 1) + 8.f);
}

static ImVec2 GetOutputSlotPos(Delegate& delegate, const Node& node, SlotIndex slotIndex, float factor)
{
    ImVec2 Size = node.mRect.GetSize() * factor;
    size_t OutputsCount = delegate.GetTemplate(node.mTemplateIndex).mOutputCount;
    return ImVec2(node.mRect.Min.x * factor + Size.x,
                  node.mRect.Min.y * factor + Size.y * ((float)slotIndex + 1) / ((float)OutputsCount + 1) + 8.f);
}

static ImRect GetNodeRect(const Node& node, float factor)
{
    ImVec2 Size = node.mRect.GetSize() * factor;
    return ImRect(node.mRect.Min * factor, node.mRect.Min * factor + Size);
}

static ImVec2 editingNodeSource;
static bool editingInput = false;
static ImVec2 captureOffset;

enum NodeOperation
{
    NO_None,
    NO_EditingLink,
    NO_QuadSelecting,
    NO_MovingNodes,
    NO_EditInput,
    NO_PanView,
};
static NodeOperation nodeOperation = NO_None;

static void HandleZoomScroll(ImRect regionRect, ViewState& viewState, const Options& options)
{
    ImGuiIO& io = ImGui::GetIO();

    if (regionRect.Contains(io.MousePos))
    {
        if (io.MouseWheel < -FLT_EPSILON)
        {
            viewState.mFactorTarget *= 1.f - options.mZoomRatio;
        }

        if (io.MouseWheel > FLT_EPSILON)
        {
            viewState.mFactorTarget *= 1.0f + options.mZoomRatio;
        }
    }

    ImVec2 mouseWPosPre = (io.MousePos - ImGui::GetCursorScreenPos()) / viewState.mFactor;
    viewState.mFactorTarget = ImClamp(viewState.mFactorTarget, options.mMinZoom, options.mMaxZoom);
    viewState.mFactor = ImLerp(viewState.mFactor, viewState.mFactorTarget, options.mZoomLerpFactor);
    ImVec2 mouseWPosPost = (io.MousePos - ImGui::GetCursorScreenPos()) / viewState.mFactor;
    if (ImGui::IsMousePosValid())
    {
        viewState.mPosition += mouseWPosPost - mouseWPosPre;
    }
}

void GraphEditorClear()
{
    nodeOperation = NO_None;
}

static void FitNodes(Delegate& delegate, ViewState& viewState, const ImVec2 viewSize, bool selectedNodesOnly)
{
    const size_t nodeCount = delegate.GetNodeCount();

    if (!nodeCount)
    {
        return;
    }

    bool validNode = false;
    ImVec2 min(FLT_MAX, FLT_MAX);
    ImVec2 max(-FLT_MAX, -FLT_MAX);
    for (NodeIndex nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
    {
        const Node& node = delegate.GetNode(nodeIndex);
        
        if (selectedNodesOnly && !node.mSelected)
        {
            continue;
        }
        
        min = ImMin(min, node.mRect.Min);
        min = ImMin(min, node.mRect.Max);
        max = ImMax(max, node.mRect.Min);
        max = ImMax(max, node.mRect.Max);
        validNode = true;
    }
    
    if (!validNode)
    {
        return;
    }
    
    min -= viewSize * 0.05f;
    max += viewSize * 0.05f;
    ImVec2 nodesSize = max - min;
    ImVec2 nodeCenter = (max + min) * 0.5f;
    
    float ratioY = viewSize.y / nodesSize.y;
    float ratioX = viewSize.x / nodesSize.x;

    viewState.mFactor = viewState.mFactorTarget = ImMin(ImMin(ratioY, ratioX), 1.f);
    viewState.mPosition = ImVec2(-nodeCenter.x, -nodeCenter.y) + (viewSize * 0.5f) / viewState.mFactorTarget;
}

static void DisplayLinks(Delegate& delegate,
                         ImDrawList* drawList,
                         const ImVec2 offset,
                         const float factor,
                         const ImRect regionRect,
                         NodeIndex hoveredNode,
                         const Options& options)
{
    const size_t linkCount = delegate.GetLinkCount();
    for (LinkIndex linkIndex = 0; linkIndex < linkCount; linkIndex++)
    {
        const auto link = delegate.GetLink(linkIndex);
        const auto nodeInput = delegate.GetNode(link.mInputNodeIndex);
        const auto nodeOutput = delegate.GetNode(link.mOutputNodeIndex);
        ImVec2 p1 = offset + GetOutputSlotPos(delegate, nodeInput, link.mInputSlotIndex, factor);
        ImVec2 p2 = offset + GetInputSlotPos(delegate, nodeOutput, link.mOutputSlotIndex, factor);

        // con. view clipping
        if ((p1.y < 0.f && p2.y < 0.f) || (p1.y > regionRect.Max.y && p2.y > regionRect.Max.y) ||
            (p1.x < 0.f && p2.x < 0.f) || (p1.x > regionRect.Max.x && p2.x > regionRect.Max.x))
            continue;

        bool highlightCons = hoveredNode == link.mInputNodeIndex || hoveredNode == link.mOutputNodeIndex;
        uint32_t col = delegate.GetTemplate(nodeInput.mTemplateIndex).mHeaderColor | (highlightCons ? 0xF0F0F0 : 0);
        if (options.mDisplayLinksAsCurves)
        {
            // curves
             drawList->AddBezierCubic(p1, p1 + ImVec2(50, 0) * factor, p2 + ImVec2(-50, 0) * factor, p2, 0xFF000000, options.mLineThickness * 1.5f * factor);
             drawList->AddBezierCubic(p1, p1 + ImVec2(50, 0) * factor, p2 + ImVec2(-50, 0) * factor, p2, col, options.mLineThickness * 1.5f * factor);
             /*
            ImVec2 p10 = p1 + ImVec2(20.f * factor, 0.f);
            ImVec2 p20 = p2 - ImVec2(20.f * factor, 0.f);

            ImVec2 dif = p20 - p10;
            ImVec2 p1a, p1b;
            if (fabsf(dif.x) > fabsf(dif.y))
            {
                p1a = p10 + ImVec2(fabsf(fabsf(dif.x) - fabsf(dif.y)) * 0.5 * sign(dif.x), 0.f);
                p1b = p1a + ImVec2(fabsf(dif.y) * sign(dif.x) , dif.y);
            }
            else
            {
                p1a = p10 + ImVec2(0.f, fabsf(fabsf(dif.y) - fabsf(dif.x)) * 0.5 * sign(dif.y));
                p1b = p1a + ImVec2(dif.x, fabsf(dif.x) * sign(dif.y));
            }
            drawList->AddLine(p1,  p10, col, 3.f * factor);
            drawList->AddLine(p10, p1a, col, 3.f * factor);
            drawList->AddLine(p1a, p1b, col, 3.f * factor);
            drawList->AddLine(p1b, p20, col, 3.f * factor);
            drawList->AddLine(p20,  p2, col, 3.f * factor);
            */
        }
        else
        {
            // straight lines
            std::array<ImVec2, 6> pts;
            int ptCount = 0;
            ImVec2 dif = p2 - p1;

            ImVec2 p1a, p1b;
            const float limitx = 12.f * factor;
            if (dif.x < limitx)
            {
                ImVec2 p10 = p1 + ImVec2(limitx, 0.f);
                ImVec2 p20 = p2 - ImVec2(limitx, 0.f);

                dif = p20 - p10;
                p1a = p10 + ImVec2(0.f, dif.y * 0.5f);
                p1b = p1a + ImVec2(dif.x, 0.f);

                pts = { p1, p10, p1a, p1b, p20, p2 };
                ptCount = 6;
            }
            else
            {
                if (fabsf(dif.y) < 1.f)
                {
                    pts = { p1, (p1 + p2) * 0.5f, p2 };
                    ptCount = 3;
                }
                else
                {
                    if (fabsf(dif.y) < 10.f)
                    {
                        if (fabsf(dif.x) > fabsf(dif.y))
                        {
                            p1a = p1 + ImVec2(fabsf(fabsf(dif.x) - fabsf(dif.y)) * 0.5f * sign(dif.x), 0.f);
                            p1b = p1a + ImVec2(fabsf(dif.y) * sign(dif.x), dif.y);
                        }
                        else
                        {
                            p1a = p1 + ImVec2(0.f, fabsf(fabsf(dif.y) - fabsf(dif.x)) * 0.5f * sign(dif.y));
                            p1b = p1a + ImVec2(dif.x, fabsf(dif.x) * sign(dif.y));
                        }
                    }
                    else
                    {
                        if (fabsf(dif.x) > fabsf(dif.y))
                        {
                            float d = fabsf(dif.y) * sign(dif.x) * 0.5f;
                            p1a = p1 + ImVec2(d, dif.y * 0.5f);
                            p1b = p1a + ImVec2(fabsf(fabsf(dif.x) - fabsf(d) * 2.f) * sign(dif.x), 0.f);
                        }
                        else
                        {
                            float d = fabsf(dif.x) * sign(dif.y) * 0.5f;
                            p1a = p1 + ImVec2(dif.x * 0.5f, d);
                            p1b = p1a + ImVec2(0.f, fabsf(fabsf(dif.y) - fabsf(d) * 2.f) * sign(dif.y));
                        }
                    }
                    pts = { p1, p1a, p1b, p2 };
                    ptCount = 4;
                }
            }
            float highLightFactor = factor * (highlightCons ? 2.0f : 1.f);
            for (int pass = 0; pass < 2; pass++)
            {
                drawList->AddPolyline(pts.data(), ptCount, pass ? col : 0xFF000000, false, (pass ? options.mLineThickness : (options.mLineThickness * 1.5f)) * highLightFactor);
            }
        }
    }
}

static void HandleQuadSelection(Delegate& delegate, ImDrawList* drawList, const ImVec2 offset, const float factor, ImRect contentRect, const Options& options)
{
    if (!options.mAllowQuadSelection)
    {
        return;
    }
    ImGuiIO& io = ImGui::GetIO();
    static ImVec2 quadSelectPos;
    //auto& nodes = delegate->GetNodes();
    auto nodeCount = delegate.GetNodeCount();

    if (nodeOperation == NO_QuadSelecting && ImGui::IsWindowFocused())
    {
        const ImVec2 bmin = ImMin(quadSelectPos, io.MousePos);
        const ImVec2 bmax = ImMax(quadSelectPos, io.MousePos);
        drawList->AddRectFilled(bmin, bmax, options.mQuadSelection, 1.f);
        drawList->AddRect(bmin, bmax, options.mQuadSelectionBorder, 1.f);
        if (!io.MouseDown[0])
        {
            if (!io.KeyCtrl && !io.KeyShift)
            {
                for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
                {
                    delegate.SelectNode(nodeIndex, false);
                }
            }

            nodeOperation = NO_None;
            ImRect selectionRect(bmin, bmax);
            for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
            {
                const auto node = delegate.GetNode(nodeIndex);
                ImVec2 nodeRectangleMin = offset + node.mRect.Min * factor;
                ImVec2 nodeRectangleMax = nodeRectangleMin + node.mRect.GetSize() * factor;
                if (selectionRect.Overlaps(ImRect(nodeRectangleMin, nodeRectangleMax)))
                {
                    if (io.KeyCtrl)
                    {
                        delegate.SelectNode(nodeIndex, false);
                    }
                    else
                    {
                        delegate.SelectNode(nodeIndex, true);
                    }
                }
                else
                {
                    if (!io.KeyShift)
                    {
                        delegate.SelectNode(nodeIndex, false);
                    }
                }
            }
        }
    }
    else if (nodeOperation == NO_None && io.MouseDown[0] && ImGui::IsWindowFocused() &&
             contentRect.Contains(io.MousePos))
    {
        nodeOperation = NO_QuadSelecting;
        quadSelectPos = io.MousePos;
    }
}

static bool HandleConnections(ImDrawList* drawList,
                       NodeIndex nodeIndex,
                       const ImVec2 offset,
                       const float factor,
                       Delegate& delegate,
                       const Options& options,
                       bool bDrawOnly,
                       SlotIndex& inputSlotOver,
                       SlotIndex& outputSlotOver,
                       const bool inMinimap)
{
    static NodeIndex editingNodeIndex;
    static SlotIndex editingSlotIndex;

    ImGuiIO& io = ImGui::GetIO();
    const auto node = delegate.GetNode(nodeIndex);
    const auto nodeTemplate = delegate.GetTemplate(node.mTemplateIndex);
    const auto linkCount = delegate.GetLinkCount();

    size_t InputsCount = nodeTemplate.mInputCount;
    size_t OutputsCount = nodeTemplate.mOutputCount;
    inputSlotOver = -1;
    outputSlotOver = -1;

    // draw/use inputs/outputs
    bool hoverSlot = false;
    for (int i = 0; i < 2; i++)
    {
        float closestDistance = FLT_MAX;
        SlotIndex closestConn = -1;
        ImVec2 closestTextPos;
        ImVec2 closestPos;
        const size_t slotCount[2] = {InputsCount, OutputsCount};
        
        for (SlotIndex slotIndex = 0; slotIndex < slotCount[i]; slotIndex++)
        {
            const char** con = i ? nodeTemplate.mOutputNames : nodeTemplate.mInputNames;
            const char* conText = (con && con[slotIndex]) ? con[slotIndex] : "";

            ImVec2 p =
                offset + (i ? GetOutputSlotPos(delegate, node, slotIndex, factor) : GetInputSlotPos(delegate, node, slotIndex, factor));
            float distance = Distance(p, io.MousePos);
            bool overCon = (nodeOperation == NO_None || nodeOperation == NO_EditingLink) &&
                           (distance < options.mNodeSlotRadius * 2.f) && (distance < closestDistance);

            
            ImVec2 textSize;
            textSize = ImGui::CalcTextSize(conText);
            ImVec2 textPos =
                p + ImVec2(-options.mNodeSlotRadius * (i ? -1.f : 1.f) * (overCon ? 3.f : 2.f) - (i ? 0 : textSize.x),
                           -textSize.y / 2);

            ImRect nodeRect = GetNodeRect(node, factor);
            if (!inMinimap && (overCon || (nodeRect.Contains(io.MousePos - offset) && closestConn == -1 &&
                            (editingInput == (i != 0)) && nodeOperation == NO_EditingLink)))
            {
                closestDistance = distance;
                closestConn = slotIndex;
                closestTextPos = textPos;
                closestPos = p;
                
                if (i)
                {
                    outputSlotOver = slotIndex;
                }
                else
                {
                    inputSlotOver = slotIndex;
                }
            }
            else
            {
               const ImU32* slotColorSource = i ? nodeTemplate.mOutputColors : nodeTemplate.mInputColors;
               const ImU32 slotColor = slotColorSource ? slotColorSource[slotIndex] : options.mDefaultSlotColor;
               drawList->AddCircleFilled(p, options.mNodeSlotRadius, IM_COL32(0, 0, 0, 200));
               drawList->AddCircleFilled(p, options.mNodeSlotRadius * 0.75f, slotColor);
               if (!options.mDrawIONameOnHover)
               {
                    drawList->AddText(io.FontDefault, 14, textPos + ImVec2(2, 2), IM_COL32(0, 0, 0, 255), conText);
                    drawList->AddText(io.FontDefault, 14, textPos, IM_COL32(150, 150, 150, 255), conText);
               }
            }
        }

        if (closestConn != -1)
        {
            const char** con = i ? nodeTemplate.mOutputNames : nodeTemplate.mInputNames;
            const char* conText = (con && con[closestConn]) ? con[closestConn] : "";
            const ImU32* slotColorSource = i ? nodeTemplate.mOutputColors : nodeTemplate.mInputColors;
            const ImU32 slotColor = slotColorSource ? slotColorSource[closestConn] : options.mDefaultSlotColor;
            hoverSlot = true;
            drawList->AddCircleFilled(closestPos, options.mNodeSlotRadius * options.mNodeSlotHoverFactor * 0.75f, IM_COL32(0, 0, 0, 200));
            drawList->AddCircleFilled(closestPos, options.mNodeSlotRadius * options.mNodeSlotHoverFactor, slotColor);
            drawList->AddText(io.FontDefault, 16, closestTextPos + ImVec2(1, 1), IM_COL32(0, 0, 0, 255), conText);
            drawList->AddText(io.FontDefault, 16, closestTextPos, IM_COL32(250, 250, 250, 255), conText);
            bool inputToOutput = (!editingInput && !i) || (editingInput && i);
            if (nodeOperation == NO_EditingLink && !io.MouseDown[0] && !bDrawOnly)
            {
                if (inputToOutput)
                {
                    // check loopback
                    Link nl;
                    if (editingInput)
                        nl = Link{nodeIndex, closestConn, editingNodeIndex, editingSlotIndex};
                    else
                        nl = Link{editingNodeIndex, editingSlotIndex, nodeIndex, closestConn};

                    if (!delegate.AllowedLink(nl.mOutputNodeIndex, nl.mInputNodeIndex))
                    {
                        break;
                    }
                    bool alreadyExisting = false;
                    for (size_t linkIndex = 0; linkIndex < linkCount; linkIndex++)
                    {
                        const auto link = delegate.GetLink(linkIndex);
                        if (!memcmp(&link, &nl, sizeof(Link)))
                        {
                            alreadyExisting = true;
                            break;
                        }
                    }

                    if (!alreadyExisting)
                    {
                        for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
                        {
                            const auto link = delegate.GetLink(linkIndex);
                            if (link.mOutputNodeIndex == nl.mOutputNodeIndex && link.mOutputSlotIndex == nl.mOutputSlotIndex)
                            {
                                delegate.DelLink(linkIndex);
                                
                                break;
                            }
                        }

                        delegate.AddLink(nl.mInputNodeIndex, nl.mInputSlotIndex, nl.mOutputNodeIndex, nl.mOutputSlotIndex);
                    }
                }
            }
            // when ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() is uncommented, one can't click the node
            // input/output when mouse is over the node itself.
            if (nodeOperation == NO_None &&
                /*ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() &&*/ io.MouseClicked[0] && !bDrawOnly)
            {
                nodeOperation = NO_EditingLink;
                editingInput = i == 0;
                editingNodeSource = closestPos;
                editingNodeIndex = nodeIndex;
                editingSlotIndex = closestConn;
                if (editingInput)
                {
                    // remove existing link
                    for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
                    {
                        const auto link = delegate.GetLink(linkIndex);
                        if (link.mOutputNodeIndex == nodeIndex && link.mOutputSlotIndex == closestConn)
                        {
                            delegate.DelLink(linkIndex);
                            break;
                        }
                    }
                }
            }
        }
    }
    return hoverSlot;
}

static void DrawGrid(ImDrawList* drawList, ImVec2 windowPos, const ViewState& viewState, const ImVec2 canvasSize, ImU32 gridColor, ImU32 gridColor2, float gridSize)
{
    float gridSpace = gridSize * viewState.mFactor;
    int divx = static_cast<int>(-viewState.mPosition.x / gridSize);
    int divy = static_cast<int>(-viewState.mPosition.y / gridSize);
    for (float x = fmodf(viewState.mPosition.x * viewState.mFactor, gridSpace); x < canvasSize.x; x += gridSpace, divx ++)
    {
        bool tenth = !(divx % 10);
        drawList->AddLine(ImVec2(x, 0.0f) + windowPos, ImVec2(x, canvasSize.y) + windowPos, tenth ? gridColor2 : gridColor);
    }
    for (float y = fmodf(viewState.mPosition.y * viewState.mFactor, gridSpace); y < canvasSize.y; y += gridSpace, divy ++)
    {
        bool tenth = !(divy % 10);
        drawList->AddLine(ImVec2(0.0f, y) + windowPos, ImVec2(canvasSize.x, y) + windowPos, tenth ? gridColor2 : gridColor);
    }
}

// return true if node is hovered
static bool DrawNode(ImDrawList* drawList,
                     NodeIndex nodeIndex,
                     const ImVec2 offset,
                     const float factor,
                     Delegate& delegate,
                     bool overInput,
                     const Options& options,
                     const bool inMinimap,
                     const ImRect& viewPort)
{
    ImGuiIO& io = ImGui::GetIO();
    const auto node = delegate.GetNode(nodeIndex);
    IM_ASSERT((node.mRect.GetWidth() != 0.f) && (node.mRect.GetHeight() != 0.f) && "Nodes must have a non-zero rect.");
    const auto nodeTemplate = delegate.GetTemplate(node.mTemplateIndex);
    const ImVec2 nodeRectangleMin = offset + node.mRect.Min * factor;

    const bool old_any_active = ImGui::IsAnyItemActive();
    ImGui::SetCursorScreenPos(nodeRectangleMin);
    const ImVec2 nodeSize = node.mRect.GetSize() * factor;

    // test nested IO
    drawList->ChannelsSetCurrent(1); // Background
    const size_t InputsCount = nodeTemplate.mInputCount;
    const size_t OutputsCount = nodeTemplate.mOutputCount;

    /*
    for (int i = 0; i < 2; i++)
    {
        const size_t slotCount[2] = {InputsCount, OutputsCount};
        
        for (size_t slotIndex = 0; slotIndex < slotCount[i]; slotIndex++)
        {
            const char* con = i ? nodeTemplate.mOutputNames[slotIndex] : nodeTemplate.mInputNames[slotIndex];//node.mOutputs[slot_idx] : node->mInputs[slot_idx];
            if (!delegate->IsIOPinned(nodeIndex, slot_idx, i == 1))
            {
               
            }
            continue;

            ImVec2 p = offset + (i ? GetOutputSlotPos(delegate, node, slotIndex, factor) : GetInputSlotPos(delegate, node, slotIndex, factor));
            const float arc = 28.f * (float(i) * 0.3f + 1.0f) * (i ? 1.f : -1.f);
            const float ofs = 0.f;

            ImVec2 pts[3] = {p + ImVec2(arc + ofs, 0.f), p + ImVec2(0.f + ofs, -arc), p + ImVec2(0.f + ofs, arc)};
            drawList->AddTriangleFilled(pts[0], pts[1], pts[2], i ? 0xFFAA5030 : 0xFF30AA50);
            drawList->AddTriangle(pts[0], pts[1], pts[2], 0xFF000000, 2.f);
        }
    }
    */

    ImGui::SetCursorScreenPos(nodeRectangleMin);
    float maxHeight = ImMin(viewPort.Max.y, nodeRectangleMin.y + nodeSize.y) - nodeRectangleMin.y;
    float maxWidth = ImMin(viewPort.Max.x, nodeRectangleMin.x + nodeSize.x) - nodeRectangleMin.x;
    ImGui::InvisibleButton("node", ImVec2(maxWidth, maxHeight));
    // must be called right after creating the control we want to be able to move
    bool nodeMovingActive = ImGui::IsItemActive();

    // Save the size of what we have emitted and whether any of the widgets are being used
    bool nodeWidgetsActive = (!old_any_active && ImGui::IsAnyItemActive());
    ImVec2 nodeRectangleMax = nodeRectangleMin + nodeSize;

    bool nodeHovered = false;
    if (ImGui::IsItemHovered() && nodeOperation == NO_None && !overInput)
    {
        nodeHovered = true;
    }

    if (ImGui::IsWindowFocused())
    {
        if ((nodeWidgetsActive || nodeMovingActive) && !inMinimap)
        {
            if (!node.mSelected)
            {
                if (!io.KeyShift)
                {
                    const auto nodeCount = delegate.GetNodeCount();
                    for (size_t i = 0; i < nodeCount; i++)
                    {
                        delegate.SelectNode(i, false);
                    }
                }
                delegate.SelectNode(nodeIndex, true);
            }
        }
    }
    if (nodeMovingActive && io.MouseDown[0] && nodeHovered && !inMinimap)
    {
        if (nodeOperation != NO_MovingNodes)
        {
            nodeOperation = NO_MovingNodes;
        }
    }

    const bool currentSelectedNode = node.mSelected;
    const ImU32 node_bg_color = nodeHovered ? nodeTemplate.mBackgroundColorOver : nodeTemplate.mBackgroundColor;

    drawList->AddRect(nodeRectangleMin,
                      nodeRectangleMax,
                      currentSelectedNode ? options.mSelectedNodeBorderColor : options.mNodeBorderColor,
                      options.mRounding,
                      ImDrawFlags_RoundCornersAll,
                      currentSelectedNode ? options.mBorderSelectionThickness : options.mBorderThickness);

    ImVec2 imgPos = nodeRectangleMin + ImVec2(14, 25);
    ImVec2 imgSize = nodeRectangleMax + ImVec2(-5, -5) - imgPos;
    float imgSizeComp = std::min(imgSize.x, imgSize.y);

    drawList->AddRectFilled(nodeRectangleMin, nodeRectangleMax, node_bg_color, options.mRounding);
    /*float progress = delegate->NodeProgress(nodeIndex);
    if (progress > FLT_EPSILON && progress < 1.f - FLT_EPSILON)
    {
        ImVec2 progressLineA = nodeRectangleMax - ImVec2(nodeSize.x - 2.f, 3.f);
        ImVec2 progressLineB = progressLineA + ImVec2(nodeSize.x * factor - 4.f, 0.f);
        drawList->AddLine(progressLineA, progressLineB, 0xFF400000, 3.f);
        drawList->AddLine(progressLineA, ImLerp(progressLineA, progressLineB, progress), 0xFFFF0000, 3.f);
    }*/
    ImVec2 imgPosMax = imgPos + ImVec2(imgSizeComp, imgSizeComp);

    //ImVec2 imageSize = delegate->GetEvaluationSize(nodeIndex);
    /*float imageRatio = 1.f;
    if (imageSize.x > 0.f && imageSize.y > 0.f)
    {
        imageRatio = imageSize.y / imageSize.x;
    }
    ImVec2 quadSize = imgPosMax - imgPos;
    ImVec2 marge(0.f, 0.f);
    if (imageRatio > 1.f)
    {
        marge.x = (quadSize.x - quadSize.y / imageRatio) * 0.5f;
    }
    else
    {
        marge.y = (quadSize.y - quadSize.y * imageRatio) * 0.5f;
    }*/

    //delegate->DrawNodeImage(drawList, ImRect(imgPos, imgPosMax), marge, nodeIndex);

    drawList->AddRectFilled(nodeRectangleMin,
                            ImVec2(nodeRectangleMax.x, nodeRectangleMin.y + 20),
                            nodeTemplate.mHeaderColor, options.mRounding);

    drawList->PushClipRect(nodeRectangleMin, ImVec2(nodeRectangleMax.x, nodeRectangleMin.y + 20), true);
    drawList->AddText(nodeRectangleMin + ImVec2(2, 2), IM_COL32(0, 0, 0, 255), node.mName);
    drawList->PopClipRect();

    ImRect customDrawRect(nodeRectangleMin + ImVec2(options.mRounding, 20 + options.mRounding), nodeRectangleMax - ImVec2(options.mRounding, options.mRounding));
    if (customDrawRect.Max.y > customDrawRect.Min.y && customDrawRect.Max.x > customDrawRect.Min.x)
    {
        delegate.CustomDraw(drawList, customDrawRect, nodeIndex);
    }
/*
    const ImTextureID bmpInfo = (ImTextureID)(uint64_t)delegate->GetBitmapInfo(nodeIndex).idx;
    if (bmpInfo)
    {
        ImVec2 bmpInfoPos(nodeRectangleMax - ImVec2(26, 12));
        ImVec2 bmpInfoSize(20, 20);
        if (delegate->NodeIsCompute(nodeIndex))
        {
            drawList->AddImageQuad(bmpInfo,
                                   bmpInfoPos,
                                   bmpInfoPos + ImVec2(bmpInfoSize.x, 0.f),
                                   bmpInfoPos + bmpInfoSize,
                                   bmpInfoPos + ImVec2(0., bmpInfoSize.y));
        }
        else if (delegate->NodeIs2D(nodeIndex))
        {
            drawList->AddImageQuad(bmpInfo,
                                   bmpInfoPos,
                                   bmpInfoPos + ImVec2(bmpInfoSize.x, 0.f),
                                   bmpInfoPos + bmpInfoSize,
                                   bmpInfoPos + ImVec2(0., bmpInfoSize.y));
        }
        else if (delegate->NodeIsCubemap(nodeIndex))
        {
            drawList->AddImageQuad(bmpInfo,
                                   bmpInfoPos + ImVec2(0., bmpInfoSize.y),
                                   bmpInfoPos + bmpInfoSize,
                                   bmpInfoPos + ImVec2(bmpInfoSize.x, 0.f),
                                   bmpInfoPos);
        }
    }*/
    return nodeHovered;
}

bool DrawMiniMap(ImDrawList* drawList, Delegate& delegate, ViewState& viewState, const Options& options, const ImVec2 windowPos, const ImVec2 canvasSize)
{
    if (Distance(options.mMinimap.Min, options.mMinimap.Max) <= FLT_EPSILON)
    {
        return false;
    }

    const size_t nodeCount = delegate.GetNodeCount();

    if (!nodeCount)
    {
        return false;
    }

    ImVec2 min(FLT_MAX, FLT_MAX);
    ImVec2 max(-FLT_MAX, -FLT_MAX);
    const ImVec2 margin(50, 50);
    for (NodeIndex nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
    {
        const Node& node = delegate.GetNode(nodeIndex);
        min = ImMin(min, node.mRect.Min - margin);
        min = ImMin(min, node.mRect.Max + margin);
        max = ImMax(max, node.mRect.Min - margin);
        max = ImMax(max, node.mRect.Max + margin);
    }

    // add view in world space
    const ImVec2 worldSizeView = canvasSize / viewState.mFactor;
    const ImVec2 viewMin(-viewState.mPosition.x, -viewState.mPosition.y);
    const ImVec2 viewMax = viewMin + worldSizeView;
    min = ImMin(min, viewMin);
    max = ImMax(max, viewMax);
    const ImVec2 nodesSize = max - min;
    const ImVec2 middleWorld = (min + max) * 0.5f;
    const ImVec2 minScreen = windowPos + options.mMinimap.Min * canvasSize;
    const ImVec2 maxScreen = windowPos + options.mMinimap.Max * canvasSize;
    const ImVec2 viewSize = maxScreen - minScreen;
    const ImVec2 middleScreen = (minScreen + maxScreen) * 0.5f;
    const float ratioY = viewSize.y / nodesSize.y;
    const float ratioX = viewSize.x / nodesSize.x;
    const float factor = ImMin(ImMin(ratioY, ratioX), 1.f);

    drawList->AddRectFilled(minScreen, maxScreen, IM_COL32(30, 30, 30, 200), 3, ImDrawFlags_RoundCornersAll);

    for (NodeIndex nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
    {
        const Node& node = delegate.GetNode(nodeIndex);
        const auto nodeTemplate = delegate.GetTemplate(node.mTemplateIndex);

        ImRect rect = node.mRect;
        rect.Min -= middleWorld;
        rect.Min *= factor;
        rect.Min += middleScreen;

        rect.Max -= middleWorld;
        rect.Max *= factor;
        rect.Max += middleScreen;

        drawList->AddRectFilled(rect.Min, rect.Max, nodeTemplate.mBackgroundColor, 1, ImDrawFlags_RoundCornersAll);
        if (node.mSelected)
        {
            drawList->AddRect(rect.Min, rect.Max, options.mSelectedNodeBorderColor, 1, ImDrawFlags_RoundCornersAll);
        }
    }

    // add view
    ImVec2 viewMinScreen = (viewMin - middleWorld) * factor + middleScreen;
    ImVec2 viewMaxScreen = (viewMax - middleWorld) * factor + middleScreen;
    drawList->AddRectFilled(viewMinScreen, viewMaxScreen, IM_COL32(255, 255, 255, 32), 1, ImDrawFlags_RoundCornersAll);
    drawList->AddRect(viewMinScreen, viewMaxScreen, IM_COL32(255, 255, 255, 128), 1, ImDrawFlags_RoundCornersAll);
    
    ImGuiIO& io = ImGui::GetIO();
    const bool mouseInMinimap = ImRect(minScreen, maxScreen).Contains(io.MousePos);
    if (mouseInMinimap && io.MouseClicked[0])
    {
        const ImVec2 clickedRatio = (io.MousePos - minScreen) / viewSize;
        const ImVec2 worldPosCenter = ImVec2(ImLerp(min.x, max.x, clickedRatio.x), ImLerp(min.y, max.y, clickedRatio.y));
        
        ImVec2 worldPosViewMin = worldPosCenter - worldSizeView * 0.5;
        ImVec2 worldPosViewMax = worldPosCenter + worldSizeView * 0.5;
        if (worldPosViewMin.x < min.x)
        {
            worldPosViewMin.x = min.x;
            worldPosViewMax.x = worldPosViewMin.x + worldSizeView.x;
        }
        if (worldPosViewMin.y < min.y)
        {
            worldPosViewMin.y = min.y;
            worldPosViewMax.y = worldPosViewMin.y + worldSizeView.y;
        }
        if (worldPosViewMax.x > max.x)
        {
            worldPosViewMax.x = max.x;
            worldPosViewMin.x = worldPosViewMax.x - worldSizeView.x;
        }
        if (worldPosViewMax.y > max.y)
        {
            worldPosViewMax.y = max.y;
            worldPosViewMin.y = worldPosViewMax.y - worldSizeView.y;
        }
        viewState.mPosition = ImVec2(-worldPosViewMin.x, -worldPosViewMin.y);
    }
    return mouseInMinimap;
}

void Show(Delegate& delegate, const Options& options, ViewState& viewState, bool enabled, FitOnScreen* fit)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);

    const ImVec2 windowPos = ImGui::GetCursorScreenPos();
    const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    const ImVec2 scrollRegionLocalPos(0, 0);

    ImRect regionRect(windowPos, windowPos + canvasSize);

    HandleZoomScroll(regionRect, viewState, options);
    ImVec2 offset = ImGui::GetCursorScreenPos() + viewState.mPosition * viewState.mFactor;
    captureOffset = viewState.mPosition * viewState.mFactor;

    //ImGui::InvisibleButton("GraphEditorButton", canvasSize);
    ImGui::BeginChildFrame(71711, canvasSize);

    ImGui::SetCursorPos(windowPos);
    ImGui::BeginGroup();

    ImGuiIO& io = ImGui::GetIO();

    // Create our child canvas
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(30, 30, 30, 200));

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImGui::PushClipRect(regionRect.Min, regionRect.Max, true);
    drawList->AddRectFilled(windowPos, windowPos + canvasSize, options.mBackgroundColor);

    // Background or Display grid
    if (options.mRenderGrid)
    {
        DrawGrid(drawList, windowPos, viewState, canvasSize, options.mGridColor, options.mGridColor2, options.mGridSize);
    }
    
    // Fit view
    if (fit && ((*fit == Fit_AllNodes) || (*fit == Fit_SelectedNodes)))
    {
        FitNodes(delegate, viewState, canvasSize, (*fit == Fit_SelectedNodes));
    }

    if (enabled)
    {
        static NodeIndex hoveredNode = -1;
        // Display links
        drawList->ChannelsSplit(3);

        // minimap
        drawList->ChannelsSetCurrent(2); // minimap
        const bool inMinimap = DrawMiniMap(drawList, delegate, viewState, options, windowPos, canvasSize);

        // Focus rectangle
        if (ImGui::IsWindowFocused())
        {
           drawList->AddRect(regionRect.Min, regionRect.Max, options.mFrameFocus, 1.f, 0, 2.f);
        }

        drawList->ChannelsSetCurrent(1); // Background

        // Links
        DisplayLinks(delegate, drawList, offset, viewState.mFactor, regionRect, hoveredNode, options);

        // edit node link
        if (nodeOperation == NO_EditingLink)
        {
            ImVec2 p1 = editingNodeSource;
            ImVec2 p2 = io.MousePos;
            drawList->AddLine(p1, p2, IM_COL32(200, 200, 200, 255), 3.0f);
        }

        // Display nodes
        drawList->PushClipRect(regionRect.Min, regionRect.Max, true);
        hoveredNode = -1;
        
        SlotIndex inputSlotOver = -1;
        SlotIndex outputSlotOver = -1;
        NodeIndex nodeOver = -1;

        const auto nodeCount = delegate.GetNodeCount();
        for (int i = 0; i < 2; i++)
        {
            for (NodeIndex nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
            {
                //const auto* node = &nodes[nodeIndex];
                const auto node = delegate.GetNode(nodeIndex);
                if (node.mSelected != (i != 0))
                {
                    continue;
                }

                // node view clipping
                ImRect nodeRect = GetNodeRect(node, viewState.mFactor);
                nodeRect.Min += offset;
                nodeRect.Max += offset;
                if (!regionRect.Overlaps(nodeRect))
                {
                    continue;
                }

                ImGui::PushID((int)nodeIndex);
                SlotIndex inputSlot = -1;
                SlotIndex outputSlot = -1;

                bool overInput = (!inMinimap) && HandleConnections(drawList, nodeIndex, offset, viewState.mFactor, delegate, options, false, inputSlot, outputSlot, inMinimap);

                // shadow
                /*
                ImVec2 shadowOffset = ImVec2(30, 30);
                ImVec2 shadowPivot = (nodeRect.Min + nodeRect.Max) /2.f;
                ImVec2 shadowPointMiddle = shadowPivot + shadowOffset;
                ImVec2 shadowPointTop = ImVec2(shadowPivot.x, nodeRect.Min.y) + shadowOffset;
                ImVec2 shadowPointBottom = ImVec2(shadowPivot.x, nodeRect.Max.y) + shadowOffset;
                ImVec2 shadowPointLeft = ImVec2(nodeRect.Min.x, shadowPivot.y) + shadowOffset;
                ImVec2 shadowPointRight = ImVec2(nodeRect.Max.x, shadowPivot.y) + shadowOffset;

                // top left
                drawList->AddRectFilledMultiColor(nodeRect.Min + shadowOffset, shadowPointMiddle, IM_COL32(0 ,0, 0, 0), IM_COL32(0,0,0,0), IM_COL32(0, 0, 0, 255), IM_COL32(0, 0, 0, 0));

                // top right
                drawList->AddRectFilledMultiColor(shadowPointTop, shadowPointRight, IM_COL32(0 ,0, 0, 0), IM_COL32(0,0,0,0), IM_COL32(0, 0, 0, 0), IM_COL32(0, 0, 0, 255));

                // bottom left
                drawList->AddRectFilledMultiColor(shadowPointLeft, shadowPointBottom, IM_COL32(0 ,0, 0, 0), IM_COL32(0, 0, 0, 255), IM_COL32(0, 0, 0, 0), IM_COL32(0,0,0,0));

                // bottom right
                drawList->AddRectFilledMultiColor(shadowPointMiddle, nodeRect.Max + shadowOffset, IM_COL32(0, 0, 0, 255), IM_COL32(0 ,0, 0, 0), IM_COL32(0,0,0,0), IM_COL32(0, 0, 0, 0));
                */
                if (DrawNode(drawList, nodeIndex, offset, viewState.mFactor, delegate, overInput, options, inMinimap, regionRect))
                {
                    hoveredNode = nodeIndex;
                }

                HandleConnections(drawList, nodeIndex, offset, viewState.mFactor, delegate, options, true, inputSlot, outputSlot, inMinimap);
                if (inputSlot != -1 || outputSlot != -1)
                {
                    inputSlotOver = inputSlot;
                    outputSlotOver = outputSlot;
                    nodeOver = nodeIndex;
                }

                ImGui::PopID();
            }
        }
        

        
        drawList->PopClipRect();

        if (nodeOperation == NO_MovingNodes)
        {
            if (ImGui::IsMouseDragging(0, 1))
            {
                ImVec2 delta = io.MouseDelta / viewState.mFactor;
                if (fabsf(delta.x) >= 1.f || fabsf(delta.y) >= 1.f)
                {
                    delegate.MoveSelectedNodes(delta);
                }
            }
        }

        drawList->ChannelsSetCurrent(0);

        // quad selection
        if (!inMinimap)
        {
            HandleQuadSelection(delegate, drawList, offset, viewState.mFactor, regionRect, options);
        }

        drawList->ChannelsMerge();

        // releasing mouse button means it's done in any operation
        if (nodeOperation == NO_PanView)
        {
            if (!io.MouseDown[2])
            {
                nodeOperation = NO_None;
            }
        }
        else if (nodeOperation != NO_None && !io.MouseDown[0])
        {
            nodeOperation = NO_None;
        }

        // right click
        if (!inMinimap && nodeOperation == NO_None && regionRect.Contains(io.MousePos) &&
                (ImGui::IsMouseClicked(1) /*|| (ImGui::IsWindowFocused() && ImGui::IsKeyPressedMap(ImGuiKey_Tab))*/))
        {
            delegate.RightClick(nodeOver, inputSlotOver, outputSlotOver);
        }

        // Scrolling
        if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && io.MouseClicked[2] && nodeOperation == NO_None)
        {
            nodeOperation = NO_PanView;
        }
        if (nodeOperation == NO_PanView)
        {
            viewState.mPosition += io.MouseDelta / viewState.mFactor;
        }
    }

    ImGui::PopClipRect();

    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(2);
    ImGui::EndGroup();
    ImGui::EndChildFrame();

    ImGui::PopStyleVar(3);
    
    // change fit to none
    if (fit)
    {
        *fit = Fit_None;
    }
}

bool EditOptions(Options& options)
{
    bool updated = false;
    if (ImGui::CollapsingHeader("Colors", nullptr))
    {
        ImColor backgroundColor(options.mBackgroundColor);
        ImColor gridColor(options.mGridColor);
        ImColor selectedNodeBorderColor(options.mSelectedNodeBorderColor);
        ImColor nodeBorderColor(options.mNodeBorderColor);
        ImColor quadSelection(options.mQuadSelection);
        ImColor quadSelectionBorder(options.mQuadSelectionBorder);
        ImColor defaultSlotColor(options.mDefaultSlotColor);
        ImColor frameFocus(options.mFrameFocus);

        updated |= ImGui::ColorEdit4("Background", (float*)&backgroundColor);
        updated |= ImGui::ColorEdit4("Grid", (float*)&gridColor);
        updated |= ImGui::ColorEdit4("Selected Node Border", (float*)&selectedNodeBorderColor);
        updated |= ImGui::ColorEdit4("Node Border", (float*)&nodeBorderColor);
        updated |= ImGui::ColorEdit4("Quad Selection", (float*)&quadSelection);
        updated |= ImGui::ColorEdit4("Quad Selection Border", (float*)&quadSelectionBorder);
        updated |= ImGui::ColorEdit4("Default Slot", (float*)&defaultSlotColor);
        updated |= ImGui::ColorEdit4("Frame when has focus", (float*)&frameFocus);

        options.mBackgroundColor = backgroundColor;
        options.mGridColor = gridColor;
        options.mSelectedNodeBorderColor = selectedNodeBorderColor;
        options.mNodeBorderColor = nodeBorderColor;
        options.mQuadSelection = quadSelection;
        options.mQuadSelectionBorder = quadSelectionBorder;
        options.mDefaultSlotColor = defaultSlotColor;
        options.mFrameFocus = frameFocus;
    }

    if (ImGui::CollapsingHeader("Options", nullptr))
    {
        updated |= ImGui::InputFloat4("Minimap", &options.mMinimap.Min.x);
        updated |= ImGui::InputFloat("Line Thickness", &options.mLineThickness);
        updated |= ImGui::InputFloat("Grid Size", &options.mGridSize);
        updated |= ImGui::InputFloat("Rounding", &options.mRounding);
        updated |= ImGui::InputFloat("Zoom Ratio", &options.mZoomRatio);
        updated |= ImGui::InputFloat("Zoom Lerp Factor", &options.mZoomLerpFactor);
        updated |= ImGui::InputFloat("Border Selection Thickness", &options.mBorderSelectionThickness);
        updated |= ImGui::InputFloat("Border Thickness", &options.mBorderThickness);
        updated |= ImGui::InputFloat("Slot Radius", &options.mNodeSlotRadius);
        updated |= ImGui::InputFloat("Slot Hover Factor", &options.mNodeSlotHoverFactor);
        updated |= ImGui::InputFloat2("Zoom min/max", &options.mMinZoom);
        updated |= ImGui::InputFloat("Slot Hover Factor", &options.mSnap);
        
        if (ImGui::RadioButton("Curved Links", options.mDisplayLinksAsCurves))
        {
            options.mDisplayLinksAsCurves = !options.mDisplayLinksAsCurves;
            updated = true;
        }
        if (ImGui::RadioButton("Straight Links", !options.mDisplayLinksAsCurves))
        {
            options.mDisplayLinksAsCurves = !options.mDisplayLinksAsCurves;
            updated = true;
        }

        updated |= ImGui::Checkbox("Allow Quad Selection", &options.mAllowQuadSelection);
        updated |= ImGui::Checkbox("Render Grid", &options.mRenderGrid);
        updated |= ImGui::Checkbox("Draw IO names on hover", &options.mDrawIONameOnHover);
    }

    return updated;
}

} // namespace
