#include <chrono>
#include <spdlog/spdlog.h>

#include <sdk/RETypeDB.hpp>

#include "utility/Exceptions.hpp"
#include "utility/Scan.hpp"

#include "MethodDatabase.hpp"

std::shared_ptr<MethodDatabase>& MethodDatabase::get() {
    static auto instance = std::make_shared<MethodDatabase>();
    return instance;
}

std::string MethodDatabase::resolve_address(uintptr_t addr) {
    return get()->find_method(addr);
}

std::optional<std::string> MethodDatabase::on_initialize() {
    spdlog::info("[MethodDatabase] Building method address map...");

    const auto start = std::chrono::steady_clock::now();

    auto tdb = sdk::RETypeDB::get();

    if (tdb == nullptr) {
        return "MethodDatabase: RETypeDB not available";
    }

    const auto num_methods = tdb->get_num_methods();
    size_t thunks = 0;

    {
        std::unique_lock lock{m_mutex};

        for (uint32_t i = 0; i < num_methods; ++i) {
            auto method = tdb->get_method(i);

            if (method == nullptr) {
                continue;
            }

            auto func = method->get_function();

            if (func == nullptr) {
                continue;
            }

            auto declaring_type = method->get_declaring_type();

            if (declaring_type == nullptr) {
                continue;
            }

            try {
                auto full_name = declaring_type->get_full_name();
                auto method_name = method->get_name();

                if (method_name == nullptr) {
                    continue;
                }

                full_name += ".";
                full_name += method_name;

                const auto func_addr = (uintptr_t)func;
                m_method_map[func_addr] = full_name;

                // If the function starts with an E9 jmp, also map the jump target
                if (*(uint8_t*)func == 0xE9) {
                    const auto target = utility::calculate_absolute((uintptr_t)func + 1);
                    m_method_map[target] = full_name;
                    ++thunks;
                }
            } catch (...) {
                continue;
            }
        }
    }

    const auto end = std::chrono::steady_clock::now();
    m_build_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Estimate RAM: each std::map node has ~48 bytes overhead (red-black tree node with 3 pointers + color + key + std::string object)
    // plus the actual string heap allocation for strings > SSO threshold
    constexpr size_t map_node_overhead = sizeof(uintptr_t) + sizeof(std::string) + sizeof(void*) * 3 + sizeof(int);
    m_estimated_ram = 0;
    for (const auto& [addr, name] : m_method_map) {
        m_estimated_ram += map_node_overhead + (name.size() > 22 ? name.size() + 1 : 0);
    }

    spdlog::info("[MethodDatabase] Built map with {} methods in {}ms (~{:.1f} MB RAM)", m_method_map.size(), m_build_time_ms, m_estimated_ram / (1024.0 * 1024.0));
    spdlog::info("[MethodDatabase] Thunks found: {}", thunks);

    utility::exceptions::set_address_name_resolver(&MethodDatabase::resolve_address);

    return Mod::on_initialize();
}

std::string MethodDatabase::find_method(uintptr_t addr) const {
    std::shared_lock lock{m_mutex};

    if (m_method_map.empty()) {
        return {};
    }

    // Try to resolve the function start from the unwind info first
    const auto func_start = utility::find_function_start_unwind(addr);

    if (func_start) {
        auto it = m_method_map.find(*func_start);

        if (it != m_method_map.end()) {
            return it->second;
        }
    }

    // Fall back to upper_bound lookup
    auto it = m_method_map.upper_bound(addr);

    if (it == m_method_map.begin()) {
        return {};
    }

    --it;

    constexpr uintptr_t max_method_size = 0x10000;

    if (addr - it->first > max_method_size) {
        return {};
    }

    return it->second;
}
