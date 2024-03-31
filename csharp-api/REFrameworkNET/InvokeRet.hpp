#pragma once

#include <reframework/API.hpp>
#include <vcclr.h>

#pragma managed

namespace REFrameworkNET {
public ref struct InvokeRet {
    InvokeRet(const reframework::InvokeRet& ret) {
        using namespace System::Runtime::InteropServices;
        Marshal::Copy(System::IntPtr((void*)&ret), m_invokeRetBytes, 0, sizeof(::reframework::InvokeRet));
    }

    reframework::InvokeRet* Marshal() {
        pin_ptr<uint8_t> pinned_bytes = &m_invokeRetBytes[0];
        uint8_t* bytes = pinned_bytes;

        reframework::InvokeRet* ret = reinterpret_cast<reframework::InvokeRet*>(bytes);

        return ret;
    }

    property System::Span<uint8_t> Bytes {
    public:
        System::Span<uint8_t> get() {
            pin_ptr<uint8_t> pinned_bytes = &m_invokeRetBytes[0];
            uint8_t* bytes = pinned_bytes;
            return System::Span<uint8_t>(bytes, 128);
        }
    };

    property uint8_t Byte {
    public:
        uint8_t get() {
            return m_invokeRetBytes[0];
        }
    }

    property uint16_t Word {
    public:
        uint16_t get() {
            return Marshal()->word;
        }
    }

    property uint32_t DWord {
    public:
        uint32_t get() {
            return Marshal()->dword;
        }
    }
    property float Float {
    public:
        float get() {
            return Marshal()->f;
        }
    }
    property uint64_t QWord {
    public:
        uint64_t get() {
            return Marshal()->qword;
        }
    }

    property double Double {
    public:
        double get() {
            return Marshal()->d;
        }
    }
    property System::Object^ Ptr {
    public:
        System::Object^ get() {
            return System::UIntPtr(Marshal()->ptr);
        }
    }

    property bool ExceptionThrown {
    public:
        bool get() {
            return Marshal()->exception_thrown;
        }
    }

private:
    //::reframework::InvokeRet m_impl;
    array<uint8_t>^ m_invokeRetBytes = gcnew array<uint8_t>(sizeof(::reframework::InvokeRet));
};
}