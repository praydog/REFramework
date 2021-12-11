class REThreadContext;
class SystemString;

namespace sdk {
typedef void (*InvokeMethod)(void* stack_frame, void* context);

struct REStaticTbl;
class VM;
class VMContext;

VMContext* get_thread_context(int32_t unk = -1);
InvokeMethod* get_invoke_table();
}


#pragma once

#include <cstdint>
#include <string_view>
#include <exception>

#include "RETypeDB.hpp"

#ifdef RE7
#include "regenny/re7/via/clr/VM.hpp"
#endif

namespace sdk {
struct REStaticTbl {
    uint8_t** elements;
    uint32_t size;
};

// AKA via.clr.VM
#ifdef RE7
class VM : public regenny::via::clr::VM {
#else
class VM {
#endif
public:
    static VM* get();

public:
    REThreadContext* get_thread_context(int32_t unk = -1);
    sdk::RETypeDB* get_type_db();
    REStaticTbl& get_static_tbl();

    uint8_t* get_static_tbl_for_type(uint32_t type_index);

    static sdk::InvokeMethod* get_invoke_table();
    static SystemString* create_managed_string(std::wstring_view str); // System.String

private:
    using ThreadContextFn = REThreadContext* (*)(VM*, int32_t);
    static void update_pointers();

    static VM** s_global_context;
    static sdk::InvokeMethod* s_invoke_tbl;
    static ThreadContextFn s_get_thread_context;

    static int32_t s_static_tbl_offset;
    static int32_t s_type_db_offset;
};

class VMContext : public ::REThreadContext {
public:
    void* unhandled_exception();
    void* local_frame_gc();
    void* end_global_frame();

    void cleanup_after_exception(int32_t old_reference_count);

    class Exception : public std::exception {
    public:
        const char* what() const override {
            return "VMContext::Exception";
        }
    };

    class ScopedTranslator {
    public:
        ScopedTranslator(VMContext* context)
            : m_context{context},
            m_prev_reference_count{context->referenceCount},
            m_old_translator{_set_se_translator(ScopedTranslator::translator)}
        {
        }
        ~ScopedTranslator() {
            _set_se_translator(m_old_translator);
        }

        auto get_prev_reference_count() const {
            return m_prev_reference_count;
        }

        auto get_context() const {
            return m_context;
        }

    private:
        static void translator(unsigned int, struct ::_EXCEPTION_POINTERS*);

        const ::_se_translator_function m_old_translator;
        VMContext* m_context{};
        int32_t m_prev_reference_count{};
    };

private:
    void update_pointers();
};
}