#pragma once

// Runtime dispatch for RopewayCameraSystem fields.
// RE3 has an extra 8-byte pad after cameraControllerInfos (0x58),
// shifting all subsequent fields +8 vs RE2/RE8 layout.

#include "GameIdentity.hpp"
#include "ReClass.hpp"

namespace sdk::re3 {
// RE3-specific layout: all fields after cameraControllerInfos are shifted +8.
struct RopewayCameraSystem_RE3 : public REBehavior {
    char pad_0048[8];                                                    // 0x0048
    float motionCameraSpeedScale;                                        // 0x0050
    char pad_0054[4];                                                    // 0x0054
    class DotNetGenericList* cameraControllerInfos;                      // 0x0058
    char pad_0060[8];                                                    // 0x0060 <-- extra pad in RE3
    class RopewayCameraInterpolateSettings* cameraInterpSettings;        // 0x0068
    float rayCastStartOffset;                                            // 0x0070
    float maxViewShieldLength;                                           // 0x0074
    class CollisionSystemAsyncCastRayHandle* castHandleTerrain;          // 0x0078
    class CollisionSystemAsyncCastSphereHandle* castHandleCharacter;     // 0x0080
    class CollisionSystemAsyncCastRayHandle* castHandleAim;              // 0x0088
    class REGameObject* cameraGameObject;                                // 0x0090
    char pad_0098[8];                                                    // 0x0098
    class RopewayPlayerCameraController* cameraController;               // 0x00A0
    class RopewayPlayerCameraController* previousController;             // 0x00A8
    class REManagedObject* N00006F3D;                                    // 0x00B0
    class DampingFloat* damping;                                         // 0x00B8
    class ValueTriggerBoolean* isExistStickInput;                        // 0x00C0
    class RECamera* mainCamera;                                          // 0x00C8
    class DotNetGenericList* fixCameraIdentifiers;                       // 0x00D0
    class DotNetGenericList* cameraGateInfoContainers;                   // 0x00D8
    class RERenderCaptureToTexture* N00006F43;                           // 0x00E0
    class REJoint* playerJoint;                                          // 0x00E8
    class RopewayMainCameraController* mainCameraController;             // 0x00F0
};
} // namespace sdk::re3

// Dispatch a RopewayCameraSystem field access.
// Returns lvalue reference — works for both reads and writes.
// Fields before cameraControllerInfos (ownerGameObject, etc.) don't need dispatch.
#define CAMSYS(ptr, field) \
    (sdk::GameIdentity::get().is_re3() \
        ? reinterpret_cast<sdk::re3::RopewayCameraSystem_RE3*>(ptr)->field \
        : (ptr)->field)
