#include "API.hpp"

#include "TDB.hpp"

namespace REFrameworkNET {
    TDB^ TDB::Get() {
        return REFrameworkNET::API::API::GetTDB();
    }

    generic <class T>
    reframework::API::TypeDefinition* TDB::GetTypeDefinitionPtr() {
        auto t = REFrameworkNET::API::GetTDB()->GetType(T::typeid->FullName->Replace("+", "."));

        if (t == nullptr) {
            return nullptr;
        }

        return (reframework::API::TypeDefinition*)t->Ptr();
    }
}