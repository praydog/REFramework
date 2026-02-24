#pragma once

#include <map>
#include <shared_mutex>
#include <string>

#include "Mod.hpp"

class MethodDatabase : public Mod {
public:
    static std::shared_ptr<MethodDatabase>& get();

    std::string_view get_name() const override { return "MethodDatabase"; }
    std::optional<std::string> on_initialize() override;

    std::string find_method(uintptr_t addr) const;

private:
    static std::string resolve_address(uintptr_t addr);

    mutable std::shared_mutex m_mutex{};
    std::map<uintptr_t, std::string> m_method_map{};
    size_t m_build_time_ms{0};
    size_t m_estimated_ram{0};
};
