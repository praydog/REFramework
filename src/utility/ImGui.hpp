#pragma once

#include <optional>
#include <imgui.h>
#include <sdk/ReClass.hpp>

namespace imgui {
std::optional<Vector3f> get_camera_up();
void draw_sphere(const Vector3f& center, float radius, ImU32 color, bool outline = true);
void draw_capsule(const Vector3f& start, const Vector3f& end, float radius, ImU32 color, bool outline = true);
}