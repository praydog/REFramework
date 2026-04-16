#pragma once

namespace sdk {
namespace memory {
void* allocate(size_t size, bool zero_memory = true);
void deallocate(void* ptr);

void* reallocate(void* ptr, size_t old_size, size_t size);

namespace detail {
void* allocate_plugin_loader(size_t size);
}
}
}