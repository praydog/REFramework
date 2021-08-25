#include <unordered_set>
#include <iomanip>

#include "utility/Module.hpp"
#include "utility/Scan.hpp"

#include "sdk/RETypeDB.hpp"

#include "IntegrityCheckBypass.hpp"

struct IntegrityCheckPattern {
    std::string pat{};
    uint32_t offset{};
};

std::optional<std::string> IntegrityCheckBypass::on_initialize() {
    // Patterns for assigning or accessing of the integrity check boolean (RE3)
    // and for jumping past the integrity checks (RE8)
    // In RE8, the integrity checks cause a noticeable stutter as well.
    std::vector<IntegrityCheckPattern> possible_patterns {
#ifdef RE3
        /*
        cmp     qword ptr [rax+18h], 0
        cmovz   ecx, r15d
        mov     cs:bypass_integrity_checks, cl*/
        // Referenced above "steam_api64.dll"
        {"48 ? ? 18 00 41 ? ? ? 88 0D ? ? ? ?", 11}, 
        {"48 ? ? 18 00 0F ? ? 88 0D ? ? ? ? 49 ? ? ? 48", 10},
#elif defined(RE8)
        /*
        These are partially obfuscated and are within protected sections.
        The ja jumps past the checksum checks which cause very large stutters if they are ran.
        We'll replace the ja to always jump past the checksum checks.

        There are various patterns here because the code is obfuscated, there's an element of randomness per update.
        Lots of random junk code. Some instructions are obfuscated into multiple instructions as well.
        We're taking a shot in the dark here hoping that the obfuscated code
        stays generally the same past a game update.
        */

        /*
        sub     eax, ecx
        ja      NO_CHECKSUM_CHECKS1
        mov     eax, [rsp+whatever]
        */
        // app.PlayerCore.onDamage, app.EnemyCore.onDie2 (onDie2 gets called from onDie)
        {"29 c8 0f 87 ? ? ? ? 8b 84", 2},

        /*
        sub     eax, ecx
        ja      NO_CHECKSUM_CHECKS2
.       xor     eax, eax
        sub     eax, [rsp+whatever]
        */
        // app.PlayerCore.onDamage #2
        {"29 c8 0f 87 ? ? ? ? 31 C0 2B", 2},

        /*
        mov     eax, [rsp+whatever]
        sub     eax, ecx
        ja      NO_CHECKSUM_CHECKS3
        xor     eax, eax
        */
        // app.PlayerCore.onDamage #3, app.EnemyCore.onDie2 #2
        {"8b 84 ? ? ? ? ? 29 c8 0f 87 ? ? ? ?", 9},
        /* 
        There is another one inside of app.GlobalService.msgSceneTransition_afterDeactivate
        but didn't bother to patch it out. Reason being that it seems to only get called when loading is finished. 
        Maybe some more investigation is required here?
        */
        // The above function names can be found within il2cpp_dump.json, which is dumped with REFramework's "Dump SDK" button in developer mode.
#endif
    };

    std::unordered_set<uintptr_t> already_patched{};

    const auto module_size = *utility::get_module_size(g_framework->get_module().as<HMODULE>());
    const auto module_end = g_framework->get_module() + module_size;

    for (auto& possible_pattern : possible_patterns) {
        auto integrity_check_ref = utility::scan(g_framework->get_module().as<HMODULE>(), possible_pattern.pat);

        if (!integrity_check_ref) {
            continue;
        }

#if defined(RE3)
        m_bypass_integrity_checks = (bool*)utility::calculate_absolute(*integrity_check_ref + possible_pattern.offset);
#elif defined(RE8)
        while (integrity_check_ref) {
            const auto ja_instruction = *integrity_check_ref + possible_pattern.offset;

            if (already_patched.contains(ja_instruction)) {
                spdlog::info("IntegrityCheckBypass: ja instruction at 0x{:X} already patched, continuing...", ja_instruction);
                integrity_check_ref =
                    utility::scan(*integrity_check_ref + 1, module_end - (*integrity_check_ref + 1), possible_pattern.pat);
                continue;
            }

            // Create a ja->jmp patch for bypassing the integrity check
            std::vector<uint8_t> patch_bytes{0xE9, 0x00, 0x00, 0x00, 0x00, 0x90};

            // Overwrite the target address with the original ja target. Add 1 byte because the new instruction is smaller.
            *(uint32_t*)&patch_bytes[1] = *(uint32_t*)(ja_instruction + 2) + 1;

            // Convert the uint8_t patch_bytes to int16_t vector
            std::vector<int16_t> patch_int16_bytes{};

            for (auto& patch_byte : patch_bytes) {
                patch_int16_bytes.push_back(patch_byte);
            }

            // Log the patch address (ja_instruction) and bytes with spdlog
            spdlog::info("Patch address: 0x{:X}", ja_instruction);

            // Convert patch_bytes to hex string with stringstream and then log the string with spdlog
            std::stringstream ss;
            ss << std::hex << std::setfill('0');
            for (auto& patch_byte : patch_bytes) {
                ss << std::setw(2) << (int)patch_byte << " ";
            }

            spdlog::info("Patch bytes: {}", ss.str());

            // Patch the bytes
            m_patches.emplace_back(Patch::create(ja_instruction, patch_int16_bytes));
            already_patched.emplace(ja_instruction);

            // Search for the next integrity check using the same pattern
            integrity_check_ref = utility::scan(*integrity_check_ref + 1, module_end - (*integrity_check_ref + 1), possible_pattern.pat);
        }

        // If we didn't find any integrity checks
        if (m_patches.empty()) {
            spdlog::info("Could not find any integrity checks to bypass!");
        }
#endif
    }

    // These may be removed, so don't fail altogether
    /*if (m_bypass_integrity_checks == nullptr) {
        return "Failed to find IntegrityCheckBypass pattern";
    }*/

#ifdef RE3
    spdlog::info("[{:s}]: bypass_integrity_checks: {:x}", get_name().data(), (uintptr_t)m_bypass_integrity_checks);
#endif

    return Mod::on_initialize();
}

