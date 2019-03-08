#include <sstream>
#include <fstream>
#include <forward_list>

#include <windows.h>

#include "utility/String.hpp"
#include "utility/Scan.hpp"

#include "REFramework.hpp"
#include "ObjectExplorer.hpp"

ObjectExplorer::ObjectExplorer()
{
    m_typeName.reserve(256);
    m_objectAddress.reserve(256);
}

void ObjectExplorer::onDrawUI() {
    ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_Once);

    if (!ImGui::CollapsingHeader(getName().data())) {
        return;
    }

    if (m_doInit) {
        populateClasses();
        populateEnums();
    }

    auto curtime = std::chrono::system_clock::now();

    // List of globals to choose from
    if (ImGui::CollapsingHeader("Singletons")) {
        if (curtime > m_nextRefresh) {
            g_framework->getGlobals()->safeRefresh();
            m_nextRefresh = curtime + std::chrono::seconds(1);
        }

        // make a copy, we want to sort by name
        auto singletons = g_framework->getGlobals()->getObjects();

        // first loop, sort
        std::sort(singletons.begin(), singletons.end(), [](REManagedObject** a, REManagedObject** b) {
            auto aType = utility::REManagedObject::safeGetType(*a);
            auto bType = utility::REManagedObject::safeGetType(*b);

            if (aType == nullptr || aType->name == nullptr) {
                return true;
            }

            if (bType == nullptr || bType->name == nullptr) {
                return false;
            }

            return std::string_view{ aType->name } < std::string_view{ bType->name };
        });

        // Display the nodes
        for (auto obj : singletons) {
            auto t = utility::REManagedObject::safeGetType(*obj);

            if (t == nullptr || t->name == nullptr) {
                continue;
            }

            ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_Once);

            auto madeNode = ImGui::TreeNode(t->name);
            contextMenu(*obj);

            if (madeNode) {
                handleAddress(*obj);
                ImGui::TreePop();
            }
        }
    }

    if (ImGui::CollapsingHeader("Types")) {
        std::vector<uint8_t> fakeType{ 0 };

        for (const auto& name : m_sortedTypes) {
            fakeType.clear();

            auto t = getType(name);

            if (t == nullptr) {
                continue;
            }

            if (t->size >= fakeType.capacity()) {
                fakeType.reserve(t->size);
            }

            handleType((REManagedObject*)fakeType.data(), t);
        }
    }

    if (m_doInit || ImGui::InputText("Type Name", m_typeName.data(), 256)) {
        m_displayedTypes.clear();

        if (auto t = getType(m_typeName.data())) {
            m_displayedTypes.push_back(t);
        }
        else {
            // Search the list for a partial match instead
            for (auto i = std::find_if(m_sortedTypes.begin(), m_sortedTypes.end(), [this](const auto& a) { return a.find(m_typeName.data()) != std::string::npos; });
                i != m_sortedTypes.end();
                i = std::find_if(i + 1, m_sortedTypes.end(), [this](const auto& a) { return a.find(m_typeName.data()) != std::string::npos; }))
            {
                if (auto t = getType(*i)) {
                    m_displayedTypes.push_back(t);
                }
            }
        }
    }

    ImGui::InputText("REObject Address", m_objectAddress.data(), 16, ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsHexadecimal);

    if (m_objectAddress[0] != 0) {
        handleAddress(std::stoull(m_objectAddress, nullptr, 16));
    }

    std::vector<uint8_t> fakeType{ 0 };

    for (auto t : m_displayedTypes) {
        fakeType.clear();
        fakeType.reserve(t->size);
        
        handleType((REManagedObject*)fakeType.data(), t);
    }

    m_doInit = false;
}

