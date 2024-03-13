#include <shared_mutex>
#include <spdlog/spdlog.h>

#include "utility/Scan.hpp"
#include "utility/Module.hpp"

#include "reframework/API.hpp"
#include "ReClass.hpp"
#include "RETypeDB.hpp"
#include "REContext.hpp"

namespace sdk {
    VM** VM::s_global_context{ nullptr };
    sdk::InvokeMethod* VM::s_invoke_tbl{nullptr};
    VM::ThreadContextFn VM::s_get_thread_context{ nullptr };
    int32_t VM::s_static_tbl_offset{ 0 };
    int32_t VM::s_type_db_offset{ 0 };

    sdk::VM* VM::get() {
        update_pointers();
        return *s_global_context;
    }

    REThreadContext* VM::get_thread_context(int32_t unk /*= -1*/) {
        update_pointers();

        return s_get_thread_context(this, unk);
    }

    sdk::RETypeDB* VM::get_type_db() {
        update_pointers();

        return *(sdk::RETypeDB**)((uintptr_t)this + s_type_db_offset);
    }

    REStaticTbl& VM::get_static_tbl() {
        update_pointers();

        return *(REStaticTbl*)((uintptr_t)this + s_static_tbl_offset);
    }

    uint8_t* VM::get_static_tbl_for_type(uint32_t type_index) {
#if TDB_VER > 49
        auto& tbls = get_static_tbl();

        /*if (type_index >= tbls.size) {
            return nullptr;
        }*/

        return tbls.elements[type_index];
#else
        if (type_index >= this->num_types) {
            return nullptr;
        }

        return (uint8_t*)this->types[type_index].static_fields;
#endif
    }

    static std::shared_mutex s_mutex{};

