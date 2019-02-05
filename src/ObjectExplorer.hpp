#pragma once

#include <unordered_set>

#include "Address.hpp"
#include "Mod.hpp"

class ObjectExplorer : public Mod {
public:
    ObjectExplorer();

    std::string_view getName() const override { return "ObjectExplorer"; };

    void onDrawUI() override;

private:
    void handleAddress(Address address, int32_t offset = -1);
    void handleGameObject(REGameObject* gameObject);
    void handleTransform(RETransform* transform);
    void handleType(REType* t);

    void makeTreeOffset(REManagedObject* object, uint32_t offset, std::string_view name);
    bool isManagedObject(Address address) const;

    std::string m_objectAddress{ "0" };
};