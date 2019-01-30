#pragma once

#include <cstdint>

// because the SSE structures dont match the game size
// #define VECTORMATH_FORCE_SCALAR_MODE 1
#include <vectormath/vectormath.hpp>


using Vector3f = Vector3;
using Vector4f = Vector4;
using Matrix3x3f = Matrix3;
using Matrix3x4f = Matrix3; // because vector3s have a w component.
using Matrix4x4f = Matrix4;

#include "ReClass_Internal.hpp"
#include "REString.hpp"
#include "RETransform.hpp"