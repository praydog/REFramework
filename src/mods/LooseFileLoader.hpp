#pragma once

#include <utility/FunctionHook.hpp>

#include "../Mod.hpp"

class LooseFileLoader : public Mod {
public:
    static std::shared_ptr<LooseFileLoader>& get();

public:
    std::string_view get_name() const override { return "LooseFileLoader"; }

    std::optional<std::string> on_initialize() override;
    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;
    
    void on_frame() override;
    void on_draw_ui() override;

private:
    void hook();
    static uint32_t path_to_hash_hook(const wchar_t* path);

    bool m_hook_success{false};
    bool m_attempted_hook{false};
    uint32_t m_files_encountered{};
    uint32_t m_loose_files_loaded{};

    std::unique_ptr<FunctionHook> m_path_to_hash_hook{nullptr};

    ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled")) };

    ValueList m_options{
        *m_enabled
    };
};