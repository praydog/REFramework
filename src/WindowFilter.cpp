#include "WindowFilter.hpp"

// To prevent usage of statics (TLS breaks the present thread...?)
WindowFilter g_window_filter{};

WindowFilter& WindowFilter::get() {
    return g_window_filter;
}

WindowFilter::WindowFilter() {
    // We create a job thread because GetWindowTextA can actually deadlock inside
    // the present thread...
    m_job_thread = std::make_unique<std::jthread>([this](std::stop_token s){
        while (!s.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds{100});

            if (m_window_jobs.empty()) {
                return;
            }

            std::vector<HWND> window_jobs{};

            {
                std::scoped_lock _{m_mutex};
                window_jobs = std::move(m_window_jobs);
            }

            for (const auto hwnd : window_jobs) {
                if (is_filtered_nocache(hwnd)) {
                    filter_window(hwnd);
                }
            }
        }
    });
}

WindowFilter::~WindowFilter() {
    m_job_thread->request_stop();
    m_job_thread->join();
}

bool WindowFilter::is_filtered(HWND hwnd) {
    if (hwnd == nullptr) {
        return true;
    }
    
    std::scoped_lock _{m_mutex};

    if (m_filtered_windows.find(hwnd) != m_filtered_windows.end()) {
        return true;
    }

    // if we havent even seen this window yet, add it to the job queue
    // and return true;
    if (m_seen_windows.find(hwnd) == m_seen_windows.end()) {
        m_seen_windows.insert(hwnd);
        m_window_jobs.push_back(hwnd);
        return true;
    }

    return false;
}

bool WindowFilter::is_filtered_nocache(HWND hwnd) {
    // get window name
    char window_name[256]{};
    GetWindowTextA(hwnd, window_name, sizeof(window_name));

    const auto sv = std::string_view{window_name};

    if (sv.find("UE4SS") != std::string_view::npos) {
        return true;
    }

    if (sv.find("PimaxXR") != std::string_view::npos) {
        return true;
    }

    // TODO: more problematic windows
    return false;
}