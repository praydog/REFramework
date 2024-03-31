#pragma once

#include "ManagedObject.hpp"

namespace REFrameworkNET {
// RE Engine's implementation of System.Array
generic <typename T>
public ref class SystemArray : public ManagedObject/*, System::Collections::Generic::IList*/
{
public:
    SystemArray(ManagedObject^ obj) : ManagedObject(obj) { }

protected:

private:
};
}