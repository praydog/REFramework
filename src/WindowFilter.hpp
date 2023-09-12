#pragma once

#include <Windows.h>

#include <string_view>
#include <unordered_set>
#include <thread>
#include <vector>
#include <mutex>

class WindowFilter {
public:
    static WindowFilter& get();

public:
    WindowFilter();
    virtual ~WindowFilter();

    bool is_filtered(HWND hwnd);

    void filter_window(HWND hwnd) {
        std::scoped_lock _{m_mutex};
        m_filtered_windows.insert(hwnd);
    }

private:
    bool is_filtered_nocache(HWND hwnd);

    std::recursive_mutex m_mutex{};
    std::vector<HWND> m_window_jobs{};
    std::unique_ptr<std::jthread> m_job_thread{};

    std::unordered_set<HWND> m_seen_windows{};
    std::unordered_set<HWND> m_filtered_windows{};
};