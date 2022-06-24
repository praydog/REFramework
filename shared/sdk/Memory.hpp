#pragma once

namespace sdk {
namespace via {
namespace memory {
void* allocate(size_t size);
void deallocate(void* ptr);
}
}
}