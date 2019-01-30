#pragma once

#include "Mod.hpp"
#include "FunctionHook.hpp"

class PositionHooks : public Mod {
public:
    PositionHooks();

protected:
    void* updateTransformHook_Internal(RETransform* t);
    static void* updateTransformHook(RETransform* t);

    std::unique_ptr<FunctionHook> m_updateTransformHook;
};