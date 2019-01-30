#pragma once

#include "Mod.hpp"
#include "FunctionHook.hpp"

class RECamera;

// Is used to get the camera, which can be used to get all the other components and game objects in the game
class ComponentHook : public Mod {
public:
    ComponentHook();

    void onDrawUI() override;
    void onUpdateTransform(RETransform* transform) override;

protected:
    RECamera* getMainCameraHook_Internal(void* a1);
    static RECamera* getMainCameraHook(void* a1);

    RECamera* m_camera{ nullptr };
    std::string m_componentName{ "pl1000" };

    std::unique_ptr<FunctionHook> m_getMainCameraHook;
};