#include "utility/Scan.hpp"

#include "IntegrityCheckBypass.hpp"

struct IntegrityCheckPattern {
    std::string pat{};
    uint32_t offset{};
};

std::optional<std::string> IntegrityCheckBypass::on_initialize() {
    // Patterns for assigning or accessing of the integrity check boolean
    std::vector<IntegrityCheckPattern> possible_patterns{
        /*
        cmp     qword ptr [rax+18h], 0
        cmovz   ecx, r15d
        mov     cs:bypass_integrity_checks, cl*/
        // Referenced above "steam_api64.dll"
        {"48 ? ? 18 00 41 ? ? ? 88 0D ? ? ? ?", 11},
        {"48 ? ? 18 00 0F ? ? 88 0D ? ? ? ? 49 ? ? ? 48", 10},
    };

    for (auto& possible_pattern : possible_patterns) {
        auto integrity_check_ref = utility::scan(g_framework->get_module().as<HMODULE>(), possible_pattern.pat);

        if (!integrity_check_ref) {
            continue;
        }

        m_bypass_integrity_checks = (bool*)utility::calculate_absolute(*integrity_check_ref + possible_pattern.offset);
    }

    // These may be removed, so don't fail altogether
    /*if (m_bypass_integrity_checks == nullptr) {
        return "Failed to find IntegrityCheckBypass pattern";
    }*/

    spdlog::info("[{:s}]: bypass_integrity_checks: {:x}", get_name().data(), (uintptr_t)m_bypass_integrity_checks);

    return Mod::on_initialize();
}

void IntegrityCheckBypass::on_frame() {
    if (m_bypass_integrity_checks != nullptr) {
        *m_bypass_integrity_checks = true;
    }
}