void ObjectExplorer::handleAddress(Address address, int32_t offset, Address parent) {
    if (!isManagedObject(address)) {
        return;
    }

    auto object = address.as<REManagedObject*>();
    
    if (parent == nullptr) {
        parent = address;
    }

    bool madeNode = false;
    auto isGameObject = utility::REManagedObject::isA(object, "via.GameObject");

    if (offset != -1) {
        ImGui::SetNextTreeNodeOpen(false, ImGuiCond_::ImGuiCond_Once);

        madeNode = stretchedTreeNode(parent.get(offset), "0x%X:", offset);
        auto isHovered = ImGui::IsItemHovered();
        auto additionalText = std::string{};

        contextMenu(object);

        if (isGameObject) {
            additionalText = utility::REString::getString(address.as<REGameObject*>()->name);
        }
        else {
            // Change name based on VMType
            switch (utility::REManagedObject::getVMType(object)) {
            case via::clr::VMObjType::Array:
            {
                auto arr = (REArrayBase*)object;
                std::string name{};
                name += "Array<";
                name += arr->containedType != nullptr ? arr->containedType->type->name : "";
                name += ">";

                additionalText = name;
                break;
            }

            case via::clr::VMObjType::String:
                additionalText = "String";
                break;
            case via::clr::VMObjType::Delegate:
                additionalText = "Delegate";
                break;
            case via::clr::VMObjType::ValType:
                additionalText = "ValType";
                break;
            case via::clr::VMObjType::Object:
                additionalText = object->info->classInfo->type->name;
                break;
            case via::clr::VMObjType::NULL_:
            default:
                additionalText = "NULL_OBJECT";
                break;
            }
        }

        if (isHovered) {
            makeSameLineText(additionalText, VARIABLE_COLOR_HIGHLIGHT);
        }
        else {
            makeSameLineText(additionalText, VARIABLE_COLOR);
        }
    }

    if (madeNode || offset == -1) {
        if (isGameObject) {
            handleGameObject(address.as<REGameObject*>());
        }

        if (utility::REManagedObject::isA(object, "via.Component")) {
            handleComponent(address.as<REComponent*>());
        }

        handleType(object, utility::REManagedObject::getType(object));

        if (ImGui::TreeNode(object, "AutoGenerated Types")) {
            auto typeInfo = object->info->classInfo->type;
            auto size = utility::REManagedObject::getSize(object);

            for (auto i = (uint32_t)sizeof(void*); i < size; i += sizeof(void*)) {
                auto ptr = Address(object).get(i).to<REManagedObject*>();

                handleAddress(ptr, i, object);
            }

            ImGui::TreePop();
        }
    }

    if (madeNode && offset != -1) {
        ImGui::TreePop();
    }
}

void ObjectExplorer::handleGameObject(REGameObject* gameObject) {
    ImGui::Text("Name: %s", utility::REString::getString(gameObject->name).c_str());
    makeTreeOffset(gameObject, offsetof(REGameObject, transform), "Transform");
    makeTreeOffset(gameObject, offsetof(REGameObject, folder), "Folder");
}

void ObjectExplorer::handleComponent(REComponent* component) {
    makeTreeOffset(component, offsetof(REComponent, ownerGameObject), "Owner");
    makeTreeOffset(component, offsetof(REComponent, childComponent), "ChildComponent");
    makeTreeOffset(component, offsetof(REComponent, prevComponent), "PrevComponent");
    makeTreeOffset(component, offsetof(REComponent, nextComponent), "NextComponent");
}

void ObjectExplorer::handleTransform(RETransform* transform) {

}

void ObjectExplorer::handleType(REManagedObject* obj, REType* t) {
    if (obj == nullptr || t == nullptr) {
        return;
    }

    auto count = 0;

    for (auto typeInfo = t; typeInfo != nullptr; typeInfo = typeInfo->super) {
        auto name = typeInfo->name;

        if (name == nullptr) {
            continue;
        }

        if (!widgetWithContext(typeInfo, [&name]() { return ImGui::TreeNode(name); })) {
            break;
        }

        const auto isRealObject = utility::REManagedObject::isManagedObject(obj);

        // Topmost type
        if (typeInfo == t && isRealObject) {
            ImGui::Text("Size: 0x%X", utility::REManagedObject::getSize(obj));
        }
        // Super types
        else {
            ImGui::Text("Size: 0x%X", typeInfo->size);
        }

        ++count;

        // Display type flags
        if (typeInfo->classInfo != nullptr) {
            if (stretchedTreeNode("TypeFlags")) {
                displayEnumValue("via.clr.TypeFlag", (int64_t)typeInfo->classInfo->typeFlags);
                ImGui::TreePop();
            }
        }

        displayMethods(obj, typeInfo);
        displayFields(obj, typeInfo);
    }

    for (auto i = 0; i < count; ++i) {
        ImGui::TreePop();
    }
}

