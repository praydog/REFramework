#include <cassert>
#include <iostream>
#include <vector>
#include <future>
#include <memory>
#include <thread>
#include "utility/SubmittedQueueSelection.hpp"
struct Queue { int device; bool valid=true; };
struct Ref { Queue* q; Queue* Get() const { return q; } };
struct Entry { Ref queue; unsigned thread_id; };

static void test_capture_lifetime() {
    using State = utility::d3d12::SubmittedQueueCaptureState<int>;
    auto capture = std::make_shared<State>();
    // The real callback takes a retained state snapshot before forwarding the
    // submission. Teardown may run while the original submission is in flight.
    std::shared_ptr<State> active = capture;
    std::promise<void> submitted;
    std::promise<void> resume;
    auto resume_future = resume.get_future();
    std::thread callback{[&]() {
        auto old = std::atomic_load(&active);
        submitted.set_value();
        resume_future.wait();
        const bool recorded = old->try_record([](auto& entries) { entries.push_back(1); });
        assert(!recorded);
    }};
    submitted.get_future().wait();
    std::atomic_store(&active, std::shared_ptr<State>{});
    capture->disable();
    auto replacement = std::make_shared<State>();
    std::atomic_store(&active, replacement);
    resume.set_value();
    callback.join();
    assert(capture->snapshot().empty() && replacement->snapshot().empty());
    assert(replacement->try_record([](auto& entries) { entries.push_back(2); }));
    assert(replacement->snapshot() == std::vector<int>{2});

    // A concurrent snapshot/teardown must not make the submitting worker wait
    // behind a mutex; exercise the same try_record implementation as production.
    std::promise<void> held;
    std::promise<void> unlock;
    auto unlock_future = unlock.get_future();
    std::thread holder{[&]() {
        replacement->try_record([&](auto&) {
            held.set_value();
            unlock_future.wait();
        });
    }};
    held.get_future().wait();
    assert(!replacement->try_record([](auto&) { assert(false); }));
    unlock.set_value();
    holder.join();
    replacement->disable();
    assert(replacement->snapshot().empty());
    assert(!replacement->try_record([](auto&) { assert(false); }));
}

int main() {
    test_capture_lifetime();
    Queue game{1}, aux_a{1}, aux_b{1}, other{2}, invalid{1, false};
    auto match = [](Queue* q) { return q->valid && q->device==1; };
    std::vector<Entry> entries{{{&game},10}};
    auto r=utility::d3d12::select_submitted_queue((Queue*)nullptr,false,entries,10,match);
    assert(r.queue==&game && !r.retained);
    entries.push_back({{&aux_a},10}); entries.push_back({{&aux_b},10});
    // RE8 regression: two VR helper queues appear after initial game Present.
    r=utility::d3d12::select_submitted_queue(&game,true,entries,10,match);
    assert(r.queue==&game && r.retained);
    // Initial ambiguous queues and a fresh/reset swapchain cannot reuse a pin.
    r=utility::d3d12::select_submitted_queue((Queue*)nullptr,false,entries,10,match);
    assert(r.queue==nullptr && r.matching_device==3 && r.matching_thread==3);
    r=utility::d3d12::select_submitted_queue(&game,false,entries,10,match);
    assert(r.queue==nullptr);
    // A device change invalidates the old pin; only the new device may win.
    entries={{{&other},10}};
    auto match2=[](Queue* q) { return q->valid && q->device==2; };
    r=utility::d3d12::select_submitted_queue(&game,true,entries,10,match2);
    assert(r.queue==&other && !r.retained);
    // Failed GetDevice is not a valid cache hit.
    entries.clear();
    r=utility::d3d12::select_submitted_queue(&invalid,true,entries,10,match);
    assert(r.queue==nullptr);
    // Preserve existing unique-thread selection among device-compatible queues.
    entries={{{&game},10},{{&aux_a},11},{{&other},10}};
    r=utility::d3d12::select_submitted_queue((Queue*)nullptr,false,entries,10,match);
    assert(r.queue==&game && r.matching_device==2 && r.matching_thread==1);
    // Even a different submitting thread does not invalidate an established pin.
    r=utility::d3d12::select_submitted_queue(&game,true,entries,11,match);
    assert(r.queue==&game && r.retained);
    std::cout << "PASS: capture teardown/generation/contention, initial selection, RE8 auxiliary queues, reset/swapchain change, device change, failed identity, thread matching\n";
}