    void VM::update_pointers() {
        {
            // Lock a shared lock for the s_mutex
            std::shared_lock lock(s_mutex);

            if (s_global_context != nullptr && s_get_thread_context != nullptr) {
                return;
            }
        }

        // Create a unique lock for the s_mutex as we get to the meat of the function
        std::unique_lock lock{ s_mutex };

        spdlog::info("[VM::update_pointers] Updating...");

        // Version 1
        //auto ref = utility::scan(g_framework->getModule().as<HMODULE>(), "48 8B 0D ? ? ? ? BA FF FF FF FF E8 ? ? ? ? 48 89 C3");

        auto mod = utility::get_executable();
        auto start = (uintptr_t)mod;
        auto end = (uintptr_t)start + *utility::get_module_size(mod);

        std::unordered_map<uintptr_t, uint32_t> references{};

        struct CtxPattern {
            int32_t ctx_offset;
            int32_t get_thread_context_offset;
            std::string pattern;
        };

        auto patterns = std::vector<CtxPattern>{
            // Version 2 Dec 17th, 2019, first ptr is at game.exe+0x7095E08
            // Works RE2, RE3, RE8, DMC5
            { 3, 13, "48 8B 0D ? ? ? ? BA FF FF FF FF E8 ? ? ? ?" },
             // Only seen in RE7
            { 3, 11, "48 8B 0D ? ? ? ? 83 CA FF E8 ? ? ? ?" },
        };


        std::optional<Address> ref{};
        const CtxPattern* context_pattern{nullptr};
        
        for (const auto& pattern : patterns) {
            ref = {};
            references.clear();

            const auto& pat = pattern.pattern;

            for (auto i = utility::scan(start, end - start, pat); i.has_value(); i = utility::scan(*i + 1, end - (*i + 1), pat)) {
                auto potential_ctx_ref = utility::calculate_absolute(*i + 3);

                references[potential_ctx_ref]++;

                // this is for sure the right one
                if (references[potential_ctx_ref] > 10) {
                    ref = *i;
                    context_pattern = &pattern;
                    break;
                }
            }

            if (ref) {
                break;
            }
        }

        if (!ref || *ref == nullptr) {
            spdlog::info("[VM::update_pointers] Unable to find ref.");
            return;
        }

        s_global_context = (decltype(s_global_context))utility::calculate_absolute(*ref + context_pattern->ctx_offset);
        s_get_thread_context = (decltype(s_get_thread_context))utility::calculate_absolute(*ref + context_pattern->get_thread_context_offset);

        for (auto i = 0; i < 0x20000; i += sizeof(void*)) {
            auto ptr = *(sdk::RETypeDB**)((uintptr_t)*s_global_context + i);

            if (ptr == nullptr || IsBadReadPtr(ptr, sizeof(void*)) || ((uintptr_t)ptr & (sizeof(void*) - 1)) != 0) {
                continue;
            }

            if (*(uint32_t*)ptr == *(uint32_t*)"TDB") {
                const auto version = *(uint32_t*)((uintptr_t)ptr + 4);

                s_tdb_version = version;
                s_type_db_offset = i;
                s_static_tbl_offset = s_type_db_offset - 0x30; // hope this holds true for the older gameS!!!!!!!!!!!!!!!!!!!
                spdlog::info("[VM::update_pointers] s_type_db_offset: {:x}", s_type_db_offset);
                spdlog::info("[VM::update_pointers] s_static_tbl_offset: {:x}", s_static_tbl_offset);
                spdlog::info("[VM::update_pointers] TDB Version: {}", version);
                spdlog::info("[VM::update_pointers] TDB: {:x}", (uintptr_t)ptr);
                break;
            }
        }

        spdlog::info("[VM::update_pointers] s_global_context: {:x}", (uintptr_t)s_global_context);
        spdlog::info("[VM::update_pointers] s_get_thread_context: {:x}", (uintptr_t)s_get_thread_context);

        // Needed on TDB73/AJ. The 0x30 offset we have is not correct, so we need to find the correct one
        // And the "correct" one is the first one that doesn't look like a BS pointer (crude, i know)
        // so... TODO: find a better way to do this
#if TDB_VER >= 71
        if (s_global_context != nullptr && *s_global_context != nullptr) {
            auto static_tbl = (REStaticTbl**)((uintptr_t)*s_global_context + s_static_tbl_offset);
            if (IsBadReadPtr(*static_tbl, sizeof(void*)) || ((uintptr_t)*static_tbl & (sizeof(void*) - 1)) != 0) {
                spdlog::info("[VM::update_pointers] Static table offset is bad, correcting...");

                // We are looking for the two arrays, the static field table, and the static field "initialized table"
                // The initialized table tells whether a specific entry in the static field table has been initialized or not
                // so they both should have the same size, easy to find
                for (auto i = sizeof(void*); i < 0x100; i+= sizeof(void*)) {
                    const auto& ptr = *(REStaticTbl**)((uintptr_t)*s_global_context + (s_type_db_offset - i));

                    if (IsBadReadPtr(ptr, sizeof(void*)) || ((uintptr_t)ptr & (sizeof(void*) - 1)) != 0) {
                        continue;
                    }

                    spdlog::info("[VM::update_pointers] Examining {:x}", (uintptr_t)ptr);

                    const auto& potential_count = *(uint32_t*)((uintptr_t)&ptr + sizeof(void*));

                    if (potential_count < 2000) {
                        continue;
                    }

                    constexpr auto array_size = (sizeof(void*) * 2);
                    const auto previous_offset = s_type_db_offset - i - array_size;
                    const auto& previous_ptr = *(REStaticTbl**)((uintptr_t)*s_global_context + previous_offset);
                    const auto& previous_count = *(uint32_t*)((uintptr_t)&previous_ptr + sizeof(void*));

                    if (previous_count == potential_count) {
                        spdlog::info("[VM::update_pointers] Found static table at {:x} (offset {:x})", (uintptr_t)ptr, previous_offset);
                        s_static_tbl_offset = previous_offset;
                        break;
                    }
                }
            }
        }
#endif

        // Get invoke_tbl
        // this SEEMS to work on RE2 and onwards, but not on RE7
        // look into it later
#if TDB_VER > 49
        // Just a potential method inside the table
        // we will scan for something pointing to it,
        // meaning that we will land in the middle of the invoke table somwhere
        // from there, we will scan backwards for a null pointer,
        // which will be the start of the table
        std::vector<std::string> invoke_patterns {
            "40 53 48 83 ec 20 48 8b 41 30 4c 8b d2 48 8b 51 40 48 8b d9 4c 8b 00 48 8b 41 10", // RE2 - MHRise v1
            "40 53 48 83 ec 20 48 8b 41 10 48 8b da 8b 48 08", // MHRise Sunbreak/newer games?
            "40 53 48 83 EC ? 48 8B 41 30 4C 8B D2 4C 8B 49 10 48 8B D9 48 8B 51 40 49 8B CA 4C 8B 00 41 FF" // seen in game pass RE2
        };

        // ok so if these patterns above are failing, we can find the invoke table by looking for these set of instructions:
        // 8D 56 FF                                      lea     edx, [rsi-1]
        // 48 8B CF                                      mov     rcx, rdi
        // E8 67 FD 6C 01                                call    sub_[removed]

        // Then, scroll up from here, and you'll see something that looks like this:
        /*
        E8 D4 D6 6C 01                                call    sub_[removed]
        41 B8 53 15 00 00                             mov     r8d, 1553h ; dead giveaway is a number like this that is the size of the invoke table
        48 8D 15 37 C7 6B 06                          lea     rdx, g_invokeTbl ; this is what we want
        48 8B 08                                      mov     rcx, [rax]
        48 8B 05 2D 47 86 06                          mov     rax, cs:off_[removed]
        48 89 08                                      mov     [rax], rcx
        48 8B CF                                      mov     rcx, rdi
        */


        std::optional<uintptr_t> method_inside_invoke_tbl{std::nullopt};

        for (const auto& pat : invoke_patterns) {
            auto ref = utility::scan(mod, pat);

            if (ref) {
                method_inside_invoke_tbl = ref;
                break;
            }
        }
        if (!method_inside_invoke_tbl) {
            spdlog::info("[VM::update_pointers] Unable to find method inside invoke table.");
            return;
        }

        spdlog::info("[VM::update_pointers] method_inside_invoke_tbl: {:x}", (uintptr_t)*method_inside_invoke_tbl);

        auto ptr_inside_invoke_tbl = utility::scan_ptr(mod, *method_inside_invoke_tbl);

        if (!ptr_inside_invoke_tbl) {
            spdlog::info("[VM::update_pointers] Unable to find ptr inside invoke table.");
            return;
        }

        spdlog::info("[VM::update_pointers] ptr_inside_invoke_tbl: {:x}", (uintptr_t)*ptr_inside_invoke_tbl);

        // Scan backwards for a null pointer
        for (auto i = *ptr_inside_invoke_tbl; ; i -= sizeof(void*)) {
            if (*(void**)i == nullptr) {
                s_invoke_tbl = (sdk::InvokeMethod*)i;
                break;
            }
        }

        spdlog::info("[VM::update_pointers] s_invoke_tbl: {:x}", (uintptr_t)s_invoke_tbl);
#endif
    }

