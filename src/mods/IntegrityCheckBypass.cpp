#include <unordered_set>
#include <iomanip>

#include "utility/Module.hpp"
#include "utility/Scan.hpp"

#include "sdk/RETypeDB.hpp"

#include "Hooks.hpp"

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
        spdlog::info("Scanning for {}", possible_pattern.pat);

        auto integrity_check_ref = utility::scan(g_framework->get_module().as<HMODULE>(), possible_pattern.pat);

        if (!integrity_check_ref) {
            continue;
        }

#if defined(RE3)
        m_bypass_integrity_checks = (bool*)utility::calculate_absolute(*integrity_check_ref + possible_pattern.offset);
#elif defined(RE8)
        ignore_application_entries();

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

#ifdef MHRISE
    // this is pretty much what it was like finding this, you just gotta look a little closer!
    const auto very_cool_type = sdk::find_type_definition_by_fqn(0x83f09f47);
    static std::vector<Patch::Ptr> very_cool_patches{};

    auto find_method_by_hash = [](sdk::RETypeDefinition* t, size_t hash) -> sdk::REMethodDefinition* {
        for (auto& method : t->get_methods()) {
            if (utility::hash(method.get_name()) == hash) {
                return &method;
            }
        }

        return nullptr;
    };

    if (very_cool_type != nullptr) {
        auto patch_very_cool_method = [&](size_t hash) {
            const auto method = find_method_by_hash(very_cool_type, hash);

            if (method == nullptr) {
                spdlog::error("Could not find very cool method", hash);
                return false;
            }

            if (method->get_function() == nullptr) {
                spdlog::error("[{:s}]: Could not find very_cool_type::very_cool_method!", get_name().data());
                return false;
            }

            spdlog::info("[{:s}]: Patching very cool method!", get_name().data());
        
            very_cool_patches.emplace_back(Patch::create((uintptr_t)method->get_function(), { 0xB0, 0x00, 0xC3 }, true));

            return true;
        };

        patch_very_cool_method(0x21c27632fa7ba29b);
        patch_very_cool_method(0x49b943a462e8cf6a);
    } else {
        spdlog::error("[{:s}]: Could not find very_cool_type!", get_name().data());
    }

    const auto very_awesome_type = sdk::find_type_definition_by_fqn(0xce04a0c6);

    if (very_awesome_type != nullptr) {
        const auto very_awesome_method = find_method_by_hash(very_awesome_type, 0x9f79221341cfcb18);

        if (very_awesome_method != nullptr) {
            if (very_awesome_method->get_function() == nullptr) {
                spdlog::error("[{:s}]: Could not find very_awesome_type::very_awesome_method!", get_name().data());
                return Mod::on_initialize();
            }

            const auto very_awesome_call = utility::scan_opcode((uintptr_t)very_awesome_method->get_function(), 10, 0xE8);

            if (!very_awesome_call) {
                spdlog::error("[{:s}]: Could not find very_awesome_call!", get_name().data());
                return Mod::on_initialize();
            }

            const auto real_awesome_function = utility::calculate_absolute(*very_awesome_call + 1);

            if (real_awesome_function == 0) {
                spdlog::error("[{:s}]: Could not find real_awesome_function!", get_name().data());
                return Mod::on_initialize();
            }

            spdlog::info("[{:s}]: Patching very awesome method!", get_name().data());

            very_cool_patches.emplace_back(Patch::create(real_awesome_function, { 0xB0, 0x00, 0xC3 }, true));
        } else {
            spdlog::error("[{:s}]: Could not find very_awesome_method!", get_name().data());
        }
    } else {
        spdlog::error("[{:s}]: Could not find very_awesome_type!", get_name().data());
    }
#endif

    spdlog::info("Done.");

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
    disable_update_timers("app.HIDManager");
    disable_update_timers("app.FadeManager");
#endif
}

#ifdef RE8
void IntegrityCheckBypass::disable_update_timers(std::string_view name) const {
    // get the singleton correspdonding to the given name
    auto manager = sdk::get_managed_singleton<::REManagedObject>(name);

    // If the interact manager is null, we're probably not in the game
    if (manager == nullptr || manager->info == nullptr || manager->info->classInfo == nullptr) {
        return;
    }

    // Get the sdk::RETypeDefinition of the manager
    auto t = utility::re_managed_object::get_type_definition(manager);

    if (t == nullptr) {
        return;
    }

    // Get the update timer fields, which are responsible for disabling interactions (for app.InteractManager)
    // if the integrity checks are triggered
    auto update_timer_enable_field = t->get_field("UpdateTimerEnable");
    auto update_timer_late_enable_field = t->get_field("LateUpdateTimerEnable");

    // Get the actual field data now within the manager
    if (update_timer_enable_field != nullptr) {
        auto& update_timer_enable = update_timer_enable_field->get_data<bool>(manager, true);

        // Log that we are about to set these to false if they were true before
        if (update_timer_enable) {
            spdlog::info("[{:s}]: {:s}.UpdateTimerEnable was true, disabling it...", get_name().data(), name.data());
        }

        update_timer_enable = false;
    }

    if (update_timer_late_enable_field != nullptr) {
        auto& update_timer_late_enable = update_timer_late_enable_field->get_data<bool>(manager, true);

        if (update_timer_late_enable) {
            spdlog::info("[{:s}]: {:s}.LateUpdateTimerEnable was true, disabling it...", get_name().data(), name.data());
        }

        update_timer_late_enable = false;
    }
}
#endif

