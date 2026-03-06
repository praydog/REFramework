#pragma once

// Runtime dispatch for FirstPerson camera structs that differ between games.
//
// RopewayCameraSystem:
//   RE3 has extra 8-byte pad after cameraControllerInfos → all fields shifted +8.
//   RE2 and RE8 share the same layout.
//
// RopewayPlayerCameraController:
//   RE2 has 8-byte pad at 0xA0 (vs 24 bytes in RE8/RE3) → all fields after shifted -0x10.
//   RE8 and RE3 share the same layout.

#include "GameIdentity.hpp"
#include "ReClass.hpp"

// ---------------------------------------------------------------------------
// RE3-specific RopewayCameraSystem (fields shifted +8 after cameraControllerInfos)
// ---------------------------------------------------------------------------
namespace sdk::re3 {
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

// ---------------------------------------------------------------------------
// RE2-specific RopewayPlayerCameraController (fields shifted -0x10 after worldRotation)
// Verified against RE2 TDB at runtime: activeCamera=0xA8, cameraParam=0xB8
// ---------------------------------------------------------------------------
namespace sdk::re2 {
struct RopewayPlayerCameraController_RE2 : public REBehavior {
    char pad_0048[24];                                                   // 0x0048
    Vector4f pivotPosition;                                              // 0x0060
    Vector4f pivotRotation;                                              // 0x0070
    Vector4f worldPosition;                                              // 0x0080
    Vector4f worldRotation;                                              // 0x0090
    char pad_00A0[8];                                                    // 0x00A0 (8 bytes, vs 24 in RE8)
    class RECamera* activeCamera;                                        // 0x00A8
    class REJoint* joint;                                                // 0x00B0
    class RECameraParam* cameraParam;                                    // 0x00B8
    class ValueTriggerBoolean* N0000772F;                                // 0x00C0
    char pad_00C8[8];                                                    // 0x00C8
    class REBehavior* cascade;                                           // 0x00D0
    class RECameraParam* cameraParam2;                                   // 0x00D8
    char pad_00E0[8];                                                    // 0x00E0
    class DampingFloat* controlDamping;                                  // 0x00E8
    class DampingFloat* controlDamping2;                                 // 0x00F0
    class TwirlerCameraSettings* cameraLimitSettings;                    // 0x00F8
    bool isNotUsingWeapon;                                               // 0x0100
    char pad_0101[7];                                                    // 0x0101
    float pitch;                                                         // 0x0108
    float yaw;                                                           // 0x010C
};
} // namespace sdk::re2

// ---------------------------------------------------------------------------
// Dispatch macros
// ---------------------------------------------------------------------------

// RopewayCameraSystem field dispatch (RE3 shifted +8 vs RE2/RE8).
#define CAMSYS(ptr, field) \
    (sdk::GameIdentity::get().is_re3() \
        ? reinterpret_cast<sdk::re3::RopewayCameraSystem_RE3*>(ptr)->field \
        : (ptr)->field)

// RopewayPlayerCameraController field dispatch (RE2 shifted -0x10 vs RE8/RE3).
// Only use for fields AFTER worldRotation (0x90). Fields at/before 0x90 are identical.
#define CAMCTRL(ptr, field) \
    (sdk::GameIdentity::get().is_re2() \
        ? reinterpret_cast<sdk::re2::RopewayPlayerCameraController_RE2*>(ptr)->field \
        : (ptr)->field)
