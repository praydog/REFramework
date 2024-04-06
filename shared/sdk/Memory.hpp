#pragma once

namespace sdk {
namespace memory {
void* allocate(size_t size);
void deallocate(void* ptr);

void* reallocate(void* ptr, size_t old_size, size_t size);
}
}