#pragma once

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <vector>

namespace utility::d3d12 {
// Independently owned by the active hook and each in-flight callback. Recording
// never waits for Present or teardown; a disabled generation cannot be reused.
template<class Entry> class SubmittedQueueCaptureState {
public:
    template<class Record> bool try_record(Record record) {
        std::unique_lock lock{m_mutex, std::try_to_lock};
        if (!lock.owns_lock() || !m_enabled) {
            return false;
        }
        record(m_entries);
        return true;
    }

    std::vector<Entry> snapshot() const {
        std::scoped_lock lock{m_mutex};
        return m_entries;
    }

    void disable() {
        std::vector<Entry> retired;
        {
            std::scoped_lock lock{m_mutex};
            m_enabled = false;
            retired.swap(m_entries);
        }
        // In particular, release COM references outside the recording mutex.
    }

private:
    mutable std::mutex m_mutex;
    bool m_enabled{true};
    std::vector<Entry> m_entries;
};

template<class Queue> struct QueueSelection {
    Queue* queue{};
    std::size_t matching_device{};
    std::size_t matching_thread{};
    bool retained{};
};

// A queue already proven for this swapchain remains valid when auxiliary
// queues begin submitting. Initial selection must still be unambiguous.
template<class Queue, class Entries, class MatchesDevice>
QueueSelection<Queue> select_submitted_queue(Queue* captured, bool same_swapchain,
    const Entries& entries, std::uint32_t current_thread, MatchesDevice matches_device) {
    if (captured != nullptr && same_swapchain && matches_device(captured)) {
        return {captured, 0, 0, true};
    }

    QueueSelection<Queue> result;
    Queue* sole{};
    Queue* same_thread{};
    for (const auto& entry : entries) {
        auto* queue = entry.queue.Get();
        if (!matches_device(queue)) {
            continue;
        }
        sole = queue;
        ++result.matching_device;
        if (entry.thread_id == current_thread) {
            same_thread = queue;
            ++result.matching_thread;
        }
    }
    result.queue = result.matching_thread == 1 ? same_thread :
        (result.matching_device == 1 ? sole : nullptr);
    return result;
}
}
