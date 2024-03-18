#include "TypeDefinition.hpp"

#include "ManagedObject.hpp"

namespace REFrameworkNET {
    TypeDefinition^ ManagedObject::GetTypeDefinition() {
        auto result = m_object->get_type_definition();

        if (result == nullptr) {
            return nullptr;
        }

        return gcnew TypeDefinition(result);
    }
}