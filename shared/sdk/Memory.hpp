#pragma once

namespace sdk {
namespace memory {
void* allocate(size_t size);
void deallocate(void* ptr);
}
}