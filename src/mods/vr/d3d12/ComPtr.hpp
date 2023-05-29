#pragma once

#include <wrl.h>

namespace d3d12 {
template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
}