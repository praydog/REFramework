#pragma once

#include "IObject.hpp"

namespace REFrameworkNET {
ref class ObjectEnumerator : public System::Collections::IEnumerator {
private:
    int position = -1;
    IObject^ nativeObject;

public:
    ObjectEnumerator(IObject^ nativeObject) {
        this->nativeObject = nativeObject;
    }

    // IEnumerator implementation
    virtual bool MoveNext() {
        int itemCount = GetItemCount();
        if (position < itemCount - 1) {
            position++;
            return true;
        }
        return false;
    }

    virtual void Reset() {
        position = -1;
    }

    virtual property System::Object^ Current {
        System::Object^ get() {
            if (position == -1 || position >= GetItemCount()) {
                throw gcnew System::InvalidOperationException();
            }

            System::Object^ result = nullptr;
            if (nativeObject->HandleInvokeMember_Internal("get_Item", gcnew array<System::Object^>{ position }, result)) {
                return result;
            }

            return nullptr;
        }
    }

private:
    int GetItemCount() {
        //return nativeObject->Invoke("get_Count", gcnew array<System::Object^>{})->DWord;
        System::Object^ result = nullptr;

        if (nativeObject->HandleInvokeMember_Internal("get_Count", gcnew array<System::Object^>{}, result)) {
            return (int)result;
        }

        if (nativeObject->HandleInvokeMember_Internal("get_Length", gcnew array<System::Object^>{}, result)) {
            return (int)result;
        }

        return 0;}
};
}