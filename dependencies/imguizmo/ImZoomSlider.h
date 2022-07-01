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
#pragma once

namespace ImZoomSlider
{
   typedef int ImGuiZoomSliderFlags;
   enum ImGuiPopupFlags_
   {
      ImGuiZoomSliderFlags_None = 0,
      ImGuiZoomSliderFlags_Vertical = 1,
      ImGuiZoomSliderFlags_NoAnchors = 2,
      ImGuiZoomSliderFlags_NoMiddleCarets = 4,
      ImGuiZoomSliderFlags_NoWheel = 8,
   };

   template<typename T> bool ImZoomSlider(const T lower, const T higher, T& viewLower, T& viewHigher, float wheelRatio = 0.01f, ImGuiZoomSliderFlags flags = ImGuiZoomSliderFlags_None)
   {
      bool interacted = false;
      ImGuiIO& io = ImGui::GetIO();
      ImDrawList* draw_list = ImGui::GetWindowDrawList();

      static const float handleSize = 12;
      static const float roundRadius = 3.f;
      static const char* controlName = "ImZoomSlider";

      static bool movingScrollBarSvg = false;
      static bool sizingRBarSvg = false;
      static bool sizingLBarSvg = false;
      static ImGuiID editingId = (ImGuiID)-1;
      static float scrollingSource = 0.f;
      static float saveViewLower;
      static float saveViewHigher;

      const bool isVertical = flags & ImGuiZoomSliderFlags_Vertical;
      const ImVec2 canvasPos = ImGui::GetCursorScreenPos();
      const ImVec2 canvasSize = ImGui::GetContentRegionAvail();
      const float canvasSizeLength = isVertical ? ImGui::GetItemRectSize().y : canvasSize.x;
      const ImVec2 scrollBarSize = isVertical ? ImVec2(14.f, canvasSizeLength) : ImVec2(canvasSizeLength, 14.f);

      ImGui::InvisibleButton(controlName, scrollBarSize);
      const ImGuiID currentId = ImGui::GetID(controlName);

      const bool usingEditingId = currentId == editingId;
      const bool canUseControl = usingEditingId || editingId == -1;
      const bool movingScrollBar = usingEditingId ? movingScrollBarSvg : false;
      const bool sizingRBar = usingEditingId ? sizingRBarSvg : false;
      const bool sizingLBar = usingEditingId ? sizingLBarSvg : false;
      const int componentIndex = isVertical ? 1 : 0;
      const ImVec2 scrollBarMin = ImGui::GetItemRectMin();
      const ImVec2 scrollBarMax = ImGui::GetItemRectMax();
      const ImVec2 scrollBarA = ImVec2(scrollBarMin.x, scrollBarMin.y) - (isVertical ? ImVec2(2,0) : ImVec2(0,2));
      const ImVec2 scrollBarB = isVertical ? ImVec2(scrollBarMax.x - 1.f, scrollBarMin.y + canvasSizeLength) : ImVec2(scrollBarMin.x + canvasSizeLength, scrollBarMax.y - 1.f);
      const float scrollStart = ((viewLower - lower) / (higher - lower)) * canvasSizeLength + scrollBarMin[componentIndex];
      const float scrollEnd = ((viewHigher - lower) / (higher - lower)) * canvasSizeLength + scrollBarMin[componentIndex];
      const float screenSize = scrollEnd - scrollStart;
      const ImVec2 scrollTopLeft = isVertical ? ImVec2(scrollBarMin.x, scrollStart) : ImVec2(scrollStart, scrollBarMin.y);
      const ImVec2 scrollBottomRight = isVertical ? ImVec2(scrollBarMax.x - 2.f, scrollEnd) : ImVec2(scrollEnd, scrollBarMax.y - 2.f);
      const bool inScrollBar = canUseControl && ImRect(scrollTopLeft, scrollBottomRight).Contains(io.MousePos);
      const ImRect scrollBarRect(scrollBarA, scrollBarB);
      const float deltaScreen = io.MousePos[componentIndex] - scrollingSource;
      const float deltaView = ((higher - lower) / canvasSizeLength) * deltaScreen;
      const uint32_t barColor = ImGui::GetColorU32((inScrollBar || movingScrollBar) ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
      const float middleCoord = (scrollStart + scrollEnd) * 0.5f;
      const bool insideControl = canUseControl && ImRect(scrollBarMin, scrollBarMax).Contains(io.MousePos);
      const bool hasAnchors = !(flags & ImGuiZoomSliderFlags_NoAnchors);
      const float viewMinSize = ((3.f * handleSize) / canvasSizeLength) * (higher - lower);
      const auto ClipView = [lower, higher, &viewLower, &viewHigher]() {
         if (viewLower < lower)
         {
            const float deltaClip = lower - viewLower;
            viewLower += deltaClip;
            viewHigher += deltaClip;
         }
         if (viewHigher > higher)
         {
            const float deltaClip = viewHigher - higher;
            viewLower -= deltaClip;
            viewHigher -= deltaClip;
         }
      };

      bool onLeft = false;
      bool onRight = false;

      draw_list->AddRectFilled(scrollBarA, scrollBarB, 0xFF101010, roundRadius);
      draw_list->AddRectFilled(scrollBarA, scrollBarB, 0xFF222222, 0);
      draw_list->AddRectFilled(scrollTopLeft, scrollBottomRight, barColor, roundRadius);

      if (!(flags & ImGuiZoomSliderFlags_NoMiddleCarets))
      {
         for (float i = 0.5f; i < 3.f; i += 1.f)
         {
            const float coordA = middleCoord - handleSize * 0.5f;
            const float coordB = middleCoord + handleSize * 0.5f;
            ImVec2 base = scrollBarMin;
            base.x += scrollBarSize.x * 0.25f * i;
            base.y += scrollBarSize.y * 0.25f * i;

            if (isVertical)
            {
               draw_list->AddLine(ImVec2(base.x, coordA), ImVec2(base.x, coordB), ImGui::GetColorU32(ImGuiCol_SliderGrab));
            }
            else
            {
               draw_list->AddLine(ImVec2(coordA, base.y), ImVec2(coordB, base.y), ImGui::GetColorU32(ImGuiCol_SliderGrab));
            }
         }
      }

      // Mouse wheel
      if (io.MouseClicked[0] && insideControl && !inScrollBar)
      {
         const float ratio = (io.MousePos[componentIndex] - scrollBarMin[componentIndex]) / (scrollBarMax[componentIndex] - scrollBarMin[componentIndex]);
         const float size = (higher - lower);
         const float halfViewSize = (viewHigher - viewLower) * 0.5f;
         const float middle = ratio * size + lower;
         viewLower = middle - halfViewSize;
         viewHigher = middle + halfViewSize;
         ClipView();
         interacted = true;
      }

      if (!(flags & ImGuiZoomSliderFlags_NoWheel) && inScrollBar && fabsf(io.MouseWheel) > 0.f)
      {
         const float ratio = (io.MousePos[componentIndex] - scrollStart) / (scrollEnd - scrollStart);
         const float amount = io.MouseWheel * wheelRatio * (viewHigher - viewLower);
         
         viewLower -= ratio * amount;
         viewHigher += (1.f - ratio) * amount;
         ClipView();
         interacted = true;
      }

      if (screenSize > handleSize * 2.f && hasAnchors)
      {
         const ImRect barHandleLeft(scrollTopLeft, isVertical ? ImVec2(scrollBottomRight.x, scrollTopLeft.y + handleSize) : ImVec2(scrollTopLeft.x + handleSize, scrollBottomRight.y));
         const ImRect barHandleRight(isVertical ? ImVec2(scrollTopLeft.x, scrollBottomRight.y - handleSize) : ImVec2(scrollBottomRight.x - handleSize, scrollTopLeft.y), scrollBottomRight);

         onLeft = barHandleLeft.Contains(io.MousePos);
         onRight = barHandleRight.Contains(io.MousePos);

         draw_list->AddRectFilled(barHandleLeft.Min, barHandleLeft.Max, ImGui::GetColorU32((onLeft || sizingLBar) ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), roundRadius);
         draw_list->AddRectFilled(barHandleRight.Min, barHandleRight.Max, ImGui::GetColorU32((onRight || sizingRBar) ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), roundRadius);
      }

      if (sizingRBar)
      {
         if (!io.MouseDown[0])
         {
            sizingRBarSvg = false;
            editingId = (ImGuiID)-1;
         }
         else
         {
            viewHigher = ImMin(saveViewHigher + deltaView, higher);
         }
      }
      else if (sizingLBar)
      {
         if (!io.MouseDown[0])
         {
            sizingLBarSvg = false;
            editingId = (ImGuiID)-1;
         }
         else
         {
            viewLower = ImMax(saveViewLower + deltaView, lower);
         }
      }
      else
      {
         if (movingScrollBar)
         {
            if (!io.MouseDown[0])
            {
               movingScrollBarSvg = false;
               editingId = (ImGuiID)-1;
            }
            else
            {
               viewLower = saveViewLower + deltaView;
               viewHigher = saveViewHigher + deltaView;
               ClipView();
            }
         }
         else
         {
            if (inScrollBar && ImGui::IsMouseClicked(0))
            {
               movingScrollBarSvg = true;
               scrollingSource = io.MousePos[componentIndex];
               saveViewLower = viewLower;
               saveViewHigher = viewHigher;
               editingId = currentId;
            }
            if (!sizingRBar && onRight && ImGui::IsMouseClicked(0) && hasAnchors)
            {
               sizingRBarSvg = true;
               editingId = currentId;
            }
            if (!sizingLBar && onLeft && ImGui::IsMouseClicked(0) && hasAnchors)
            {
               sizingLBarSvg = true;
               editingId = currentId;
            }
         }
      }

      // minimal size check
      if ((viewHigher - viewLower) < viewMinSize)
      {
         const float middle = (viewLower + viewHigher) * 0.5f;
         viewLower = middle - viewMinSize * 0.5f;
         viewHigher = middle + viewMinSize * 0.5f;
         ClipView();
      }

      return movingScrollBar || sizingRBar || sizingLBar || interacted;
   }

} // namespace
