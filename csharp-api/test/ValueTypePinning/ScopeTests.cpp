#using <REFramework.NET.dll> as_friend
#include "../../REFrameworkNET/ObjectAddress.inl"

using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;

namespace REFrameworkNET::Tests {
public interface class IStorageTest {};

// Separate assembly using normal bridge types and the production ObjectAddress source.
// No engine initialization, native TDB, or fabricated engine pointer is used.
public ref class ValueTypePinning abstract sealed {
    static ValueType^ MakeValue(int expected) {
        auto value = (ValueType^)RuntimeHelpers::GetUninitializedObject(ValueType::typeid);
        ValueType::typeid->GetField("m_data", BindingFlags::Instance | BindingFlags::NonPublic)
            ->SetValue(value, BitConverter::GetBytes(expected));
        return value;
    }

    static void Check(bool condition) {
        if (!condition) throw gcnew InvalidOperationException("Value pinning regression failed");
    }

    static void Compact() {
        GC::Collect(2, GCCollectionMode::Forced, true, true);
        GC::WaitForPendingFinalizers();
    }

    [MethodImpl(MethodImplOptions::NoInlining)]
    static WeakReference^ ExceptionCleanup() {
        auto value = MakeValue(42);
        auto weak = gcnew WeakReference(value->Data);
        try {
            ObjectAddress address(value);
            Compact();
            Check(*(int*)address.Ptr() == 42);
            throw gcnew ArgumentException("expected test exception");
        } catch (ArgumentException^) {}
        return weak;
    }

public:
    static void Run() {
        Check(ValueType::typeid->Assembly != ValueTypePinning::typeid->Assembly);
        Check(ValueType::typeid->Assembly->GetType("REFrameworkNET.Tests.ValueTypePinning") == nullptr);
        int moved = 0;
        for (int i = 0; i < 32; ++i) {
            auto garbage = gcnew array<Byte>(8192);
            auto value = MakeValue(i);
            auto before = value->GetAddress();
            Compact();
            if (before != value->GetAddress()) ++moved;
            GC::KeepAlive(value);
            GC::KeepAlive(garbage);
        }
        Check(moved > 0); // Negative control only compares addresses; never dereferences stale memory.

        for (int i = 0; i < 64; ++i) {
            const int expected = Int32::MinValue + i;
            auto value = MakeValue(expected);
            auto proxy = (IProxy^)AnyProxy<IStorageTest^>::Create(value);
            ObjectAddress direct(value);
            {
                ObjectAddress nested(proxy);
                Check(Object::ReferenceEquals(nested.Instance(), value));
                Check(direct.Address() == nested.Address());
                Compact();
                Check(*(int*)nested.Ptr() == expected);
                *(int*)nested.Ptr() = expected + 1;
            }
            Compact(); // Releasing the inner pin must leave the outer one valid.
            Check(direct.Address() == value->GetAddress());
            Check(*(int*)direct.Ptr() == expected + 1);
        }
        auto released = ExceptionCleanup();
        Compact();
        Check(!released->IsAlive);
        Console::WriteLine("PASS: movable negative control, nested/proxy pins, read/write, exception cleanup");
    }
};
}
