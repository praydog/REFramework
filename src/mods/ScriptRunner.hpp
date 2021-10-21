#pragma once

#include <unordered_map>

#include <sol/sol.hpp>

#include "Mod.hpp"

class ScriptState {
public:
    ScriptState();

    void run_script(const std::string& p);

    void on_pre_application_entry(const char* name);
    void on_application_entry(const char* name);

private:
    sol::state m_lua{};
    std::unordered_multimap<std::string, sol::function> m_pre_application_entry_fns{};
    std::unordered_multimap<std::string, sol::function> m_application_entry_fns{};
};

class ScriptRunner : public Mod {
public:
    std::string_view get_name() const override { return "ScriptRunner"; }

    void on_draw_ui() override;
    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;

private:
    ScriptState m_state{};
};