void IntegrityCheckBypass::ignore_application_entries() {
    Hooks::get()->ignore_application_entry(0x76b8100bec7c12c3);
    Hooks::get()->ignore_application_entry(0x9f63c0fc4eea6626);
}

void IntegrityCheckBypass::immediate_patch_re8() {
    // We have to immediately patch this at startup in RE8 unlike MHRise
    // because the game immediately starts checking the integrity of the executable
    // on the first execution of this callback, unlike MHRise which was delayed.
    // So we can't use the IL2CPP metadata yet, we have to resort to
    // plain old pattern scanning.
    // We're essentially patching the application entries we ignored above, but immediately.
    spdlog::info("[IntegrityCheckBypass]: Scanning RE8...");

    const auto game = utility::get_executable();
    const auto game_size = utility::get_module_size(game).value_or(0);
    const auto game_end = (uintptr_t)game + game_size;

    // Present in MHRise and RE8.
    // sub rax, 128E329h
    const uint32_t sussy_constant = 0x128E329;
    std::optional<uintptr_t> sussy_result{};

    bool patched_sussy1 = false;

    for (sussy_result = utility::scan_data(game, (const uint8_t*)&sussy_constant, sizeof(sussy_constant)); 
         sussy_result.has_value(); 
         sussy_result = utility::scan_data(*sussy_result + 1, (game_end - (*sussy_result + 1)) - 0x100, (const uint8_t*)&sussy_constant, sizeof(sussy_constant)))
    {
        // Find the start of the instruction, given the sussy_constant is in the middle of it.
        const auto resolved_instruction = utility::resolve_instruction(*sussy_result);

        // If this instruction didn't get resolved, go onto the next one. We probably ran into garbage data.
        if (resolved_instruction) {
            const auto sussy_function_start = utility::find_function_start(resolved_instruction->addr);

            if (!sussy_function_start) {
                spdlog::error("[IntegrityCheckBypass]: Could not find function start for sussy_constant @ 0x{:x}", *sussy_result);
                continue;
            }

            // Create a patch that returns instantly.
            static auto patch = Patch::create(sussy_function_start.value(), { 0xC3 }, true);
            patched_sussy1 = true;
            spdlog::info("[IntegrityCheckBypass]: Patched sussy_function 1");
            break;
        }
    }

    if (!patched_sussy1) {
        spdlog::error("[IntegrityCheckBypass]: Could not find sussy_constant usage!");
    }

    // Now we need to patch the second callback.
    // I hope this constant isn't randomly generated by the protection!!!!!!
    /*
        call    ProtectionTripResult
        cmp     eax, 1F2h
        jz      ...
        lea     rcx, ProtectionGlobalContext
        call    ProtectionTripResult
    */
    const auto sussy_result_2 = utility::scan(game, "E8 ? ? ? ? 3D F2 01 00 00 0F 84 ? ? ? ? 48 8D 0D ? ? ? ? E8");

    if (sussy_result_2) {
        const auto sussy_function_start = utility::find_function_start(sussy_result_2.value());

        if (sussy_function_start) {
            static auto patch = Patch::create(sussy_function_start.value(), { 0xC3 }, true);
            spdlog::info("[IntegrityCheckBypass]: Patched sussy_function 2");
        }
    } else {
        spdlog::error("[IntegrityCheckBypass]: Could not find sussy_result_2!");
    }

    // These are embedded checks that run during startup and sometimes during loading transitions
    // they get passed different indices that make it perform different behavior
    // if it returns 1, the original execution flow gets altered
    // and stuff like DLC loading gets skipped so it needs to always return 0
    // there are really obvious constants to go off of within these functions
    // but they look like they might be auto generated so can't rely on them
    const auto sussy_result_3 = utility::scan(game, "8D ? 02 E8 ? ? ? ? 0F B6 C8 48 ? ? 50 48 ? ? 18 0F");

    if (sussy_result_3) {
        const auto func = utility::calculate_absolute(*sussy_result_3 + 4);
        static auto patch = Patch::create(func, { 0xB0, 0x00, 0xC3 }, true);
        spdlog::info("[IntegrityCheckBypass]: Patched sussy_function 3");
    } else {
        const auto sussy_result_alternative = utility::scan(game, "8D ? 05 E8 ? ? ? ? 0F B6 C8 48 ? ? 50 48 ? ? 18 0F");

        if (sussy_result_alternative) {
            const auto func = utility::calculate_absolute(*sussy_result_alternative + 4);
            static auto patch = Patch::create(func, { 0xB0, 0x00, 0xC3 }, true);
            spdlog::info("[IntegrityCheckBypass]: Patched sussy_function 3");
        } else {
            spdlog::error("[IntegrityCheckBypass]: Could not find sussy_result_3!");
        }
    }

    const auto sussy_result_4 = utility::scan(game, "72 ? 41 8B ? E8 ? ? ? ? 0F B6 C8 48 ? ? 50 48 ? ? 18 0F");

    if (sussy_result_4) {
        const auto func = utility::calculate_absolute(*sussy_result_4 + 6);
        static auto patch = Patch::create(func, { 0xB0, 0x00, 0xC3 }, true);
        spdlog::info("[IntegrityCheckBypass]: Patched sussy_function 4");
    } else {
        spdlog::error("[IntegrityCheckBypass]: Could not find sussy_result_4!");
    }
}

