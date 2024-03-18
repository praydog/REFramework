#pragma once

#include <reframework/API.hpp>

#pragma managed

namespace REFrameworkNET {
public ref struct InvokeRet {
    InvokeRet(const reframework::InvokeRet& ret) {
        Bytes = gcnew array<uint8_t>(ret.bytes.size());
        for (size_t i = 0; i < ret.bytes.size(); i++) {
            Bytes[i] = ret.bytes[i];
        }
        Byte = ret.byte;
        Word = ret.word;
        DWord = ret.dword;
        F = ret.f;
        QWord = ret.qword;
        D = ret.d;
        Ptr = gcnew System::UIntPtr(ret.ptr);
        ExceptionThrown = ret.exception_thrown;
    }

    // TODO: improve this? Does .NET have unions?
    property array<uint8_t>^ Bytes;
    property uint8_t Byte;
    property uint16_t Word;
    property uint32_t DWord;
    property float F;
    property uint64_t QWord;
    property double D;
    property System::Object^ Ptr;
    property bool ExceptionThrown;
};
}