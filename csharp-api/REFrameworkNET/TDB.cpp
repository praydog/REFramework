#include "API.hpp"

#include "TDB.hpp"

namespace REFrameworkNET {
    TDB^ TDB::Get() {
        return REFrameworkNET::API::API::GetTDB();
    }

    generic <class T>
    reframework::API::TypeDefinition* TDB::GetTypeDefinitionPtr() {
        auto fullName = T::typeid->FullName->Replace("+", ".")->Replace("_System.", "System.");
        auto t = REFrameworkNET::API::GetTDB()->GetType(fullName);

        if (t == nullptr) {
            return nullptr;
        }

        return (reframework::API::TypeDefinition*)t->Ptr();
    }
}