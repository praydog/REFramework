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
#include <functional>
#include <exception>

#include "TDBVer.hpp"
#include "RETypeDB.hpp"

#if TDB_VER <= 49
#include "regenny/re7/via/clr/VM.hpp"
#endif

namespace sdk {
struct SystemArray;
    
struct REStaticTbl {
    uint8_t** elements;
    uint32_t size;
};

// AKA via.clr.VM
#if TDB_VER <= 49
class VM : public regenny::via::clr::VM {
#else
class VM {
#endif
public:
    static inline uint32_t s_tdb_version{0};
    static VM* get();

public:
    REThreadContext* get_thread_context(int32_t unk = -1);
    sdk::RETypeDB* get_type_db();
    REStaticTbl& get_static_tbl();

    uint8_t* get_static_tbl_for_type(uint32_t type_index);

    static sdk::InvokeMethod* get_invoke_table();
    static SystemString* create_managed_string(std::wstring_view str); // System.String
    static sdk::SystemArray* create_managed_array(::REManagedObject* runtime_type, uint32_t length); // System.Array

    static ::REManagedObject* create_sbyte(int8_t value); // System.SByte
    static ::REManagedObject* create_byte(uint8_t value); // System.Byte
    static ::REManagedObject* create_int16(int16_t value); // System.Int16
    static ::REManagedObject* create_uint16(uint16_t value); // System.UInt16
    static ::REManagedObject* create_int32(int32_t value); // System.Int32
    static ::REManagedObject* create_uint32(uint32_t value); // System.UInt32
    static ::REManagedObject* create_int64(int64_t value); // System.Int64
    static ::REManagedObject* create_uint64(uint64_t value); // System.UInt64
    static ::REManagedObject* create_single(float value); // System.Single
    static ::REManagedObject* create_double(double value); // System.Double

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
    static void safe_wrap(std::string_view function_name, std::function<void()> func);

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