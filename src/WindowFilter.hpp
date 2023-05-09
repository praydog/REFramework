#pragma once

#include <mutex>
#include <Windows.h>

#include <string_view>
#include <unordered_set>

class WindowFilter {
public:
    static WindowFilter& get() {
        static WindowFilter instance{};
        return instance;
    }

public:
    bool is_filtered(HWND hwnd) {
        if (hwnd == nullptr) {
            return true;
        }

        std::scoped_lock _{m_mutex};

        if (m_filtered_windows.find(hwnd) != m_filtered_windows.end()) {
            return true;
        }

        if (m_seen_windows.find(hwnd) != m_seen_windows.end()) {
            return false;
        }

        m_seen_windows.insert(hwnd);

        if (is_filtered_nocache(hwnd)) {
            filter_window(hwnd);
            return true;
        }

        return false;
    }

private:
    void filter_window(HWND hwnd) {
        m_filtered_windows.insert(hwnd);
    }

    bool is_filtered_nocache(HWND hwnd) {
        // get window name
        char window_name[256]{};
        GetWindowTextA(hwnd, window_name, sizeof(window_name));

        const auto sv = std::string_view{window_name};

        if (sv.find("PimaxXR") != std::string_view::npos) {
            return true;
        }

        // TODO: more problematic windows
        return false;
    }

    std::mutex m_mutex{};
    std::unordered_set<HWND> m_seen_windows{};
    std::unordered_set<HWND> m_filtered_windows{};
};