    sdk::VMContext* get_thread_context(int32_t unk /*= -1*/) {
        auto global_context = VM::get();

        if (global_context == nullptr) {
            return nullptr;
        }

        return (sdk::VMContext*)global_context->get_thread_context(unk);
    }

    static std::shared_mutex s_pointers_mtx{};
    static void* (*s_context_unhandled_exception_fn)(::REThreadContext*) = nullptr;
    static void* (*s_context_local_frame_gc_fn)(::REThreadContext*) = nullptr;
    static void* (*s_context_end_global_frame_fn)(::REThreadContext*) = nullptr;

    void sdk::VMContext::update_pointers() {
        {
            std::shared_lock _{s_pointers_mtx};

            if (s_context_unhandled_exception_fn != nullptr && s_context_local_frame_gc_fn != nullptr && s_context_end_global_frame_fn != nullptr) {
                return;
            }
        }
        
        std::unique_lock _{s_pointers_mtx};

        spdlog::info("Locating funcs");
        
        // Version 1
        //auto ref = utility::scan(g_framework->getModule().as<HMODULE>(), "48 83 78 18 00 74 ? 48 89 D9 E8 ? ? ? ? 48 89 D9 E8 ? ? ? ?");

        // Version 2 Dec 17th, 2019 game.exe+0x20437C (works on old version too)
        auto ref = utility::scan(utility::get_executable(), "48 83 78 18 00 74 ? 48 ? ? E8 ? ? ? ? 48 ? ? E8 ? ? ? ? 48 ? ? E8 ? ? ? ?");

        if (!ref) {
            spdlog::error("We're going to crash");
            return;
        }

        s_context_unhandled_exception_fn = Address{ utility::calculate_absolute(*ref + 11) }.as<decltype(s_context_unhandled_exception_fn)>();
        s_context_local_frame_gc_fn = Address{ utility::calculate_absolute(*ref + 19) }.as<decltype(s_context_local_frame_gc_fn)>();
        s_context_end_global_frame_fn = Address{ utility::calculate_absolute(*ref + 27) }.as<decltype(s_context_end_global_frame_fn)>();

        spdlog::info("Context::UnhandledException {:x}", (uintptr_t)s_context_unhandled_exception_fn);
        spdlog::info("Context::LocalFrameGC {:x}", (uintptr_t)s_context_local_frame_gc_fn);
        spdlog::info("Context::EndGlobalFrame {:x}", (uintptr_t)s_context_end_global_frame_fn);
    }