void ObjectExplorer::displayEnumValue(std::string_view name, int64_t value) {
    auto firstFound = getEnumValueName(name, (int64_t)value);

    if (!firstFound.empty()) {
        ImGui::Text("%i: ", value);
        ImGui::SameLine();
        ImGui::TextColored(VARIABLE_COLOR, "%s", firstFound.c_str());
    }
    // Assume it's a set of flags then
    else {
        ImGui::Text("%i", value);

        std::vector<std::string> names{};

        // Check which bits are set and have enum names
        for (auto i = 0; i < 32; ++i) {
            if (auto bit = (value & (1 << i)); bit != 0) {
                auto valueName = getEnumValueName(name, bit);

                if (valueName.empty()) {
                    continue;
                }

                names.push_back(valueName);
            }
        }

        // Sort and print names
        std::sort(names.begin(), names.end());
        for (const auto& valueName : names) {
            ImGui::TextColored(VARIABLE_COLOR, "%s", valueName.c_str());
        }
    }
}

void ObjectExplorer::displayMethods(REManagedObject* obj, REType* typeInfo) {
    const auto isRealObject = utility::REManagedObject::isManagedObject(obj);
    volatile auto methods = typeInfo->fields->methods;

    if (methods == nullptr || *methods == nullptr) {
        return;
    }

    auto numMethods = typeInfo->fields->num;

    if (ImGui::TreeNode(methods, "Methods: %i", numMethods)) {
        for (auto i = 0; i < numMethods; ++i) {
            volatile auto top = (*methods)[i];

            if (top == nullptr || *top == nullptr) {
                continue;
            }
            //
            auto& holder = **top;
            auto descriptor = holder.descriptor;

            if (descriptor == nullptr || descriptor->name == nullptr) {
                continue;
            }

            auto ret = descriptor->returnTypeName != nullptr ? std::string{ descriptor->returnTypeName } : std::string{ "undefined" };
            auto madeNode = widgetWithContext(descriptor, [&]() { return stretchedTreeNode(descriptor, "%s", ret.c_str()); });
            auto treeHovered = ImGui::IsItemHovered();

            // Draw the variable name with a color
            if (treeHovered) {
                makeSameLineText(descriptor->name, VARIABLE_COLOR_HIGHLIGHT);
            }
            else {
                makeSameLineText(descriptor->name, VARIABLE_COLOR);
            }

            if (madeNode) {
                ImGui::Text("Address: 0x%p", descriptor);
                ImGui::Text("Function: 0x%p", descriptor->functionPtr);

                auto t2 = getType(ret);

                if (t2 == nullptr || t2 == typeInfo) {
                    ImGui::Text("Type: %s", ret.c_str());
                }
                else {
                    std::vector<uint8_t> fakeObject{};
                    fakeObject.reserve(t2->size);
                    fakeObject.clear();

                    handleType((REManagedObject*)fakeObject.data(), t2);
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

void ObjectExplorer::displayFields(REManagedObject* obj, REType* typeInfo) {
    if (typeInfo->fields == nullptr || typeInfo->fields->variables == nullptr || typeInfo->fields->variables->data == nullptr) {
        return;
    }

    const auto isRealObject = utility::REManagedObject::isManagedObject(obj);
    auto descriptors = typeInfo->fields->variables->data->descriptors;

    if (ImGui::TreeNode(typeInfo->fields, "Fields: %i", typeInfo->fields->variables->num)) {
        for (auto i = descriptors; i != descriptors + typeInfo->fields->variables->num; ++i) {
            auto variable = *i;

            if (variable == nullptr) {
                continue;
            }

            auto madeNode = widgetWithContext(variable->function, [&]() { return stretchedTreeNode(variable, "%s", variable->typeName); });
            auto treeHovered = ImGui::IsItemHovered();

            // Draw the variable name with a color
            if (treeHovered) {
                makeSameLineText(variable->name, VARIABLE_COLOR_HIGHLIGHT);
            }
            else {
                makeSameLineText(variable->name, VARIABLE_COLOR);
            }

            // Display the field offset
            if (isRealObject) {
                auto offset = getFieldOffset(obj, variable);

                if (offset != 0) {
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, "0x%X", offset);
                }
            }

            // Info about the field
            if (madeNode) {
                if (isRealObject) {
                    attemptDisplayField(obj, variable);
                }

                if (ImGui::TreeNode(variable, "Additional Information")) {
                    ImGui::Text("Address: 0x%p", variable);
                    ImGui::Text("Function: 0x%p", variable->function);

                    // Display type information
                    if (variable->typeName != nullptr) {
                        auto t2 = getType(variable->typeName);

                        if (t2 == nullptr || t2 == typeInfo) {
                            ImGui::Text("Type: %s", variable->typeName);
                        }
                        else {
                            std::vector<uint8_t> fakeObject{};
                            fakeObject.reserve(t2->size);
                            fakeObject.clear();

                            handleType((REManagedObject*)fakeObject.data(), t2);
                        }
                    }

                    auto typeKind = variable->flags & 0x1F;

                    ImGui::Text("TypeKind: %i (%s)", typeKind, getEnumValueName("via.reflection.TypeKind", (int64_t)typeKind).c_str());
                    ImGui::Text("VarType: %i", variable->variableType);

                    if (variable->staticVariableData != nullptr) {
                        ImGui::Text("GlobalIndex: %i", variable->staticVariableData->variableIndex);
                    }

                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

void ObjectExplorer::attemptDisplayField(REManagedObject* obj, VariableDescriptor* desc) {
    if (desc->function == nullptr) {
        return;
    }

    auto makeTreeAddr = [this](void* addr) {
        if (widgetWithContext(addr, [&]() { return ImGui::TreeNode(addr, "Variable: 0x%p", addr); })) {
            if (isManagedObject(addr)) {
                handleAddress(addr);
            }

            ImGui::TreePop();
        }
    };

    auto typeName = std::string{ desc->typeName };
    auto ret = utility::hash(typeName);
    auto getValueFunc = (void* (*)(VariableDescriptor*, REManagedObject*, void*))desc->function;

    char data[0x100]{ 0 };
    auto typeKind = (via::reflection::TypeKind)(desc->flags & 0x1F);

    // 0x10 == pointer, i think?
    if (typeKind != via::reflection::TypeKind::Class || desc->staticVariableData == nullptr) {
        getValueFunc(desc, obj, &data);

        // yay for compile time string hashing
        switch (ret) {
            // signed 32
        case "u64"_fnv:
            ImGui::Text("%llu", *(int64_t*)&data);
            break;
        case "u32"_fnv:
            ImGui::Text("%i", *(int32_t*)&data);
            break;
        case "s32"_fnv:
            ImGui::Text("%i", *(int32_t*)&data);
            break;
        case "System.Nullable`1<System.Single>"_fnv:
        case "f32"_fnv:
            ImGui::Text("%f", *(float*)&data);
            break;
        case "System.Nullable`1<System.Boolean>"_fnv:
        case "bool"_fnv:
            if (*(bool*)&data) {
                ImGui::Text("true");
            }
            else {
                ImGui::Text("false");
            }
            break;
        case "c16"_fnv:
            if (*(wchar_t**)&data == nullptr) {
                break;
            }

            ImGui::Text("%s", utility::narrow(*(wchar_t**)&data).c_str());
            break;
        case "c8"_fnv:
            if (*(char**)&data == nullptr) {
                break;
            }

            ImGui::Text("%s", *(char**)&data);
            break;
        case "System.Nullable`1<via.vec2>"_fnv:
        case "via.vec2"_fnv:
        {
            auto& vec = *(Vector2f*)&data;
            ImGui::Text("%f %f", vec.x, vec.y);
            break;
        }
        case "System.Nullable`1<via.vec3>"_fnv:
        case "via.vec3"_fnv:
        {
            auto& vec = *(Vector3f*)&data;
            ImGui::Text("%f %f %f", vec.x, vec.y, vec.z);
            break;
        }
        case "via.Quaternion"_fnv:
        {
            auto& quat = *(glm::quat*)&data;
            ImGui::Text("%f %f %f %f", quat.x, quat.y, quat.z, quat.w);
            break;
        }
        case "via.string"_fnv:
            ImGui::Text("%s", utility::REString::getString(*(REString*)&data).c_str());
            break;
        default: 
        {
            if (typeKind == via::reflection::TypeKind::Enum) {
                auto value = *(int32_t*)&data;
                displayEnumValue(typeName, (int64_t)value);
            }
            else {
                makeTreeAddr(*(void**)&data);
            }

            break;
        }
        }
    }
    // Pointer... usually
    else {
        getValueFunc(desc, obj, &data);
        makeTreeAddr(*(void**)&data);
    }
}

int32_t ObjectExplorer::getFieldOffset(REManagedObject* obj, VariableDescriptor* desc) {
    if (desc->typeName == nullptr || desc->function == nullptr || m_offsetMap.find(desc) != m_offsetMap.end()) {
        return m_offsetMap[desc];
    }

    auto ret = utility::hash(std::string{ desc->typeName });

    // These usually modify the object state, not what we want.
    if (ret == "undefined"_fnv) {
        return m_offsetMap[desc];
    }

    // Set up our "translator" to throw on any exception,
    // Particularly access violations.
    // Kind of gross but it's necessary for some fields,
    // because the field function may access the thing we modified, which may actually be a pointer,
    // and we need to handle it.
    _set_se_translator([](uint32_t code, EXCEPTION_POINTERS* exc) {
        switch (code) {
        case EXCEPTION_ACCESS_VIOLATION:
        {
            spdlog::info("ObjectExplorer: Attempting to handle access violation.");

            // idk. best name i could come up with.
            static void** dotNETContext = nullptr;
            static uint8_t* (*getThreadNETContext)(void*, int) = nullptr;

            if (dotNETContext == nullptr) {
                auto ref = utility::scan(g_framework->getModule().as<HMODULE>(), "48 8B 0D ? ? ? ? BA FF FF FF FF E8 ? ? ? ? 48 89 C3");

                if (!ref) {
                    spdlog::info("Unable to find ref. We are going to crash.");
                    break;
                }

                dotNETContext = (void**)utility::calculateAbsolute(*ref + 3);
                getThreadNETContext = (decltype(getThreadNETContext))utility::calculateAbsolute(*ref + 13);

                spdlog::info("g_dotNETContext: {:x}", (uintptr_t)dotNETContext);
                spdlog::info("getThreadNETContext: {:x}", (uintptr_t)getThreadNETContext);
            }

            auto someObject = Address{ getThreadNETContext(*dotNETContext, -1) };

            // This counter needs to be dealt with, it will end up causing a crash later on.
            // We also need to "destruct" whatever object this is.
            if (someObject != nullptr) {
                auto& referenceCount = *someObject.get(0x78).as<int32_t*>();

                spdlog::error("{}", referenceCount);
                if (referenceCount > 1) {
                    --referenceCount;

                    static void* (*func1)(void*) = nullptr;
                    static void* (*func2)(void*) = nullptr;
                    static void* (*func3)(void*) = nullptr;

                    // Get our function pointers
                    if (func1 == nullptr) {
                        spdlog::info("Locating funcs");

                        auto ref = utility::scan(g_framework->getModule().as<HMODULE>(), "48 83 78 18 00 74 ? 48 89 D9 E8 ? ? ? ? 48 89 D9 E8 ? ? ? ?");

                        if (!ref) {
                            spdlog::error("We're going to crash");
                            break;
                        }

                        func1 = Address{ utility::calculateAbsolute(*ref + 11) }.as<decltype(func1)>();
                        func2 = Address{ utility::calculateAbsolute(*ref + 19) }.as<decltype(func2)>();
                        func3 = Address{ utility::calculateAbsolute(*ref + 27) }.as<decltype(func3)>();

                        spdlog::info("F1 {:x}", (uintptr_t)func1);
                        spdlog::info("F2 {:x}", (uintptr_t)func2);
                        spdlog::info("F3 {:x}", (uintptr_t)func3);
                    }

                    // Perform object cleanup that was missed because an exception occurred.
                    if (someObject.get(0x50).deref().get(0x18).deref() != nullptr) {
                        func1(someObject);                   
                    }

                    func2(someObject);
                    func3(someObject);
                }
                else {
                    spdlog::info("Reference count was 0.");
                }
            }
            else {
                spdlog::info("thread context was null. A crash may occur.");
            }
        }
        default:
            break;
        }

        throw std::exception(std::to_string(code).c_str());
    });

    struct BitTester {
        BitTester(uint8_t* oldValue)
            : ptr{ oldValue }
        {
            old = *oldValue;
        }

        ~BitTester() {
            *ptr = old;
        }

        bool isValueSame(const uint8_t* buf) const {
            return buf[0] == ptr[0];
        }

        uint8_t* ptr;
        uint8_t old;
    };

    const auto getValueFunc = (void* (*)(VariableDescriptor*, REManagedObject*, void*))desc->function;
    const auto classSize = utility::REManagedObject::getSize(obj);
    const auto size = 1;

    // Copy the object so we don't cause a crash by replacing
    // data that's being used by the game
    std::vector<uint8_t> objectCopy;
    objectCopy.reserve(classSize);
    memcpy(objectCopy.data(), obj, classSize);

    // Compare data
    for (int32_t i = sizeof(REManagedObject); i + size <= (int32_t)classSize; i += 1) {
        auto ptr = objectCopy.data() + i;
        bool same = true;

        BitTester tester{ ptr };

        // Compare data twice, first run no modifications,
        // second run, slightly modify the data to double check if it's what we want.
        for (int32_t k = 0; k < 2; ++k) {
            std::array<uint8_t, 0x100> data{ 0 };

            // Attempt to get the field value.
            try {
                getValueFunc(desc, (REManagedObject*)objectCopy.data(), data.data());
            }
            // Access violation occurred. Good thing we handle it.
            catch (const std::exception&) {
                same = false;
                break;
            }

            // Check if result is the same at our offset.
            if (!tester.isValueSame(data.data())) {
                same = false;
                break;
            }

            // Modify the data for our second run.
            *ptr ^= 1;
        }

        if (same) {
            m_offsetMap[desc] = i;
            break;
        }
    }

    return m_offsetMap[desc];
}

bool ObjectExplorer::widgetWithContext(void* address, std::function<bool()> widget) {
    auto ret = widget();
    contextMenu(address);

    return ret;
}

void ObjectExplorer::contextMenu(void* address) {
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::Selectable("Copy")) {
            std::stringstream ss;
            ss << std::hex << (uintptr_t)address;

            ImGui::SetClipboardText(ss.str().c_str());
        }

        // Log component hierarchy to disk
        if (isManagedObject(address) && utility::REManagedObject::isA((REManagedObject*)address, "via.Component") && ImGui::Selectable("Log Hierarchy")) {
            auto comp = (REComponent*)address;

            for (auto obj = comp; obj; obj = obj->childComponent) {
                auto t = utility::REManagedObject::safeGetType(obj);

                if (t != nullptr) {
                    if (obj->ownerGameObject == nullptr) {
                        spdlog::info("{:s} ({:x})", t->name, (uintptr_t)obj);
                    }
                    else {
                        auto owner = obj->ownerGameObject;
                        spdlog::info("[{:s}] {:s} ({:x})", utility::REString::getString(owner->name), t->name, (uintptr_t)obj);
                    }
                }

                if (obj->childComponent == comp) {
                    break;
                }
            }
        }

        ImGui::EndPopup();
    }
}

void ObjectExplorer::makeSameLineText(std::string_view text, const ImVec4& color) {
    if (text.empty()) {
        return;
    }

    ImGui::SameLine();
    ImGui::TextColored(color, "%s", text.data());
}

void ObjectExplorer::makeTreeOffset(REManagedObject* object, uint32_t offset, std::string_view name) {
    auto ptr = Address(object).get(offset).to<void*>();

    if (ptr == nullptr) {
        return;
    }

    auto madeNode = ImGui::TreeNode((uint8_t*)object + offset, "0x%X: %s", offset, name.data());

    contextMenu(ptr);

    if (madeNode) {
        handleAddress(ptr);
        ImGui::TreePop();
    }
}

bool ObjectExplorer::isManagedObject(Address address) const {
    return utility::REManagedObject::isManagedObject(address);
}

void ObjectExplorer::populateClasses() {
    static bool doOnce = true;

    if (!doOnce) {
        return;
    }

    auto ref = utility::scan(g_framework->getModule().as<HMODULE>(), "48 8d 0d ? ? ? ? e8 ? ? ? ? 48 8d 05 ? ? ? ? 48 89 03");
    auto& typeList = *(TypeList*)(utility::calculateAbsolute(*ref + 3));
    spdlog::info("TypeList: {:x}", (uintptr_t)&typeList);

    // I don't know why but it can extend past the size.
    for (auto i = 0; i < typeList.numAllocated; ++i) {
        auto t = (*typeList.data)[i];

        if (t == nullptr || IsBadReadPtr(t, sizeof(REType))) {
            continue;
        }

        if (t->name == nullptr) {
            continue;
        }

        auto name = std::string{ t->name };

        if (name.empty()) {
            continue;
        }

        spdlog::info("{:s}", name);
        m_sortedTypes.push_back(name);
        m_types[name] = t;
    }

    std::sort(m_sortedTypes.begin(), m_sortedTypes.end());
}

void ObjectExplorer::populateEnums() {
    std::ofstream outFile("Enums_Internal.hpp");


    auto ref = utility::scan(g_framework->getModule().as<HMODULE>(), "66 C7 40 18 01 01 48 89 05 ? ? ? ?");
    auto& l = *(std::map<uint64_t, REEnumData>*)(utility::calculateAbsolute(*ref + 9));
    spdlog::info("EnumList: {:x}", (uintptr_t)&l);

    spdlog::info("Size: {}", l.size());

    for (auto& elem : l) {
        spdlog::info(" {:x}[ {} {} ]", (uintptr_t)&elem, elem.first, elem.second.name);

        std::string name = elem.second.name;
        std::string nspace = name.substr(0, name.find_last_of("."));
        name = name.substr(name.find_last_of(".") + 1);

        for (auto pos = nspace.find("."); pos != std::string::npos; pos = nspace.find(".")) {
            nspace.replace(pos, 1, "::");
        }


        outFile << "namespace " << nspace << " {" << std::endl;
        outFile << "    enum " << name << " {" << std::endl;

        for (auto node = elem.second.values; node != nullptr; node = node->next) {
            if (node->name == nullptr) {
                continue;
            }

            spdlog::info("     {} = {}", node->name, node->value);
            outFile << "        " << node->name << " = " << node->value << "," << std::endl;

            m_enums.emplace(elem.second.name, EnumDescriptor{ node->name, node->value });
        }

        outFile << "    };" << std::endl;
        outFile << "}" << std::endl;
    }
}

std::string ObjectExplorer::getEnumValueName(std::string_view enumName, int64_t value) {
    auto values = m_enums.equal_range(enumName.data());

    for (auto i = values.first; i != values.second; ++i) {
        if (i->second.value == value) {
            return i->second.name;
        }
    }

    return "";
}

REType* ObjectExplorer::getType(std::string_view typeName) {
    if (typeName.empty()) {
        return nullptr;
    }

    if (auto i = m_types.find(typeName.data()); i != m_types.end()) {
        return i->second;
    }

    return nullptr;
}
