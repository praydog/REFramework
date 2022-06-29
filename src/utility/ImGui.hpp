#pragma once

#include <optional>
#include <imgui.h>
#include <ImGuizmo.h>
#include <sdk/ReClass.hpp>

namespace imgui {
bool draw_gizmo(Matrix4x4f& mat, IMGUIZMO_NAMESPACE::OPERATION op = IMGUIZMO_NAMESPACE::OPERATION::UNIVERSAL, IMGUIZMO_NAMESPACE::MODE mode = IMGUIZMO_NAMESPACE::MODE::WORLD);
void draw_cube(const Matrix4x4f& mat);
void draw_grid(const Matrix4x4f& mat, float size);

std::optional<Vector3f> get_camera_up();
void draw_sphere(const Vector3f& center, float radius, ImU32 color, bool outline = true);
void draw_capsule(const Vector3f& start, const Vector3f& end, float radius, ImU32 color, bool outline = true);
}