    void* sdk::VMContext::unhandled_exception() {
        update_pointers();

        return s_context_unhandled_exception_fn(this);
    }

    void* sdk::VMContext::local_frame_gc() {
        update_pointers();

        return s_context_local_frame_gc_fn(this);
    }

    void* sdk::VMContext::end_global_frame() {
        update_pointers();

        return s_context_end_global_frame_fn(this);
    }

    void sdk::VMContext::cleanup_after_exception(int32_t old_reference_count) {
        auto& reference_count = this->referenceCount;
        auto count_delta = reference_count - old_reference_count;

        spdlog::error("{}", reference_count);
        if (count_delta >= 1) {
            --reference_count;

            // Perform object cleanup that was missed because an exception occurred.
            if (this->unkPtr != nullptr && this->unkPtr->unkPtr != nullptr) {
                this->unhandled_exception();
            }

            this->local_frame_gc();
            this->end_global_frame();
        } else if (count_delta == 0) {
            spdlog::info("No fix necessary");
        }
    }

    void sdk::VMContext::safe_wrap(std::string_view function_name, std::function<void()> func) {
        auto context = sdk::get_thread_context();
        sdk::VMContext::ScopedTranslator scoped_translator{context};

        bool corrupted_before_call = context->unkPtr != nullptr && context->unkPtr->unkPtr != nullptr;

        try {
            func();

            if (context->unkPtr->unkPtr != nullptr) {
                spdlog::error("Internal game exception thrown in function call for {}", function_name.data());

                const auto exception_managed_object = (::REManagedObject*)context->unkPtr->unkPtr;

                if (utility::re_managed_object::is_managed_object(exception_managed_object)) {
                    const auto exception_tdb_type = utility::re_managed_object::get_type_definition(exception_managed_object);

                    if (exception_tdb_type != nullptr) {
                        const auto exception_name = exception_tdb_type->get_full_name();
                        spdlog::error(" Exception name: {}", exception_name.data());
                    }
                }

                if (corrupted_before_call) {
                    spdlog::error("VMContext was already corrupted before this call, a previous exception may not have been handled properly");
                }

                context->unkPtr->unkPtr = nullptr;
                throw std::runtime_error("Internal game exception thrown in function call for " + std::string(function_name.data()));
            }
        } catch(sdk::VMContext::Exception&) {
            spdlog::error("Exception thrown in call to {}", function_name.data());
            context->cleanup_after_exception(scoped_translator.get_prev_reference_count());

            throw std::runtime_error("Exception thrown in call to " + std::string(function_name.data()));
        }
    }

    void sdk::VMContext::ScopedTranslator::translator(unsigned int code, struct ::_EXCEPTION_POINTERS* exc) {
        spdlog::info("VMContext: Caught exception code {:x}", code);

        switch (code) {
        case EXCEPTION_ACCESS_VIOLATION:
            spdlog::info("VMContext: Attempting to handle access violation.");

        default:
            break;
        }

        throw sdk::VMContext::Exception{};
    }

    sdk::InvokeMethod* VM::get_invoke_table() {
        update_pointers();

        return s_invoke_tbl;
    }

    ::SystemString* VM::create_managed_string(std::wstring_view str) {
        static auto empty_string = *sdk::get_static_field<REManagedObject*>("System.String", "Empty");
        static std::vector<uint8_t> huge_string_data{};

        if (huge_string_data.empty()) {
            huge_string_data.resize(sizeof(REManagedObject) + 4 + 2048);
            memset(&huge_string_data[0], 0, huge_string_data.size());

            auto huge_string = (SystemString*)&huge_string_data[0];
            memcpy(huge_string, empty_string, sizeof(REManagedObject));
        }

        const auto str_len = str.length();
        auto huge_string = (SystemString*)&huge_string_data[0];
        huge_string->size = (int32_t)str_len;

        auto out = (SystemString*)sdk::invoke_object_func(huge_string, "Clone", {}).ptr;

        memcpy(out->data, str.data(), str_len * sizeof(wchar_t));

        return out;
    }

