#include "REDelegate.hpp"

namespace sdk {
void Delegate::invoke() {
    auto ctx = sdk::get_thread_context();
    for (uint32_t i = 0; i < num_methods; i++) {
        auto& method = methods[i];
        if (method.func == nullptr) {
            continue;
        }
        
        if (method.object != nullptr) {
            method.func(ctx, method.object);
        } else {
            method.func(ctx, nullptr);
        }
    }
}
}