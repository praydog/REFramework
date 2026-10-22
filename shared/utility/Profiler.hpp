#pragma once

#include <shared_mutex>
#include <string_view>
#include <chrono>
#include <unordered_map>
#include <source_location>

//#define REF_ENABLE_PROFILER

namespace utility{
class Profiler {
public:
    struct ProfileData {
        std::chrono::nanoseconds total{};
        std::chrono::nanoseconds min{std::chrono::nanoseconds::max()};
        std::chrono::nanoseconds max{};
        std::chrono::nanoseconds last{};
    };

    static ProfileData get_data(std::string_view name) {
        std::shared_lock _{s_mutex};
        return s_data[name];
    }

    static std::unordered_map<std::string_view, ProfileData> get_all_data() {
        std::shared_lock _{s_mutex};
        return s_data;
    }

public:
    Profiler(std::string_view name) 
        : m_name{name}
    {

    }

    virtual ~Profiler() {
        std::unique_lock _{s_mutex};
        auto& data = s_data[m_name];

        const auto end = std::chrono::high_resolution_clock::now();
        const auto duration = end - m_start_time;

        data.total += duration;
        data.last = duration;

        if (duration < data.min) {
            data.min = duration;
        }

        if (duration > data.max) {
            data.max = duration;
        }
    }

private:
    static inline std::shared_mutex s_mutex{};
    static inline std::unordered_map<std::string_view, ProfileData> s_data{};

    std::string_view m_name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time{std::chrono::high_resolution_clock::now()};
};
}

#ifdef REF_ENABLE_PROFILER
#define REF_PROFILE(name) utility::Profiler __ref_profiler__{name};
#define REF_PROFILE_FUNCTION() REF_PROFILE(std::source_location::current().function_name())
#else
#define REF_PROFILE(name)
#define REF_PROFILE_FUNCTION()
#endif