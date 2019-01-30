#include "REFramework.hpp"

#include "PositionHooks.hpp"

PositionHooks* g_hook = nullptr;

PositionHooks::PositionHooks()
{
    g_hook = this;
    m_updateTransformHook = std::make_unique<FunctionHook>(Address(GetModuleHandle(0)).get(0xee0dde0), &updateTransformHook);

    spdlog::info("PositionHooks");
}

void* PositionHooks::updateTransformHook_Internal(RETransform* t) {
    auto ret = m_updateTransformHook->getOriginal<decltype(updateTransformHook)>()(t);

    for (auto& mod : g_framework->getMods().getMods()) {
        mod->onUpdateTransform(t);
    }

    return ret;
}

void* PositionHooks::updateTransformHook(RETransform* t) {
    return g_hook->updateTransformHook_Internal(t);
}

