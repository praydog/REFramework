#pragma once

#include <chrono>

#include "Mod.hpp"

class FirstPerson : public Mod {
public:
    FirstPerson();

    void onFrame() override;
    void onDrawUI() override;
    void onComponent(REComponent* component) override;
    void onUpdateTransform(RETransform* transform) override;

protected:
    // gross
    bool listBoxHandlerAttach(void* data, int idx, const char** outText) {
        *outText = ((decltype(m_attachNames)*)data)->at(idx).data();
        return true;
    }

private:
    void reset();

    std::string m_attachBoneImgui{ "head" };
    std::wstring m_attachBone{ L"head" };
    Vector3f m_attachOffset{ 0.263f, 0.4f, 0.975f };
    Matrix3x3f m_rotationOffset{ };

    float m_scale{ 25.0f };

    RETransform* m_playerTransform{ nullptr };
    RECamera* m_camera{ nullptr };
    RECamera* m_playerCameraController{ nullptr };
    RopewayCameraSystem* m_cameraSystem{ nullptr };

    bool m_inEventCamera{ false };
    bool m_resetView{ false };

    std::chrono::high_resolution_clock::time_point m_lastFrame;

    std::vector<std::string> m_attachNames;
    int32_t m_attachSelected{ 0 };
};