#pragma once

#include <reframework/API.hpp>
#include <vcclr.h>

#pragma managed

namespace REFrameworkNET {
[System::Runtime::InteropServices::StructLayout(System::Runtime::InteropServices::LayoutKind::Explicit, Pack = 1, Size = 129)]
public value struct InvokeRet {
    [System::Runtime::InteropServices::FieldOffset(0), System::Runtime::InteropServices::MarshalAs(
        System::Runtime::InteropServices::UnmanagedType::ByValArray, 
        ArraySubType = System::Runtime::InteropServices::UnmanagedType::U1, 
        SizeConst = 128)
    ]
    unsigned char Bytes;

    [System::Runtime::InteropServices::FieldOffset(0)]
    uint8_t Byte;

    [System::Runtime::InteropServices::FieldOffset(0)]
    uint16_t Word;

    [System::Runtime::InteropServices::FieldOffset(0)]
    uint32_t DWord;

    [System::Runtime::InteropServices::FieldOffset(0)]
    uint64_t QWord;

    [System::Runtime::InteropServices::FieldOffset(0)]
    float Float;

    [System::Runtime::InteropServices::FieldOffset(0)]
    double Double;

    [System::Runtime::InteropServices::FieldOffset(0)]
    uintptr_t Ptr;

    [System::Runtime::InteropServices::FieldOffset(128)]
    bool ExceptionThrown;

    // Method to convert unmanaged InvokeRet to managed InvokeRet
    static InvokeRet FromNative(const reframework::InvokeRet& native) {
        InvokeRet managed;
        pin_ptr<System::Byte> pinned = &managed.Bytes;
        memcpy(pinned, &native.bytes, 129);
        managed.ExceptionThrown = native.exception_thrown;
        return managed;
    }
};
}