void IntegrityCheckBypass::immediate_patch_re4() {
    // This patch fixes the constant scans that are done every frame on the game's memory.
    // The scans will still be performed, but the crash will be avoided.
    // Ideally, the scans should be patched as well, because they are literally running every frame
    // which may cause performance issues.
    // As far as I can tell, this conditional jmp jumps into a via::clr::VM cleanup routine, corrupting memory.
    // This will cause the game to crash in a random location that accesses VM memory.
    // This is probably to make finding the code that causes the crash in the first place harder.
    spdlog::info("[IntegrityCheckBypass]: Scanning RE4...");

    const auto game = utility::get_executable();
    const auto conditional_jmp_block = utility::scan(game, "48 8B 8D D0 03 00 00 48 29 C1 75 ?");

    if (!conditional_jmp_block) {
        spdlog::error("[IntegrityCheckBypass]: Could not find conditional_jmp, trying fallback.");

        // mov     [rbp+192h], al
        // this is used shortly after the conditional jmp, only place that uses it.
        const auto unique_instruction = utility::scan(game, "88 85 92 01 00 00");

        if (!unique_instruction) {
            spdlog::error("[IntegrityCheckBypass]: Could not find unique_instruction!");
            return;
        }

        // Conditional jmp is very close to the instruction, before it.
        // However, this specific block of instructions is used all over the place
        // so we have to use the unique instruction is a reference point to scan from.
        const auto short_jmp_before = utility::scan_reverse(*unique_instruction, 0x100, "75 ? 50 F7 D0");

        if (short_jmp_before) {
            static auto patch = Patch::create(*short_jmp_before, { 0xEB }, true);
            spdlog::info("[IntegrityCheckBypass]: Patched conditional_jmp!");
            return;
        }

        // If we've gotten to this point, we are trying the scorched earth method of trying to obtain the function "start"
        // for this giant obfuscated blob. We will get the instructions behind the unique_instruction we found by doing that,
        // and look for the nearest branch instruction to patch.
        spdlog::error("[IntegrityCheckBypass]: Could not find short_jmp_before, trying fallback.");

        // Get the preceding instructions. If this doesn't work we'll need to scan for a common instruction anchor to scan forward from...
        auto previous_instructions = utility::get_disassembly_behind(*unique_instruction);

        if (previous_instructions.empty()) {
            spdlog::error("[IntegrityCheckBypass]: Could not find previous_instructions!");
            return;
        }

        // Reverse the order of the instructions.
        std::reverse(previous_instructions.begin(), previous_instructions.end());

        spdlog::info("[IntegrityCheckBypass]: Found {} previous instructions.", previous_instructions.size());
        spdlog::info("[IntegrityCheckBypass]: Walking previous instructions...");

        for (auto& insn : previous_instructions) {
            if (insn.instrux.BranchInfo.IsBranch) {
                spdlog::info("[IntegrityCheckBypass]: Found branch instruction, patching...");
                
                if (insn.instrux.BranchInfo.IsFar) {
                    static auto patch = Patch::create(insn.addr, { 0xE9 }, true);
                } else {
                    static auto patch = Patch::create(insn.addr, { 0xEB }, true);
                }

                spdlog::info("[IntegrityCheckBypass]: Patched conditional_jmp");
                return;
            }
        }
        
        spdlog::error("[IntegrityCheckBypass]: Could not find branch instruction to patch!");
        return;
    }

    const auto conditional_jmp = *conditional_jmp_block + 10;

    // Create a patch that always jumps.
    static auto patch = Patch::create(conditional_jmp, { 0xEB }, true);

    spdlog::info("[IntegrityCheckBypass]: Patched conditional_jmp!");
}

void IntegrityCheckBypass::remove_stack_destroyer() {
    spdlog::info("[IntegrityCheckBypass]: Searching for stack destroyer...");

    const auto game = utility::get_executable();
    const auto fn = utility::scan(game, "48 89 11 48 c7 04 24 00 00 00 00 48 81 c4 28 01 00 00");

    if (!fn) {
        spdlog::error("[IntegrityCheckBypass]: Could not find stack destroyer!");
        return;
    }

    // Create a patch that returns instantly.
    static auto patch = Patch::create(*fn, { 0xC3 }, true);

    spdlog::info("[IntegrityCheckBypass]: Patched stack destroyer!");
}