    sdk::SystemArray* VM::create_managed_array(::REManagedObject* runtime_type, uint32_t length) {
        static auto system_array_type = sdk::find_type_definition("System.Array");
        static auto create_instance_method = system_array_type->get_method("CreateInstance");

        return create_instance_method->call<sdk::SystemArray*>(sdk::get_thread_context(), runtime_type, length);
    }

    ::REManagedObject* VM::create_sbyte(int8_t value)  {
        static auto sbyte_type = ::sdk::find_type_definition("System.SByte");
        static auto value_field = sbyte_type->get_field("mValue");
        auto new_obj = sbyte_type->create_instance_full();

        if (new_obj == nullptr) {
            return nullptr;
        }

        value_field->get_data<int8_t>(new_obj) = value;
        return new_obj;
    }

    ::REManagedObject* VM::create_byte(uint8_t value) {
        static auto byte_type = ::sdk::find_type_definition("System.Byte");
        static auto value_field = byte_type->get_field("mValue");
        auto new_obj = byte_type->create_instance_full();

        if (new_obj == nullptr) {
            return nullptr;
        }

        value_field->get_data<uint8_t>(new_obj) = value;
        return new_obj;
    }

    ::REManagedObject* VM::create_int16(int16_t value) {
        static auto int16_type = ::sdk::find_type_definition("System.Int16");
        static auto value_field = int16_type->get_field("mValue");
        auto new_obj = int16_type->create_instance_full();

        if (new_obj == nullptr) {
            return nullptr;
        }

        value_field->get_data<int16_t>(new_obj) = value;
        return new_obj;
    }

    ::REManagedObject* VM::create_uint16(uint16_t value) {
        static auto uint16_type = ::sdk::find_type_definition("System.UInt16");
        static auto value_field = uint16_type->get_field("mValue");
        auto new_obj = uint16_type->create_instance_full();

        if (new_obj == nullptr) {
            return nullptr;
        }

        value_field->get_data<uint16_t>(new_obj) = value;
        return new_obj;
    }

    ::REManagedObject* VM::create_int32(int32_t value) {
        static auto int32_type = ::sdk::find_type_definition("System.Int32");
        static auto value_field = int32_type->get_field("mValue");
        auto new_obj = int32_type->create_instance_full();

        if (new_obj == nullptr) {
            return nullptr;
        }

        value_field->get_data<int32_t>(new_obj) = value;
        return new_obj;
    }

    ::REManagedObject* VM::create_uint32(uint32_t value) {
        static auto uint32_type = ::sdk::find_type_definition("System.UInt32");
        static auto value_field = uint32_type->get_field("mValue");
        auto new_obj = uint32_type->create_instance_full();

        if (new_obj == nullptr) {
            return nullptr;
        }

        value_field->get_data<uint32_t>(new_obj) = value;
        return new_obj;
    }

    ::REManagedObject* VM::create_int64(int64_t value) {
        static auto int64_type = ::sdk::find_type_definition("System.Int64");
        static auto value_field = int64_type->get_field("mValue");
        auto new_obj = int64_type->create_instance_full();

        if (new_obj == nullptr) {
            return nullptr;
        }

        value_field->get_data<int64_t>(new_obj) = value;
        return new_obj;
    }

    ::REManagedObject* VM::create_uint64(uint64_t value) {
        static auto uint64_type = ::sdk::find_type_definition("System.UInt64");
        static auto value_field = uint64_type->get_field("mValue");
        auto new_obj = uint64_type->create_instance_full();

        if (new_obj == nullptr) {
            return nullptr;
        }

        value_field->get_data<uint64_t>(new_obj) = value;
        return new_obj;
    }


    ::REManagedObject* VM::create_single(float value) {
        static auto float_type = ::sdk::find_type_definition("System.Single");
        static auto value_field = float_type->get_field("mValue");
        auto new_obj = float_type->create_instance_full();

        if (new_obj == nullptr) {
            return nullptr;
        }

        value_field->get_data<float>(new_obj) = value;
        return new_obj;
    }

    ::REManagedObject* VM::create_double(double value) {
        static auto double_type = ::sdk::find_type_definition("System.Double");
        static auto value_field = double_type->get_field("mValue");
        auto new_obj = double_type->create_instance_full();

        if (new_obj == nullptr) {
            return nullptr;
        }

        value_field->get_data<double>(new_obj) = value;
        return new_obj;
    }

    sdk::InvokeMethod* get_invoke_table() {
        return VM::get_invoke_table();
    }
}


