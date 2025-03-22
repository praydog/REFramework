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

#if TDB_VER >= 73
    Hooks::get()->ignore_application_entry(0x00c0ab9309584734);
    Hooks::get()->ignore_application_entry(0xa474f1d3a294e6a4);
#endif
#if TDB_VER >= 74
    Hooks::get()->ignore_application_entry(0x00ec4793097cd833);
    Hooks::get()->ignore_application_entry(0x00d85893096c4c0c);
#endif
}

void IntegrityCheckBypass::immediate_patch_re8() {
    // Apparently patching this in SF6 causes some bugs like chat not showing up and being unable to view replays.
    // Disabling it for now as the game still seems to work fine without it.
#ifdef SF6 
    if (true) {
        return;
    }
#endif

#if TDB_VER < 73
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
#endif
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

void* IntegrityCheckBypass::renderer_create_blas_hook(void* a1, void* a2, void* a3, void* a4, void* a5) {
    if (s_corruption_when_zero != nullptr) {
        if (*s_corruption_when_zero == 0) {
            *s_corruption_when_zero = s_last_non_zero_corruption;
            spdlog::info("[IntegrityCheckBypass]: Fixed corruption_when_zero!");
        }

        s_last_non_zero_corruption = *s_corruption_when_zero;
    }

    return s_renderer_create_blas_hook->get_original<decltype(renderer_create_blas_hook)>()(a1, a2, a3, a4, a5);
}

// This is used to nuke the heap allocated code that causes crashes
// when debuggers are attached and other integrity checks.
// They happen to be in the same (heap allocated) executable section, so we can just
// replace every byte with a RET instruction.
void IntegrityCheckBypass::nuke_heap_allocated_code(uintptr_t addr) {
    // Get the base of the memory region.
    MEMORY_BASIC_INFORMATION mbi{};
    if (VirtualQuery((LPCVOID)addr, &mbi, sizeof(mbi)) == 0) {
        spdlog::error("[IntegrityCheckBypass]: VirtualQuery failed!");
        return;
    }
    
    // Get the end of the memory region.
    const auto start = (uintptr_t)mbi.BaseAddress;
    const auto end = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;

    spdlog::info("[IntegrityCheckBypass]: Nuking heap allocated code at 0x{:X} - 0x{:X}", start, end);

    // Fix the protection of the memory region.
    ProtectionOverride _{(void*)start, mbi.RegionSize, PAGE_EXECUTE_READWRITE};

    // Replace every single byte with a RET (C3) instruction.
    std::memset((void*)start, 0xC3, mbi.RegionSize);

    spdlog::info("[IntegrityCheckBypass]: Nuked heap allocated code at 0x{:X}", start);
}

void IntegrityCheckBypass::anti_debug_watcher() try {
    static const auto ntdll = GetModuleHandleW(L"ntdll.dll");
    static const auto dbg_ui_remote_breakin = ntdll != nullptr ? GetProcAddress(ntdll, "DbgUiRemoteBreakin") : nullptr;
    static auto original_dbg_ui_remote_breakin_bytes = dbg_ui_remote_breakin != nullptr ? utility::get_original_bytes(dbg_ui_remote_breakin) : std::optional<std::vector<uint8_t>>{};

    if (dbg_ui_remote_breakin == nullptr) {
        return;
    }

    // We can generally assume it's not hooked at this point if the original bytes are empty.
    if (!original_dbg_ui_remote_breakin_bytes || original_dbg_ui_remote_breakin_bytes->empty()) {
        spdlog::info("[IntegrityCheckBypass]: Manually copying original bytes for DbgUiRemoteBreakin.");
        if (!original_dbg_ui_remote_breakin_bytes) {
            original_dbg_ui_remote_breakin_bytes = std::vector<uint8_t>{};
        }
    }

    if (original_dbg_ui_remote_breakin_bytes->size() < 32) {
        std::copy_n((uint8_t*)dbg_ui_remote_breakin + original_dbg_ui_remote_breakin_bytes->size(), 32 - original_dbg_ui_remote_breakin_bytes->size(), std::back_inserter(*original_dbg_ui_remote_breakin_bytes));
    }

    const uint64_t* first_8_bytes = (uint64_t*)dbg_ui_remote_breakin;
    const uint8_t* first_8_bytes_ptr = (uint8_t*)dbg_ui_remote_breakin;

    if (*(uint64_t*)original_dbg_ui_remote_breakin_bytes->data() != *first_8_bytes) {
        spdlog::info("[IntegrityCheckBypass]: DbgUiRemoteBreakin was hooked, restoring original bytes.");

        if (first_8_bytes_ptr[0] == 0xE9) {
            spdlog::info("[IntegrityCheckBypass]: DbgUiRemoteBreakin was directly hooked, resolving...");
            const auto resolved_jmp = utility::calculate_absolute((uintptr_t)dbg_ui_remote_breakin + 1);
            const auto is_heap_allocated = utility::get_module_within(resolved_jmp).value_or(nullptr) == nullptr;

            if (is_heap_allocated && !IsBadReadPtr((void*)resolved_jmp, 32)) {
                spdlog::info("[IntegrityCheckBypass]: Nuking heap allocated code at 0x{:X}", resolved_jmp);
                nuke_heap_allocated_code(resolved_jmp);
            }
        } else if (first_8_bytes_ptr[0] == 0xFF && first_8_bytes_ptr[1] == 0x25) {
            spdlog::info("[IntegrityCheckBypass]: DbgUiRemoteBreakin was indirectly hooked, resolving...");
            const auto resolved_ptr = utility::calculate_absolute((uintptr_t)dbg_ui_remote_breakin + 2);
            const auto resolved_jmp = *(uintptr_t*)resolved_ptr;
            const auto is_heap_allocated = utility::get_module_within(resolved_jmp).value_or(nullptr) == nullptr;

            if (is_heap_allocated && !IsBadReadPtr((void*)resolved_jmp, 32)) {
                spdlog::info("[IntegrityCheckBypass]: Nuking heap allocated code at 0x{:X}", resolved_jmp);
                nuke_heap_allocated_code(resolved_jmp);
            }
        }
        
        ProtectionOverride _{dbg_ui_remote_breakin, original_dbg_ui_remote_breakin_bytes->size(), PAGE_EXECUTE_READWRITE};
        std::copy(original_dbg_ui_remote_breakin_bytes->begin(), original_dbg_ui_remote_breakin_bytes->end(), (uint8_t*)dbg_ui_remote_breakin);

        spdlog::info("[IntegrityCheckBypass]: Restored DbgUiRemoteBreakin.");
    }
} catch (const std::exception& e) {
    spdlog::error("[IntegrityCheckBypass]: Exception in anti_debug_watcher: {}", e.what());
} catch (...) {
    spdlog::error("[IntegrityCheckBypass]: Unknown exception in anti_debug_watcher!");
}

void IntegrityCheckBypass::init_anti_debug_watcher() {
    if (s_anti_anti_debug_thread != nullptr) {
        return;
    }

    // Run the original watcher once so we get it at least without creating a thread first.
    anti_debug_watcher();

    s_anti_anti_debug_thread = std::make_unique<std::jthread>([](std::stop_token stop_token) {
        spdlog::info("[IntegrityCheckBypass]: Hello from anti_debug_watcher!");
        spdlog::info("[IntegrityCheckBypass]: Waiting for REFramework startup to finish...");

        while (g_framework == nullptr) {
            std::this_thread::yield();
        }

        spdlog::info("[IntegrityCheckBypass]: REFramework startup finished!");

        while (!stop_token.stop_requested()) {
            anti_debug_watcher();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });
}

// This allows unencrypted paks to load.
void IntegrityCheckBypass::sha3_rsa_code_midhook(safetyhook::Context& context) {
    spdlog::info("[IntegrityCheckBypass]: sha3_code_midhook called!");
    // Log registers
    spdlog::info("[IntegrityCheckBypass]: RAX: 0x{:X}", context.rax);
    spdlog::info("[IntegrityCheckBypass]: RCX: 0x{:X}", context.rcx);
    spdlog::info("[IntegrityCheckBypass]: RDX: 0x{:X}", context.rdx);
    spdlog::info("[IntegrityCheckBypass]: R8: 0x{:X}", context.r8);
    spdlog::info("[IntegrityCheckBypass]: R9: 0x{:X}", context.r9);
    spdlog::info("[IntegrityCheckBypass]: R10: 0x{:X}", context.r10);
    spdlog::info("[IntegrityCheckBypass]: R11: 0x{:X}", context.r11);
    spdlog::info("[IntegrityCheckBypass]: R12: 0x{:X}", context.r12);
    spdlog::info("[IntegrityCheckBypass]: R13: 0x{:X}", context.r13);
    spdlog::info("[IntegrityCheckBypass]: R14: 0x{:X}", context.r14);
    spdlog::info("[IntegrityCheckBypass]: R15: 0x{:X}", context.r15);
    spdlog::info("[IntegrityCheckBypass]: RSP: 0x{:X}", context.rsp);
    spdlog::info("[IntegrityCheckBypass]: RIP: 0x{:X}", context.rip);
    spdlog::info("[IntegrityCheckBypass]: RBP: 0x{:X}", context.rbp);
    spdlog::info("[IntegrityCheckBypass]: RSI: 0x{:X}", context.rsi);
    spdlog::info("[IntegrityCheckBypass]: RDI: 0x{:X}", context.rdi);

    enum PakFlags : uint8_t {
        ENCRYPTED = 0x8
    };

    const auto pak_flags = (PakFlags)context.r8; // Might change, maybe add automated register detection later

    if ((pak_flags & PakFlags::ENCRYPTED) != 0) {
        spdlog::info("[IntegrityCheckBypass]: Pak is encrypted, allowing decryption code to run.");
        return;
    }

    context.rip = *s_sha3_code_end;

    spdlog::info("[IntegrityCheckBypass]: Unencrypted pak detected, skipping decryption code!");
}

void IntegrityCheckBypass::restore_unencrypted_paks() {
    spdlog::info("[IntegrityCheckBypass]: Restoring unencrypted paks...");

    // If this breaks... we'll fix it!
    const auto game = utility::get_executable();
    const auto sha3_code_start = utility::scan(game, "C5 F8 57 C0 C5 FC 11 84 24 ? ? ? ? C5 FC 11 84 24 ? ? ? ? C5 FC 11 84 24 ? ? ? ? C5 FC 11 84 24 ? ? ? ? C5 FC 11 44 24 ? 48 C1 E9 ?");

    if (!sha3_code_start) {
        spdlog::error("[IntegrityCheckBypass]: Could not find sha3_rsa_code_start!");
        return;
    }
    
    spdlog::info("[IntegrityCheckBypass]: Found sha3_rsa_code_start @ 0x{:X}", *sha3_code_start);


    s_sha3_code_end = utility::scan(game, "48 8B 8E C0 00 00 00 48 C1 E9 ?");

    if (!s_sha3_code_end) {
        spdlog::error("[IntegrityCheckBypass]: Could not find sha3_rsa_code_end, cannot restore unencrypted paks!");
        return;
    }
    
    spdlog::info("[IntegrityCheckBypass]: Found sha3_rsa_code_end @ 0x{:X}", *s_sha3_code_end);

    s_sha3_rsa_code_midhook = safetyhook::create_mid((void*)*sha3_code_start, &sha3_rsa_code_midhook);

    spdlog::info("[IntegrityCheckBypass]: Created sha3_rsa_code_midhook!");

    const auto previous_instructions = utility::get_disassembly_behind(*s_sha3_code_end);

    // This NOPs out the conditional jump that rejects the PAK file if the integrity check fails.
    // Normally, the game computes a SHA3-256 hash of the TOC/resource headers before obfuscation.
    // It then XORs the TOC/resource headers with this hash to make them unreadable.
    // To verify integrity, the game decrypts a precomputed SHA3-256 hash from the PAK header using RSA.
    // If the computed SHA3-256 hash of the TOC/resource headers doesn't match the decrypted one, the PAK is rejected.
    // Without patching, bypassing this check would require obtaining Capcom's private RSA key to sign new hashes,
    // which is infeasible. Instead, we NOP the conditional jump to force the game to accept modded PAKs.
    // (Thanks to Rick Gibbed (@gibbed) for the explanation about how SHA3 and RSA fit together in this context)
    if (!previous_instructions.empty()) {
        const auto& previous_instruction = previous_instructions.back();

        if (previous_instruction.instrux.BranchInfo.IsBranch && previous_instruction.instrux.BranchInfo.IsConditional) {
            spdlog::info("[IntegrityCheckBypass]: Found conditional branch instruction @ 0x{:X}, NOPing it", previous_instruction.addr);

            // NOP out the conditional jump
            std::vector<int16_t> nops{};
            nops.resize(previous_instruction.instrux.Length);
            std::fill(nops.begin(), nops.end(), 0x90);

            static auto patch = Patch::create(previous_instruction.addr, nops, true);

            spdlog::info("[IntegrityCheckBypass]: NOP'd out conditional jump!");
        } else {
            spdlog::warn("[IntegrityCheckBypass]: Previous instruction is not a conditional branch, cannot NOP it!");
        }
    }
}

void IntegrityCheckBypass::immediate_patch_dd2() {
    // Just like RE4, this deals with the scans that are done every frame on the game's memory.
    // The scans are still performed, but the crash will be avoided.
    // This time, the obfuscation is much worse, and the way the crash is caused is much more indirect.
    // They corrupt something that has something to do with the renderer,
    // possibly with how it updates constant buffers and/or pipeline state
    // this makes the crash look like it comes from DXGI present, due to a GPU error.
    // The place this is happening is very simple, but it was not an easy find due to
    // how indirect it was + all the obfuscation.
    spdlog::info("[IntegrityCheckBypass]: Scanning DD2...");

    const auto game = utility::get_executable();
    const auto game_size = utility::get_module_size(game).value_or(0);
    const auto game_end = (uintptr_t)game + game_size;

#if TDB_VER >= 74
    init_anti_debug_watcher();

    const auto query_performance_frequency = &QueryPerformanceFrequency;
    const auto query_performance_counter = &QueryPerformanceCounter;

    if (query_performance_frequency != nullptr && query_performance_counter != nullptr) {
        const auto qpf_import = utility::scan_ptr(game, (uintptr_t)query_performance_frequency);
        const auto qpc_import = utility::scan_ptr(game, (uintptr_t)query_performance_counter);

        if (qpf_import && qpc_import) {
            const auto crasher_fn = utility::find_function_with_refs(game, { *qpf_import, *qpc_import });

            if (crasher_fn) {
                spdlog::info("[IntegrityCheckBypass]: Found crasher_fn!");

                auto crasher_fn_ref = utility::scan_displacement_reference(game, *crasher_fn);

                if (crasher_fn_ref) {
                    spdlog::info("[IntegrityCheckBypass]: Found crasher_fn_ref");
                }

                if (crasher_fn_ref && *(uint8_t*)(*crasher_fn_ref - 1) == 0xE9) {
                    crasher_fn_ref = utility::find_function_start(*crasher_fn_ref - 1);
                } else {
                    crasher_fn_ref = *crasher_fn;
                }

                if (crasher_fn_ref) {
                    spdlog::info("[IntegrityCheckBypass]: Found crasher fn (real)");

                    // We have to use this because I think that the AVX2 scan is broken here for some reason... uh oh...
                    const auto scanner_fn_middle = utility::scan_relative_reference_scalar((uintptr_t)game, game_size - 0x1000, *crasher_fn_ref, [](uintptr_t addr) {
                        return *(uint8_t*)(addr - 1) == 0xE8;
                    });

                    if (scanner_fn_middle) {
                        spdlog::info("[IntegrityCheckBypass]: Found scanner_fn_middle");

                        const auto scanner_fn = utility::find_function_start_unwind(*scanner_fn_middle);

                        if (scanner_fn) {
                            spdlog::info("[IntegrityCheckBypass]: Found scanner_fn!");
                            static auto nuke_patch = Patch::create(*scanner_fn, { 0xC3 }, true); // ret
                            spdlog::info("[IntegrityCheckBypass]: Patched scanner_fn!");
                        } else {
                            spdlog::error("[IntegrityCheckBypass]: Could not find scanner_fn!");
                        }
                    } else {
                        spdlog::error("[IntegrityCheckBypass]: Could not find scanner_fn_middle! (3)");
                    }
                } else {
                    spdlog::error("[IntegrityCheckBypass]: Could not find crasher_fn_ref! (2)");
                }

                // Make function just ret
                //static auto patch = Patch::create(*crasher_fn, { 0xC3 }, true);

                const auto cmp_jz = utility::find_pattern_in_path((uint8_t*)*crasher_fn, 1000, false, "39 0C 82 74 ?");

                if (cmp_jz) {
                    static auto patch = Patch::create(cmp_jz->addr + 3, { 0xEB }, true);
                    spdlog::info("[IntegrityCheckBypass]: Patched crasher_fn!");
                } else {
                    spdlog::error("[IntegrityCheckBypass]: Could not find cmp_jz!");
                }
            } else {
                spdlog::error("[IntegrityCheckBypass]: Could not find crasher_fn!");
            }
        } else {
            spdlog::error("[IntegrityCheckBypass]: Could not find QueryPerformanceFrequency/Counter imports!");
        }
    }

    if (const auto create_blas_fn = utility::find_function_from_string_ref(game, "createBLAS"); create_blas_fn.has_value()) {
        const auto create_blas_fn_unwind = utility::find_function_start_unwind(*create_blas_fn);

        if (create_blas_fn_unwind) {
            spdlog::info("[IntegrityCheckBypass]: Found createBLAS!");

            // Look for first lea rcx, [mem]
            const auto lea_rcx = utility::find_pattern_in_path((uint8_t*)*create_blas_fn_unwind, 1000, false, "48 8D 0D ? ? ? ?");

            if (lea_rcx) {
                s_corruption_when_zero = (uint32_t*)utility::calculate_absolute(lea_rcx->addr + 3);
                spdlog::info("[IntegrityCheckBypass]: Found corruption_when_zero!");
            } else {
                spdlog::error("[IntegrityCheckBypass]: Could not find lea_rcx!");
            }

            s_renderer_create_blas_hook = std::make_unique<FunctionHook>(*create_blas_fn, &renderer_create_blas_hook);

            if (!s_renderer_create_blas_hook->create()) {
                spdlog::error("[IntegrityCheckBypass]: Failed to hook createBLAS!");
            } else {
                spdlog::info("[IntegrityCheckBypass]: Hooked createBLAS!");
            }
        } else {
            spdlog::error("[IntegrityCheckBypass]: Could not find unwound createBLAS!");
        }
    } else {
        spdlog::error("[IntegrityCheckBypass]: Could not find createBLAS!");
    }

    static std::vector<Patch::Ptr> sus_constant_patches{};

    for (auto ref = utility::scan(game, "81 ? E1 53 BD 4C");
         ref.has_value();
         ref = utility::scan(*ref + 1, (game_end - (*ref + 1)) - 0x1000, "81 ? E1 53 BD 4C"))
    {
        // Patch to 0x1337BEEF
        sus_constant_patches.emplace_back(Patch::create(*ref + 2, { 0xEF, 0xBE, 0x37, 0x13 }, true));
    }

    spdlog::info("[IntegrityCheckBypass]: Patched {} sus_constants!", sus_constant_patches.size());

    restore_unencrypted_paks();
#endif

    const auto conditional_jmp_block = utility::scan(game, "41 8B ? ? 78 83 ? 07 ? ? 75 ?");

    if (conditional_jmp_block) {
        // Jnz->Jmp
        const auto conditional_jmp = *conditional_jmp_block + 10;

        // Create a patch that always jumps.
        static auto dd2patch = Patch::create(conditional_jmp, { 0xEB }, true);

        spdlog::info("[IntegrityCheckBypass]: Patched conditional_jmp! (DD2)");
    } else {
        spdlog::error("[IntegrityCheckBypass]: Could not find conditional_jmp for DD2, attempting fallback.");

        const auto create_blas_fn = utility::find_function_from_string_ref(game, "createBLAS");

        if (create_blas_fn) {
            const auto and_eax_07_instr = utility::find_pattern_in_path((uint8_t*)*create_blas_fn, 100, false, "83 E0 07");
            
            if (and_eax_07_instr) {
                // Find next conditional jmp and patch it.
                const auto conditional_jmp = utility::scan_mnemonic(and_eax_07_instr->addr + and_eax_07_instr->instrux.Length, 10, "JNZ");

                if (conditional_jmp) {
                    // Jnz->Jmp
                    static auto dd2patch = Patch::create(*conditional_jmp, { 0xEB }, true);

                    spdlog::info("[IntegrityCheckBypass]: Patched conditional_jmp! (DD2)");
                } else {
                    spdlog::error("[IntegrityCheckBypass]: Could not find conditional_jmp for DD2.");
                }
            }
        } else {
            spdlog::error("[IntegrityCheckBypass]: Could not find createBLAS!");
        }
    }

    const auto second_conditional_jmp_block = utility::scan(game, "49 3B D0 75 ? ? 8B ? ? ? ? ? ? 8B ? ? ? ? ? ? 8B ? ? 8B ? ? ? ? ?");

    if (second_conditional_jmp_block) {
        // Jnz->Jmp
        const auto second_conditional_jmp = *second_conditional_jmp_block + 3;

        // Create a patch that always jumps.
        static auto dd2patch2 = Patch::create(second_conditional_jmp, { 0xEB }, true);

        spdlog::info("[IntegrityCheckBypass]: Patched second_conditional_jmp! (DD2)");
    } else {
        spdlog::error("[IntegrityCheckBypass]: Could not find second_conditional_jmp for DD2.");
    }

    const auto natives_str_addr = utility::scan(game, "00 00 2F 00 6E 00 61 00 74 00 69 00 76 00 65 00 73 00 2F 00 00 00");

    // the purpose of this is to re-enable loose file loading
    // the game explicitly looks for this string in the path and
    // causes load failures if it finds it
    if (natives_str_addr) {
        spdlog::info("[IntegrityCheckBypass]: Found /natives/ string for DD2. Patching...");

        wchar_t* natives_str = (wchar_t*)(*natives_str_addr + 2);
        DWORD old_protect{};
        VirtualProtect(natives_str, 10 * sizeof(wchar_t), PAGE_EXECUTE_READWRITE, &old_protect);

        spdlog::info("[IntegrityCheckBypass]: /natives/ string: {}", utility::narrow(natives_str));

        // replace string with a completely invalid string that cannot be a valid path
        natives_str[0] = L'?'; // /

        DWORD old2{};
        VirtualProtect(natives_str, 10 * sizeof(wchar_t), old_protect, &old2);

        spdlog::info("[IntegrityCheckBypass]: Patched /natives/ string for DD2.");
    } else {
        spdlog::error("[IntegrityCheckBypass]: Could not find /natives/ string for DD2.");
    }
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

void IntegrityCheckBypass::setup_pristine_syscall() {
    if (s_pristine_protect_virtual_memory != nullptr) {
        spdlog::info("[IntegrityCheckBypass]: NtProtectVirtualMemory already setup!");
        return;
    }

    spdlog::info("[IntegrityCheckBypass]: Copying pristine NtProtectVirtualMemory...");

    const auto ntdll_base = GetModuleHandleA("ntdll.dll");

    if (ntdll_base == nullptr) {
        spdlog::error("[IntegrityCheckBypass]: Could not find ntdll!");
        return;
    }

    auto nt_protect_virtual_memory = (NtProtectVirtualMemory_t)GetProcAddress(ntdll_base, "NtProtectVirtualMemory");
    if (nt_protect_virtual_memory == nullptr) {
        spdlog::error("[IntegrityCheckBypass]: Could not find NtProtectVirtualMemory!");
        return;
    }

    spdlog::info("[IntegrityCheckBypass]: Found NtProtectVirtualMemory at 0x{:X}", (uintptr_t)nt_protect_virtual_memory);

    if (*(uint8_t*)nt_protect_virtual_memory == 0xE9) {
        spdlog::info("[IntegrityCheckBypass]: Found jmp at 0x{:X}, resolving...", (uintptr_t)nt_protect_virtual_memory);
        nt_protect_virtual_memory = (decltype(nt_protect_virtual_memory))utility::calculate_absolute((uintptr_t)nt_protect_virtual_memory + 1);
    }

    s_og_protect_virtual_memory = nt_protect_virtual_memory;

    // Mark the original VirtualProtect READ_WRITE_EXECUTE so if anything tries to restore the old protection, it will revert to this
    // incase trying to modify the protection after it is hooked causes a crash
    DWORD old_nt_protect_virtual_memory_protect{};
    VirtualProtect(nt_protect_virtual_memory, 256, PAGE_EXECUTE_READWRITE, &old_nt_protect_virtual_memory_protect);

    s_pristine_protect_virtual_memory = (decltype(s_pristine_protect_virtual_memory))VirtualAlloc(nullptr, 256, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    try {
        memcpy(s_pristine_protect_virtual_memory, nt_protect_virtual_memory, 256);
    } catch(...) {
        spdlog::error("[IntegrityCheckBypass]: Could not copy new instructions to pristine NtProtectVirtualMemory!");
    }

    spdlog::info("[IntegrityCheckBypass]: Copied NtProtectVirtualMemory to 0x{:X}", (uintptr_t)s_pristine_protect_virtual_memory);
}

// hahahah i hate this
void IntegrityCheckBypass::fix_virtual_protect() try {
    spdlog::info("[IntegrityCheckBypass]: Fixing VirtualProtect...");

    setup_pristine_syscall(); // Called earlier in DllMain

    // Hook VirtualProtect
    s_virtual_protect_hook = std::make_unique<FunctionHookMinHook>(VirtualProtect, (uintptr_t)virtual_protect_hook);
    if (!s_virtual_protect_hook->create()) {
        spdlog::error("[IntegrityCheckBypass]: Could not hook VirtualProtect!");
        return;
    }

    spdlog::info("[IntegrityCheckBypass]: Hooked VirtualProtect!");
} catch(...) {
    spdlog::error("[IntegrityCheckBypass]: Could not fix VirtualProtect!");
}

BOOL WINAPI IntegrityCheckBypass::virtual_protect_impl(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect) {
    static const auto this_process = GetCurrentProcess();

    LPVOID address_to_protect = lpAddress;
    NTSTATUS result = s_og_protect_virtual_memory(this_process, (PVOID*)&address_to_protect, &dwSize, flNewProtect, lpflOldProtect);

    constexpr NTSTATUS STATUS_INVALID_PAGE_PROTECTION = 0xC0000045;

    // recreated from kernelbase to be correct
    if (result == STATUS_INVALID_PAGE_PROTECTION) {
        using RtlFlushSecureMemoryCache_t = BOOLEAN (NTAPI*)(PVOID, SIZE_T);
        static const auto rtl_flush_secure_memory_cache = (RtlFlushSecureMemoryCache_t)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlFlushSecureMemoryCache");

        if (rtl_flush_secure_memory_cache != nullptr) {
            if (NT_SUCCESS(rtl_flush_secure_memory_cache(address_to_protect, dwSize))) {
                result = s_og_protect_virtual_memory(this_process, (PVOID*)&address_to_protect, &dwSize, flNewProtect, lpflOldProtect);

                if ((result & 0x80000000) == 0) {
                    return TRUE;
                }
            }
        }
    }

    if (!NT_SUCCESS(result)) {
        spdlog::error("[IntegrityCheckBypass]: NtProtectVirtualMemory(-1, {:x}, {:x}, {:x}, {:x}) failed with {:x}", (uintptr_t)address_to_protect, dwSize, flNewProtect, (uintptr_t)lpflOldProtect, (uint32_t)result);
    }
    
    return NT_SUCCESS(result);
}

// This allows our calls to VirtualProtect to go through without being hindered by... something.
BOOL WINAPI IntegrityCheckBypass::virtual_protect_hook(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect) try {
    static bool once = true;
    if (once) {
        spdlog::info("[IntegrityCheckBypass]: VirtualProtect called");
        once = false;
    }

    try {
        if (memcmp(s_og_protect_virtual_memory, s_pristine_protect_virtual_memory, 32) != 0) {
            spdlog::warn("[IntegrityCheckBypass]: Original NtProtectVirtualMemory has been modified! Attempting to restore...");

            bool needs_protection_fix = false;

            try {
                memcpy(s_og_protect_virtual_memory, s_pristine_protect_virtual_memory, 32);

                if (memcmp(s_og_protect_virtual_memory, s_pristine_protect_virtual_memory, 32) == 0) {
                    spdlog::info("[IntegrityCheckBypass]: Successfully restored NtProtectVirtualMemory");
                } else {
                    needs_protection_fix = true;
                    spdlog::error("[IntegrityCheckBypass]: Could not restore NtProtectVirtualMemory without changing protection!");
                }
            } catch(...) {
                needs_protection_fix = true;
                spdlog::error("[IntegrityCheckBypass]: Could not restore NtProtectVirtualMemory without changing protection! Attempting to restore protection anyway...");
            }

            if (needs_protection_fix) try {
                spdlog::info("[IntegrityCheckBypass]: Attempting to restore NtProtectVirtualMemory protection");

                DWORD old{};

                // Now this is a huge assumption that the hook that was placed on NtProtectVirtualMemory
                // does not prevent calling it on itself, which is a very dangerous assumption to make.
                // Usually it fails if it's attempted to be called on the executable's memory, but not other modules.
                // However I have tested it and it *does* work, so we will roll with that for now, until it doesn't.
                if (virtual_protect_impl((void*)((uintptr_t)s_og_protect_virtual_memory - 1), 33, PAGE_EXECUTE_READWRITE, &old)) {
                    memcpy(s_og_protect_virtual_memory, s_pristine_protect_virtual_memory, 32);
                    virtual_protect_impl((void*)((uintptr_t)s_og_protect_virtual_memory - 1), 33, old, &old);

                    spdlog::info("[IntegrityCheckBypass]: Restored NtProtectVirtualMemory");
                }
            } catch(...) {
                spdlog::error("[IntegrityCheckBypass]: Could not restore NtProtectVirtualMemory protection!");
            }
        }
    } catch(...) {
        spdlog::error("[IntegrityCheckBypass]: Failed to verify NtProtectVirtualMemory integrity!");
    }

    return virtual_protect_impl(lpAddress, dwSize, flNewProtect, lpflOldProtect);
} catch(...) {
    spdlog::error("[IntegrityCheckBypass]: VirtualProtect hook failed! falling back to original");
    return s_virtual_protect_hook->get_original<decltype(virtual_protect_hook)>()(lpAddress, dwSize, flNewProtect, lpflOldProtect);
}

void IntegrityCheckBypass::hook_add_vectored_exception_handler() {
#if TDB_VER >= 73
    spdlog::info("[IntegrityCheckBypass]: Hooking AddVectoredExceptionHandler...");

    s_add_vectored_exception_handler_hook = std::make_unique<FunctionHookMinHook>(AddVectoredExceptionHandler, (uintptr_t)add_vectored_exception_handler_hook);
    if (!s_add_vectored_exception_handler_hook->create()) {
        spdlog::error("[IntegrityCheckBypass]: Could not hook AddVectoredExceptionHandler!");
        return;
    }

    spdlog::info("[IntegrityCheckBypass]: Hooked AddVectoredExceptionHandler!");
#endif
}

PVOID WINAPI IntegrityCheckBypass::add_vectored_exception_handler_hook(ULONG FirstHandler, PVECTORED_EXCEPTION_HANDLER VectoredHandler) {
    s_veh_called = true;

    if (!s_veh_allowed) {
        const auto retaddr = (uintptr_t)_ReturnAddress();
        const auto module_within = utility::get_module_within(retaddr);

        if (module_within) {
            const auto module_path = utility::get_module_pathw(*module_within);
            bool is_allowed = false;

            if (module_path) {
                if (module_path->find(L"vehdebug") != std::wstring::npos) {
                    is_allowed = true;
                }

                if (module_path->find(L"coreclr") != std::wstring::npos) {
                    is_allowed = true;
                }

                if (module_path->find(L"dinput8") != std::wstring::npos) {
                    is_allowed = true;
                }
            }

            if (is_allowed || *module_within == REFramework::get_reframework_module()) 
            {
                if (module_path) {
                    spdlog::info("[IntegrityCheckBypass]: VEH allowed for {}", utility::narrow(*module_path));
                } else {
                    spdlog::info("[IntegrityCheckBypass]: VEH allowed");
                }

                return s_add_vectored_exception_handler_hook->get_original<decltype(add_vectored_exception_handler_hook)>()(FirstHandler, VectoredHandler);
            }
        }
        
        spdlog::warn("[IntegrityCheckBypass]: VEH not allowed, returning nullptr");
        allow_veh(); // VEH past this point should be okay.
        return (void*)VectoredHandler; // some bs address so it doesnt detect it as a nullptr
    }

    spdlog::info("[IntegrityCheckBypass]: VEH allowed");

    return s_add_vectored_exception_handler_hook->get_original<decltype(add_vectored_exception_handler_hook)>()(FirstHandler, VectoredHandler);
}

void IntegrityCheckBypass::hook_rtl_exit_user_process() {
    spdlog::info("[IntegrityCheckBypass]: Hooking RtlExitUserProcess...");

    const auto ntdll = GetModuleHandleW(L"ntdll.dll");

    if (ntdll == nullptr) {
        spdlog::error("[IntegrityCheckBypass]: Could not find ntdll!");
        return;
    }

    const auto RtlExitUserProcess = GetProcAddress(ntdll, "RtlExitUserProcess");

    if (RtlExitUserProcess == nullptr) {
        spdlog::error("[IntegrityCheckBypass]: Could not find RtlExitUserProcess!");
        return;
    }

    s_rtl_exit_user_process_hook = std::make_unique<FunctionHookMinHook>(RtlExitUserProcess, &rtl_exit_user_process_hook);
    if (!s_rtl_exit_user_process_hook->create()) {
        spdlog::error("[IntegrityCheckBypass]: Could not hook RtlExitUserProcess!");
        return;
    }

    spdlog::info("[IntegrityCheckBypass]: Hooked RtlExitUserProcess!");
}

void* IntegrityCheckBypass::rtl_exit_user_process_hook(uint32_t code) {
    /*__try {
        auto orig = s_rtl_exit_user_process_hook->get_original<decltype(rtl_exit_user_process_hook)>()(code);
        return orig;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        TerminateProcess(GetCurrentProcess(), code);
        return nullptr;
    }*/

    // ok for some reason I can't explain yet,
    // we need to do this because the game crashes if we don't
    // It seems to have something to do with RtlpFlsDataCleanup (which is called by RtlExitUserProcess)
    // and I think is responsible for calling the TLS destructors?
    // It calls something that's heap allocated but no longer exists, and it crashes.
    TerminateProcess(GetCurrentProcess(), code);
    return nullptr;
}