#pragma once

#include <memory>
#include <vector>

namespace utility {
struct ThreadState {
    uint32_t thread_id{0};
    bool suspended{false};

    using Ptr = std::unique_ptr<ThreadState>;
};

using ThreadStates = std::vector<ThreadState::Ptr>;

ThreadStates suspend_threads();
void resume_threads(const ThreadStates& states);

struct ThreadSuspender {
    ThreadSuspender();

    virtual ~ThreadSuspender();

    void suspend() {
        states = suspend_threads();
    }

    void resume();

    ThreadStates states{};
};
}