void IntegrityCheckBypass::on_frame() {
#ifdef RE3
    if (m_bypass_integrity_checks != nullptr) {
        *m_bypass_integrity_checks = true;
    }
#endif

#ifdef RE8
    // These three are responsible for various stutters and
    // gameplay altering effects e.g. not being able to interact with objects
    disable_update_timers("app.InteractManager");
    disable_update_timers("app.EnemyManager");
    disable_update_timers("app.GUIManager");
#endif
}

#ifdef RE8
void IntegrityCheckBypass::disable_update_timers(const std::string& name) const {
    // get the singleton correspdonding to the given name
    auto manager = g_framework->get_globals()->get<REManagedObject>(name);

    // If the interact manager is null, we're probably not in the game
    if (manager == nullptr || manager->info == nullptr || manager->info->classInfo == nullptr) {
        return;
    }

    // Get the sdk::RETypeDefinition of the manager
    auto t = (sdk::RETypeDefinition*)manager->info->classInfo;

    if (t == nullptr) {
        return;
    }

    // Get the update timer fields, which are responsible for disabling interactions (for app.InteractManager)
    // if the integrity checks are triggered
    auto update_timer_enable_field = t->get_field("UpdateTimerEnable");
    auto update_timer_late_enable_field = t->get_field("LateUpdateTimerEnable");

    if (update_timer_enable_field == nullptr || update_timer_late_enable_field == nullptr) {
        return;
    }

    // Get the actual field data now within the manager
    auto& update_timer_enable = update_timer_enable_field->get_data<bool>(manager, true);
    auto& update_timer_late_enable = update_timer_late_enable_field->get_data<bool>(manager, true);

    // Log that we are about to set these to false if they were true before
    if (update_timer_enable) {
        spdlog::info("[{:s}]: {:s}.UpdateTimerEnable was true, disabling it...", get_name().data(), name.data());
    }

    if (update_timer_late_enable) {
        spdlog::info("[{:s}]: {:s}.LateUpdateTimerEnable was true, disabling it...", get_name().data(), name.data());
    }

    // Set the fields to false
    update_timer_enable = false;
    update_timer_late_enable = false;
}
#endif
