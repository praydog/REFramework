#include <mutex>

#include <Windows.h>
#include <TlHelp32.h>
#include <spdlog/spdlog.h>

#include "Thread.hpp"

namespace utility {
namespace detail {
std::mutex g_suspend_mutex{};
}

ThreadStates suspend_threads() {
    ThreadStates states{};

    const auto pid = GetCurrentProcessId();
    const auto snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);

    if (snapshot_handle == nullptr || snapshot_handle == INVALID_HANDLE_VALUE) {
        return states;
    }

    THREADENTRY32 te{};
    te.dwSize = sizeof(THREADENTRY32);

    if (!Thread32First(snapshot_handle, &te)) {
        CloseHandle(snapshot_handle);
        return states;
    }

    const auto current_thread_id = GetCurrentThreadId();

    do {
        if (te.th32OwnerProcessID == pid && te.th32ThreadID != current_thread_id) {
            auto thread_handle = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);

            if (thread_handle != nullptr && snapshot_handle != INVALID_HANDLE_VALUE) {
                auto state = std::make_unique<ThreadState>();

                spdlog::info("Suspending {}", (uint32_t)te.th32ThreadID);

                state->thread_id = te.th32ThreadID;
                state->suspended = SuspendThread(thread_handle) > 0;
                states.emplace_back(std::move(state));

                CloseHandle(thread_handle);
            }
        }
    } while (Thread32Next(snapshot_handle, &te));

    CloseHandle(snapshot_handle);
    return states;
}

void resume_threads(const ThreadStates& states) {
    for (const ThreadState::Ptr& state : states) {
        auto thread_handle = OpenThread(THREAD_SUSPEND_RESUME, FALSE, state->thread_id);

        if (thread_handle != nullptr) {
            spdlog::info("Resuming {}", state->thread_id);

            ResumeThread(thread_handle);
            CloseHandle(thread_handle);
        }
    }
}

ThreadSuspender::ThreadSuspender()  {
    detail::g_suspend_mutex.lock();
    states = suspend_threads();
}

ThreadSuspender::~ThreadSuspender() {
    resume_threads(states);
    detail::g_suspend_mutex.unlock();
}

void ThreadSuspender::resume() {
    resume_threads(states);
    states.clear();
}
}
