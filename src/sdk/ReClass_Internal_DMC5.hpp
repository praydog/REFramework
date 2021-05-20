// Created with ReClass.NET 1.1 by KN4CK3R


class N0000000A
{
public:
    char pad_0000[24]; //0x0000
    class N0000005F *N0000000E; //0x0018
    char pad_0020[280]; //0x0020
}; //Size: 0x0138

class N0000005F
{
public:
    char pad_0000[16]; //0x0000
    class N0000007A *N00000062; //0x0010
    class N00000085 *N00000063; //0x0018
    char pad_0020[168]; //0x0020
}; //Size: 0x00C8

class N0000007A
{
public:
    char pad_0000[40]; //0x0000
    uint32_t N00000080; //0x0028
    char pad_002C[28]; //0x002C
}; //Size: 0x0048

class N00000085
{
public:
    char pad_0000[32]; //0x0000
    uint32_t N0000008A; //0x0020
    char pad_0024[36]; //0x0024
}; //Size: 0x0048

class ManagedTypes
{
public:
    class REClassInfo *types[2048]; //0x0000
}; //Size: 0x4000

class N0000019E
{
public:
    char pad_0000[104]; //0x0000
    class N00000369 *N000001AC; //0x0068
    class N000001C9 *N000001AD; //0x0070
    char pad_0078[208]; //0x0078
}; //Size: 0x0148

class N000001C9
{
public:
    class N0000019E *parent; //0x0000
    void* N000001CB; //0x0008
    char pad_0010[40]; //0x0010
    void* N000001D1; //0x0038
    char pad_0040[8]; //0x0040
    void* N000001D3; //0x0048
    char pad_0050[16]; //0x0050
    class N000001FF *N000001D6; //0x0060
    char pad_0068[8]; //0x0068
}; //Size: 0x0070

class N000001FF
{
public:
    char pad_0000[120]; //0x0000
}; //Size: 0x0078

class N0000027C
{
public:
    class N000002D1 *N0000027D; //0x0000
    char pad_0008[104]; //0x0008
    class N000002A6 *N0000028B; //0x0070
    char pad_0078[200]; //0x0078
}; //Size: 0x0140

class N000002A6
{
public:
    char pad_0000[328]; //0x0000
}; //Size: 0x0148

class N000002D1
{
public:
    char pad_0000[72]; //0x0000
}; //Size: 0x0048

class N00000369
{
public:
    class N0000039C *N0000036A; //0x0000
    char pad_0008[320]; //0x0008
}; //Size: 0x0148

class N0000039C
{
public:
    char pad_0000[72]; //0x0000
}; //Size: 0x0048

class CameraTypePtr
{
public:
    class REType *N000003AA; //0x0000
}; //Size: 0x0008

class REType
{
public:
    void *N000003B4; //0x0000
    uint32_t classIndex; //0x0008
    int16_t flags; //0x000C < 0 == NoneType, 1 == abstract, 2 == concrete, 256 == interface, 16384 == root
    char pad_000E[2]; //0x000E
    uint64_t fastClassIndex; //0x0010
    uint32_t typeIndexProbably; //0x0018
    char pad_001C[4]; //0x001C
    char *name; //0x0020
    char pad_0028[4]; //0x0028
    uint32_t size; //0x002C
    uint32_t typeCRC; //0x0030
    uint32_t miscFlags; //0x0034
    class REType *super; //0x0038
    class REType *childType; //0x0040
    class REType *chainType; //0x0048
    class REFieldList *fields; //0x0050
    class REClassInfo *classInfo; //0x0058 is a managed type if this is not null
}; //Size: 0x0060

class N000003DE
{
public:
    char pad_0000[8]; //0x0000
    class N000008E5 *N000003E0; //0x0008
    char pad_0010[312]; //0x0010
}; //Size: 0x0148

class REObject
{
public:
    class REObjectInfo *info; //0x0000
}; //Size: 0x0008

class REManagedObject : public REObject
{
public:
    uint32_t referenceCount; //0x0008
    int16_t N000071AE; //0x000C
    char pad_000E[2]; //0x000E
}; //Size: 0x0010

class REComponent : public REManagedObject
{
public:
    class REGameObject *ownerGameObject; //0x0010
    class REComponent *childComponent; //0x0018
    class REComponent *prevComponent; //0x0020
    class REComponent *nextComponent; //0x0028
}; //Size: 0x0030

class RECamera : public REComponent
{
public:
    float nearClipPlane; //0x0030
    float farClipPlane; //0x0034
    float fov; //0x0038
    float lookAtDistance; //0x003C
    bool verticalEnable; //0x0040
    char pad_0041[3]; //0x0041
    float aspectRatio; //0x0044
    int32_t N00000451; //0x0048
    uint32_t projectionType; //0x004C
    int32_t cameraType; //0x0050
    char pad_0054[12]; //0x0054
    wchar_t *cameraName; //0x0060
    uint32_t N00000455; //0x0068
    char pad_006C[32]; //0x006C
    float N00000D40; //0x008C
    float N0000045A; //0x0090
    float N00000D43; //0x0094
    float N0000045B; //0x0098
    char pad_009C[4]; //0x009C
}; //Size: 0x00A0

class REString
{
public:
    char pad_0000[24]; //0x0000
    int32_t length; //0x0018 if len >= 12, is a pointer
    int32_t maxLength; //0x001C
}; //Size: 0x0020

class REGameObject : public REManagedObject
{
public:
    uint16_t N0000047E; //0x0010
    bool shouldUpdate; //0x0012
    bool shouldDraw; //0x0013
    bool shouldUpdateSelf; //0x0014
    bool shouldDrawSelf; //0x0015
    bool shouldSelect; //0x0016
    char pad_0017[1]; //0x0017
    class RETransform *transform; //0x0018
    class REFolder *folder; //0x0020
    class REString name; //0x0028 This can either be a pointer to the name or embedded directly
    uint32_t N00000DDA; //0x0048
    float timescale; //0x004C
    char pad_0050[16]; //0x0050
}; //Size: 0x0060

class REFieldList
{
public:
    uint32_t unknown; //0x0000
    char pad_0004[4]; //0x0004
    class REFieldList *next; //0x0008
    class FunctionHolder **(*methods)[4000]; //0x0010
    int32_t num; //0x0018
    int32_t maxItems; //0x001C
    class REVariableList *variables; //0x0020
    void* N0000072F; //0x0028
    uint32_t N00000730; //0x0030
    char pad_0034[4]; //0x0034
}; //Size: 0x0038

class N0000074B
{
public:
    class FunctionHolder **N0000074C[2048]; //0x0000
}; //Size: 0x4000

class N00000756
{
public:
    class FunctionHolder *func; //0x0000
}; //Size: 0x0008

class FunctionHolder
{
public:
    class FunctionDescriptor *descriptor; //0x0000
    char pad_0008[24]; //0x0008
}; //Size: 0x0020

class FunctionDescriptor
{
public:
    char *name; //0x0000
    class MethodParamInfo(*params)[256]; //0x0008
    char pad_0010[4]; //0x0010
    int32_t numParams; //0x0014
    void* functionPtr; //0x0018
    uint32_t returnTypeFlag; //0x0020 AND 1F = via::reflection::TypeKind
    uint32_t typeIndex; //0x0024
    char *returnTypeName; //0x0028
    char pad_0030[8]; //0x0030
}; //Size: 0x0038

class REJointArray {
public:
    class N00003745* data;         // 0x0000
    int32_t size;                  // 0x0008
    int32_t numAllocated;          // 0x000C
    class JointMatrices* matrices; // 0x0010
    char pad_0018[256];            // 0x0018
};                                 // Size: 0x0118

class RETransform : public REComponent
{
public:
    Vector4f position; //0x0030
    Vector4f rotation; //0x0040
    Vector4f scale; //0x0050
    class REScene *scene; //0x0060
    class RETransform *child; //0x0068
    class RETransform *next; //0x0070
    class RETransform *parentTransform; //0x0078
    Matrix4x4f worldTransform; //0x0080
    class N00007EEE *N000007D8; //0x00C0
    int32_t N00000804; //0x00C8
    uint32_t tickCount; //0x00CC
    bool sameJointsConstraint; //0x00D0
    bool N0000081A; //0x00D1
    bool N00000814; //0x00D2
    bool absoluteScaling; //0x00D3
    char pad_00D4[4]; //0x00D4
    class REJointArray joints; //0x00D8
    char pad_01F0[8]; //0x01F0
}; //Size: 0x01F8

class N0000B6D9
{
public:
    class REManagedObject *object; //0x0000
    uint32_t v1; //0x0008
    uint32_t v2; //0x000C
    uint32_t v3; //0x0010
    uint32_t v4; //0x0014
}; //Size: 0x0018

class REScene : public REManagedObject
{
public:
    char pad_0010[3]; //0x0010
    bool enabled; //0x0013
    bool monitor; //0x0014
    bool mainScene; //0x0015
    char pad_0016[6]; //0x0016
    uint32_t frameCount; //0x001C
    float timescale; //0x0020
    char pad_0024[4]; //0x0024
    class N0000B6D9 someHugeArray[65536]; //0x0028
    char pad_180028[1176]; //0x180028
    class RETransform *N0000B786; //0x1804C0
    class REFolder *firstFolder; //0x1804C8
    class REString name; //0x1804D0
    char pad_1804F0[720]; //0x1804F0
    int64_t N0000B7E3; //0x1807C0
    class REManagedObject *N0000B7E4; //0x1807C8
}; //Size: 0x1807D0

class N0000091E
{
public:
    class N0000092A *N00000920; //0x0000
    char pad_0008[56]; //0x0008
}; //Size: 0x0040

class N0000092A
{
public:
    char pad_0000[72]; //0x0000
}; //Size: 0x0048

class N00000965
{
public:
    class REObjectInfo *object; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class REObjectInfo
{
public:
    class REClassInfo *classInfo; //0x0000
    void* N00000991; //0x0008
    void* getType; //0x0010
    void* N00000993; //0x0018
    void* N00000994; //0x0020
    void* N00000995; //0x0028
    void* N00000996; //0x0030
    void* N00000997; //0x0038
    void* N00000998; //0x0040
    void* N00000999; //0x0048
    void* getSize; //0x0050
    void* N0000099B; //0x0058
    void* N0000099C; //0x0060
}; //Size: 0x0068

class REClassInfo
{
public:
    uint16_t typeIndex; //0x0000 index into global type array
    char pad_0002[30]; //0x0002
    uint32_t typeFlags; //0x0020 System::Reflection::TypeAttributes or via::clr::TypeFlag
    char pad_0024[2]; //0x0024
    uint8_t objectType; //0x0026 1 == normal type ? ??
    char pad_0027[5]; //0x0027
    uint32_t elementSize; //0x002C
    uint32_t size; //0x0030
    char pad_0034[44]; //0x0034
    class N00002B03 *N000009C1; //0x0060
    class REType *type; //0x0068
    class REObjectInfo *parentInfo; //0x0070
}; //Size: 0x0078

class N00000CF1
{
public:
    class REObjectInfo *N00000CF2; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class RERenderOutput : public REComponent
{
public:
    char pad_0030[144]; //0x0030
}; //Size: 0x00C0

class N00000D1D
{
public:
    class REObjectInfo *object; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class REFolder : public REManagedObject
{
public:
    char pad_0010[8]; //0x0010
    int32_t N00000D4E; //0x0018
    char pad_001C[8]; //0x001C
    float N00005BA5; //0x0024
    class REString name; //0x0028
    class REString name2; //0x0048
    class REString name3; //0x0068
    class REFolder *parentFolder; //0x0088
    class REFolder *childFolder; //0x0090
    class REFolder *childFolder2; //0x0098
    char pad_00A0[40]; //0x00A0
    class REScene *scene; //0x00C8
}; //Size: 0x00D0

class N00000D61
{
public:
    class REObjectInfo *N00000D62; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class N00000DFF
{
public:
    class REObjectInfo *N00000E00; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class N00000E15
{
public:
    class REObjectInfo *N00000E16; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class N00000E2B
{
public:
    class REObjectInfo *N00000E2C; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class N00004041
{
public:
    class REObjectInfo *N00004042; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class ASDF
{
public:
    class RETransform *N0000406D; //0x0000
    char pad_0008[80]; //0x0008
}; //Size: 0x0058

class SceneObjectList : public ASDF
{
public:
    char pad_0058[48]; //0x0058
}; //Size: 0x0088

class N00004087
{
public:
    class REObjectInfo *N00004089; //0x0000
    char pad_0008[56]; //0x0008
}; //Size: 0x0040

class REToneMapping : public REComponent
{
public:
    char pad_0030[16]; //0x0030
}; //Size: 0x0040

class REBehavior : public REComponent
{
public:
    uint8_t N000076CE; //0x0030
    bool N000017DF; //0x0031
    bool enabled; //0x0032
    uint8_t N00000836; //0x0033
    uint32_t N00007712; //0x0034
    uint32_t N000076CF; //0x0038
    uint32_t N0000705D; //0x003C update cost?
    char pad_0040[8]; //0x0040
}; //Size: 0x0048

class RECameraParam : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    uint32_t N000076C0; //0x0050
    float fov; //0x0054 fov
    float param2; //0x0058
    float farClip; //0x005C far clip dist
    bool useParam; //0x0060
    char pad_0061[15]; //0x0061
}; //Size: 0x0070

class RopewayPlayerCameraController : public REBehavior
{
public:
    char pad_0048[24]; //0x0048
    Vector4f pivotPosition; //0x0060
    Vector4f pivotRotation; //0x0070
    Vector4f worldPosition; //0x0080
    Vector4f worldRotation; //0x0090 quaternion
    char pad_00A0[8]; //0x00A0
    class RECamera *activeCamera; //0x00A8
    class REJoint *joint; //0x00B0
    class RECameraParam *cameraParam; //0x00B8
    class ValueTriggerBoolean *N0000772F; //0x00C0
    char pad_00C8[8]; //0x00C8
    class REBehavior *cascade; //0x00D0
    class RECameraParam *cameraParam2; //0x00D8
    char pad_00E0[8]; //0x00E0
    class DampingFloat *controlDamping; //0x00E8
    class DampingFloat *controlDamping2; //0x00F0
    class TwirlerCameraSettings *cameraLimitSettings; //0x00F8
    bool isNotUsingWeapon; //0x0100 IDK, but it gets set to true when not using a weap.
    char pad_0101[7]; //0x0101
    float pitch; //0x0108
    float yaw; //0x010C
    class DampingFloat *controlDamping3; //0x0110
    class DampingFloat *controlDamping4; //0x0118
    char pad_0120[24]; //0x0120
    class DampingFloat *controlDamping5; //0x0138
    uint32_t cameraMode; //0x0140 3 == in weapon?
    char pad_0144[4]; //0x0144
    class RopewayCameraInterpolationPrame *N0000773F; //0x0148
    char pad_0150[72]; //0x0150
    uint32_t N00007748; //0x0198
    char pad_019C[60]; //0x019C
}; //Size: 0x01D8

class REJoint : public REManagedObject
{
public:
    class RETransform *parentTransform; //0x0010
    class REJointDesc *info; //0x0018
    Vector4f posOffset; //0x0020
    Vector3f anglesOffset; //0x0030
    float N000026BC; //0x003C
    float N000026B6; //0x0040
    float N000026BF; //0x0044
    float N000026B7; //0x0048
    char pad_004C[4]; //0x004C
    int32_t N00006E8E; //0x0050
    float N00006E97; //0x0054
    int32_t N00006E8F; //0x0058
    uint32_t N0000B861; //0x005C
}; //Size: 0x0060

class ValueTriggerBoolean : public REManagedObject
{
public:
    bool current; //0x0010
    bool old; //0x0011
    char pad_0012[2]; //0x0012
    uint32_t N0000785B; //0x0014
}; //Size: 0x0018

class DampingFloat : public REManagedObject
{
public:
    float current; //0x0010
    float source; //0x0014
    float dampingTime; //0x0018
    float dampingRate; //0x001C
    char pad_0020[16]; //0x0020
    uint32_t N00007885; //0x0030
    float target; //0x0034
    float N00007886; //0x0038
    float N00007891; //0x003C
    float progressRate; //0x0040
}; //Size: 0x0044

class TwirlerCameraSettings : public REManagedObject
{
public:
    wchar_t *settingsPath; //0x0010
    char pad_0018[16]; //0x0018
    uint32_t N000078BE; //0x0028
    uint32_t N000078DE; //0x002C
    float N000078BF; //0x0030
    char pad_0034[4]; //0x0034
    class REAnimationCurve *animationCurve; //0x0038
    class REAnimationCurve *animationCurve2; //0x0040
    float N000078C2; //0x0048
    float N000078E4; //0x004C
    float N000078C3; //0x0050
    float N000078E7; //0x0054
    float cameraSensitivity; //0x0058
    char pad_005C[20]; //0x005C
    float sensitivityScale; //0x0070
    char pad_0074[4]; //0x0074
}; //Size: 0x0078

class REAnimationCurve : public REManagedObject
{
public:
    uint32_t length; //0x0010
    float loopStartTime; //0x0014
    float loopEndTime; //0x0018
    float minValue; //0x001C
    float maxValue; //0x0020
    uint16_t loopWrapNo; //0x0024
    char pad_0026[2]; //0x0026
    bool enableClamp; //0x0028
    char pad_0029[3]; //0x0029
    float N00007918; //0x002C
    class CurveKeyFrames *keyframes; //0x0030
    class CurveKeyFrames *keyframes2; //0x0038
    char pad_0040[16]; //0x0040
}; //Size: 0x0050

class REActorMotion : public REComponent
{
public:
    class REMotion *motion; //0x0030
    char pad_0038[16]; //0x0038
    class N00007A09 *N00007950; //0x0048
    char pad_0050[32]; //0x0050
}; //Size: 0x0070

class REMotion : public REComponent
{
public:
    char pad_0030[120]; //0x0030
    class N000026F0 *N0000797C; //0x00A8
    char pad_00B0[48]; //0x00B0
    uint32_t N00007983; //0x00E0
    uint8_t N000079F6; //0x00E4
    char pad_00E5[115]; //0x00E5
    class N00007C3D *N00007992; //0x0158
    char pad_0160[32]; //0x0160
    class JointList *joints; //0x0180
    uint32_t numJoints; //0x0188
    char pad_018C[532]; //0x018C
}; //Size: 0x03A0

class N00007A09
{
public:
    class REActorLayer *N00007A0A; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class REActorLayer : public REManagedObject
{
public:
    char pad_0010[56]; //0x0010
}; //Size: 0x0048

class N00007E99
{
public:
    Matrix4x4f N00007E9A; //0x0000
}; //Size: 0x0040

class JointList
{
public:
    class N00007E99 joints[2048]; //0x0000
}; //Size: 0x20000

class N00007C3D
{
public:
    char pad_0000[32]; //0x0000
}; //Size: 0x0020

class N00007EEE
{
public:
    class N00003730 *N00007EEF; //0x0000
    char pad_0008[128]; //0x0008
}; //Size: 0x0088

class DotNetString_Implementation : public REManagedObject
{
public:
    class REManagedObject *N000036B9; //0x0010
    char pad_0018[8]; //0x0018
}; //Size: 0x0020

class DotNetGenericList : public REManagedObject
{
public:
    class REArrayThing *data; //0x0010
    char pad_0018[72]; //0x0018
}; //Size: 0x0060

class N00003730
{
public:
    char pad_0000[136]; //0x0000
}; //Size: 0x0088

class MethodParams {
public:
    char pad_0000[48]; // 0x0000
    void*** in_data;   // 0x0030 can point to data
    void** out_data;   // 0x0038 can be whatever, can be a dword, can point to data
    void* object_ptr;  // 0x0040 aka "this" pointer
};                     // Size: 0x0048

class N00003745
{
public:
    class REJoint *joints[2048]; //0x0000
}; //Size: 0x4000

class N0000379E
{
public:
    Matrix4x4f worldMatrix; //0x0000
}; //Size: 0x0040

class JointMatrices
{
public:
    class N0000379E data[2048]; //0x0000
}; //Size: 0x20000

class REJointDesc
{
public:
    wchar_t *name; //0x0000
    uint32_t nameHash; //0x0008
    int16_t parentJoint; //0x000C minus 1
    int16_t jointNumber; //0x000E
    Vector4f offset; //0x0010
    float N000037B4; //0x0020
    float N0000377B; //0x0024
    float N00003770; //0x0028
    float N0000377E; //0x002C
    float N00003771; //0x0030
    float N00003781; //0x0034
    float N00003772; //0x0038
    float N00003784; //0x003C
}; //Size: 0x0040

class PhysicsCollidableBase : public REComponent
{
public:
    char pad_0030[32]; //0x0030
}; //Size: 0x0050

class PhysicsCharacterController : public PhysicsCollidableBase
{
public:
    uint32_t N00006EB8; //0x0050
    int32_t movementFlags; //0x0054
    float height; //0x0058
    float radius; //0x005C
    uint32_t N00006EBA; //0x0060
    uint32_t N00001828; //0x0064
    float N00006EBB; //0x0068
    float slopeLimit; //0x006C
    float volumeGrowth; //0x0070
    float movementThreshold; //0x0074
    float N00006EE9; //0x0078
    float slideMovementLimit; //0x007C
    char pad_0080[8]; //0x0080
    int32_t N00006EEB; //0x0088
    char pad_008C[4]; //0x008C
    Vector3f gravityDir; //0x0090
    char pad_009C[20]; //0x009C
    uint16_t groupId; //0x00B0
    char pad_00B2[2]; //0x00B2
    bool localMode; //0x00B4
    char pad_00B5[1]; //0x00B5
    bool ignoreRotation; //0x00B6
    bool ignoreParentRotation; //0x00B7
    char pad_00B8[136]; //0x00B8
    Vector3f position; //0x0140
    char pad_014C[156]; //0x014C
    class PhysicsCollidable *collidable; //0x01E8
    class PhysicsFilterInfo *filterInfo; //0x01F0
    char pad_01F8[56]; //0x01F8
}; //Size: 0x0230

class RopewayCameraSystem : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    float motionCameraSpeedScale; //0x0050
    char pad_0054[1]; //0x0054
    uint8_t N000009D7; //0x0055
    char pad_0056[2]; //0x0056
    class DotNetGenericList *cameraControllerInfos; //0x0058
    class RopewayCameraInterpolateSettings *cameraInterpSettings; //0x0060
    float rayCastStartOffset; //0x0068
    float maxViewShieldLength; //0x006C
    class CollisionSystemAsyncCastRayHandle *castHandleTerrain; //0x0070
    class CollisionSystemAsyncCastSphereHandle *castHandleCharacter; //0x0078
    class CollisionSystemAsyncCastRayHandle *castHandleAim; //0x0080
    class REGameObject *cameraGameObject; //0x0088
    char pad_0090[8]; //0x0090
    class RopewayPlayerCameraController *cameraController; //0x0098
    class RopewayPlayerCameraController *previousController; //0x00A0
    class REManagedObject *N00006F3D; //0x00A8
    class DampingFloat *damping; //0x00B0
    class ValueTriggerBoolean *isExistStickInput; //0x00B8
    class RECamera *mainCamera; //0x00C0
    class DotNetGenericList *fixCameraIdentifiers; //0x00C8
    class DotNetGenericList *cameraGateInfoContainers; //0x00D0
    class RERenderCaptureToTexture *N00006F43; //0x00D8
    class REJoint *playerJoint; //0x00E0
    class RopewayMainCameraController *mainCameraController; //0x00E8
    bool isUseWideMonitor; //0x00F0
    char pad_00F1[7]; //0x00F1
    class REManagedObject *N00003EEC; //0x00F8
    bool N00003EED; //0x0100
    char pad_0101[39]; //0x0101
    class DotNetGenericList *stringList; //0x0128
    char pad_0130[8]; //0x0130
}; //Size: 0x0138

class N000070F5
{
public:
    Matrix3x4f localMatrix; //0x0000
}; //Size: 0x0030

class RopewayIkController : public REBehavior
{
public:
    char pad_0048[12]; //0x0048
    uint8_t updateTiming; //0x0054
    char pad_0055[3]; //0x0055
    uint8_t enableIkBits; //0x0058
    char pad_0059[3]; //0x0059
    int8_t defaultSkipIkBits; //0x005C
    char pad_005D[3]; //0x005D
    class REArrayThing *N000071C9; //0x0060
    class REArrayThing *N000071CA; //0x0068
    int32_t ikLegKind; //0x0070
    float fooatAlignBorderAngle; //0x0074
    class SystemString *someString; //0x0078
    bool useSkipIkForLeg; //0x0080
    char pad_0081[3]; //0x0081
    int32_t skipIkForLegBits; //0x0084
    bool useFootLockForLeg; //0x0088
    bool unlockFootOnAdjustTerrain; //0x0089
    bool unlockFootOnPressing; //0x008A
    bool unlockFootOnMotionInterpolating; //0x008B
    bool unlockFootOnPreventeringPenetrate; //0x008C
    char pad_008D[3]; //0x008D
    int16_t monitoringMotionLayerNoForLeg; //0x0090
    char pad_0092[2]; //0x0092
    int32_t spineKind; //0x0094
    char pad_0098[8]; //0x0098
    class SystemString *someString2; //0x00A0
    char pad_00A8[40]; //0x00A8
    class SystemString *someString3; //0x00D0
    float armDampingRate; //0x00D8
    float armDampingTime; //0x00DC
    float defaultArmActivateTime; //0x00E0
    float defaultArmResetTime; //0x00E4
    float armRayRadius; //0x00E8
    float armRayIntervalFrame; //0x00EC
    int32_t wristKind; //0x00F0
    char pad_00F4[4]; //0x00F4
    class N0000A9C1 *armFitIkUserData; //0x00F8
    char pad_0100[8]; //0x0100
    int32_t ikWristSolveMode; //0x0108
    int32_t liftUpWristOnGround; //0x010C
    bool useSkipIkForWrist; //0x0110
    char pad_0111[3]; //0x0111
    int32_t skipIkForWristBits; //0x0114
    float skipIkDampingRateForWrist; //0x0118
    bool useFootLockForWrist; //0x011C
    char pad_011D[3]; //0x011D
    int32_t footLockOptionBitsForWrist; //0x0120
    int16_t monitoringMotionLayerNoForWrist; //0x0124
    char pad_0126[2]; //0x0126
    bool setupped; //0x0128
    bool setuppedHumanRetargetIk; //0x0129
    char pad_012A[2]; //0x012A
    int32_t legStepStatus; //0x012C
    bool alignedFoot; //0x0130
    char pad_0131[3]; //0x0131
    float alignedFootAngle; //0x0134
    char pad_0138[4]; //0x0138
    bool alignedForeFoot; //0x013C
    char pad_013D[3]; //0x013D
    float N0000A953; //0x0140
    int32_t appliedSkipIkForLegBits; //0x0144
    int32_t appliedFootLockForLegBits; //0x0148
    char pad_014C[4]; //0x014C
    void *lookAtTarget; //0x0150
    bool autoLookAtEnable; //0x0158
    char pad_0159[15]; //0x0159
    class REArrayThing *N0000A958; //0x0168
    int32_t appliedSkipIkForWristBits; //0x0170
    int32_t appliedFootLockForWristBits; //0x0174
    class IkControlStatusArray *controlStatuses; //0x0178
    char pad_0180[16]; //0x0180
    class REManagedObject *N0000A95D; //0x0190
    char pad_0198[24]; //0x0198
    class REManagedObject *N0000A961; //0x01B0
    class REManagedObject *N0000A962; //0x01B8
    char pad_01C0[8]; //0x01C0
    class ObjectPointer *jointPtr; //0x01C8
    char pad_01D0[216]; //0x01D0
}; //Size: 0x02A8

class UserData : public REManagedObject
{
public:
    class REString name; //0x0010
}; //Size: 0x0030

class N0000A9C1 : public UserData
{
public:
    class REManagedObject *N0000A9C3; //0x0030
}; //Size: 0x0038

class SystemString : public REManagedObject
{
public:
    int32_t size; //0x0010
    wchar_t data[256]; //0x0014
}; //Size: 0x0214

class N0000AA93 : public REManagedObject
{
public:
    class REClassInfo *N0000AA95; //0x0010
    char pad_0018[112]; //0x0018
}; //Size: 0x0088

class N0000AAAC : public REManagedObject
{
public:
    char pad_0010[48]; //0x0010
}; //Size: 0x0040

class ObjectPointer : public REManagedObject
{
public:
    class REClassInfo *classInfo; //0x0010
    uint32_t num1; //0x0018
    uint32_t num2; //0x001C
    class REManagedObject *object; //0x0020
    char pad_0028[112]; //0x0028
}; //Size: 0x0098

class REVariableList
{
public:
    char pad_0000[8]; //0x0000
    class N0000ADA4 *data; //0x0008
    int32_t num; //0x0010
    int32_t maxItems; //0x0014
}; //Size: 0x0018

class N0000ADA4
{
public:
    class VariableDescriptor *descriptors[256]; //0x0000
}; //Size: 0x0800

class VariableDescriptor
{
public:
    char *name; //0x0000
    char pad_0008[4]; //0x0008
    uint16_t N00000871; //0x000C
    uint16_t N00008140; //0x000E
    void* function; //0x0010
    int32_t flags; //0x0018 (flags AND 0x1F) gives var type (via::clr::reflection::TypeKind)
    uint32_t flags2; //0x001C
    char *typeName; //0x0020
    char pad_0028[4]; //0x0028
    uint32_t variableType; //0x002C 1 == pointer? 3 == builtin?
    class StaticVariableDescriptor *staticVariableData; //0x0030
    char pad_0038[8]; //0x0038
}; //Size: 0x0040

class N0000B627
{
public:
    char pad_0000[8]; //0x0000
    class N0000B632 *N0000B629; //0x0008
    char pad_0010[56]; //0x0010
}; //Size: 0x0048

class N0000B632
{
public:
    char pad_0000[136]; //0x0000
}; //Size: 0x0088

class REMotionStructure : public REComponent
{
public:

}; //Size: 0x0030

class SkeletonResourceHandle
{
public:
    class N0000B89B *N0000B899; //0x0000
}; //Size: 0x0008

class JointDescDTbl
{
public:
    class JointDescData *data; //0x0000
    uint32_t num; //0x0008
    char pad_000C[4]; //0x000C
}; //Size: 0x0010

class REMotionDummySkeleton : public REMotionStructure
{
public:
    class SkeletonResourceHandle skeletonResourceHandle; //0x0030
    class JointDescDTbl jointDescTbl; //0x0038
    char pad_0048[8]; //0x0048
}; //Size: 0x0050

class N0000B89B
{
public:
    wchar_t name[64]; //0x0008

    virtual void Function0();
    virtual void Function1();
    virtual void Function2();
    virtual void Function3();
    virtual void Function4();
    virtual void Function5();
    virtual void Function6();
    virtual void Function7();
    virtual void Function8();
    virtual void Function9();
}; //Size: 0x0088

class JointDescData
{
public:
    class REJointDesc data[256]; //0x0000
}; //Size: 0x4000

class StaticVariableDescriptor
{
public:
    uint16_t ownerTypeIndex; //0x0000
    char pad_0002[6]; //0x0002
    uint32_t variableIndex; //0x0008 index into some global array...?
    uint16_t N00000839; //0x000C
    int16_t N000009F5; //0x000E
    uint32_t staticVariableOffset; //0x0010 of
    char pad_0014[1]; //0x0014
}; //Size: 0x0015

class REAnimation : public REComponent
{
public:
    char pad_0030[28]; //0x0030
    float N0000BA25; //0x004C
    float N0000B970; //0x0050
    float N0000BA28; //0x0054
    char pad_0058[32]; //0x0058
}; //Size: 0x0078

class REMotionCamera : public REAnimation
{
public:
    char pad_0078[56]; //0x0078
    bool enabledConstraints; //0x00B0
    char pad_00B1[815]; //0x00B1
    Vector4f position; //0x03E0
    Vector4f orientation; //0x03F0
    char pad_0400[48]; //0x0400
    bool interpolating; //0x0430
    char pad_0431[19]; //0x0431
}; //Size: 0x0444

class RopewayMainCameraController : public REBehavior
{
public:
    char pad_0048[9]; //0x0048
    bool updateCamera; //0x0051
    char pad_0052[6]; //0x0052
    class RopewayCameraStatus *cameraStatus; //0x0058
    Vector4f cameraObjectPosition; //0x0060
    Vector4f cameraObjectRotation; //0x0070
    Vector4f cameraPosition; //0x0080
    Vector4f cameraRotation; //0x0090
    char pad_00A0[4]; //0x00A0
    float fov; //0x00A4
    bool controllerEnabled; //0x00A8
    char pad_00A9[3]; //0x00A9
    float switchBusyTime; //0x00AC
    float switchInterpolationTime; //0x00B0
    char pad_00B4[4]; //0x00B4
    class REAnimationCurve *N00000817; //0x00B8
    class DotNetGenericList *cameraShakes; //0x00C0
    class REGameObject *mainCameraObject; //0x00C8
    class RECamera *mainCamera; //0x00D0
    class REJoint *N0000081B; //0x00D8
    class AppliedCameraShakeParam *appliedCameraShakeParam; //0x00E0
    char pad_00E8[8]; //0x00E8
}; //Size: 0x00F0

class N00000878
{
public:
    char pad_0000[8]; //0x0000
}; //Size: 0x0008

class N0000087A : public N00000878
{
public:

}; //Size: 0x0008

class N000008E5
{
public:
    char pad_0000[328]; //0x0000
}; //Size: 0x0148

class AppliedCameraShakeParam : public REManagedObject
{
public:
    char pad_0010[48]; //0x0010
}; //Size: 0x0040

class FieldInfoPtr
{
public:
    class FieldInfo(*N00000A1A)[2000000]; //0x0000
}; //Size: 0x0008

class FieldInfo
{
public:
    uint16_t ownerType; //0x0000
    int16_t indexIntoSomeOtherArray; //0x0002
    uint16_t N0000133D; //0x0004
    uint16_t N00001380; //0x0006
    char pad_0008[8]; //0x0008
    uint8_t N0000135D; //0x0010
    char pad_0011[7]; //0x0011
    void* get; //0x0018
}; //Size: 0x0020

class FieldInfoArray
{
public:
    class FieldInfo N00000A1D[2000000]; //0x0000
}; //Size: 0x3D09000

class TypeListArrayPtr
{
public:
    class TypeListArray *N00000A47; //0x0000
}; //Size: 0x0008

class TypeListArray
{
public:
    class REClassInfo N00000A51[100000]; //0x0000
}; //Size: 0xB71B00

class GlobalArrayData2
{
public:
    char pad_0000[104]; //0x0000
    class N00000A8A *N00000A6A; //0x0068
    class REObjectInfo *objectInfo; //0x0070
}; //Size: 0x0078

class N00000A8A
{
public:
    char pad_0000[72]; //0x0000
}; //Size: 0x0048

class SomeGlobalArrayThingPtr
{
public:
    class N00000AA4 *N00000A9B; //0x0000
}; //Size: 0x0008

class N00000CB2
{
public:
    char pad_0000[8]; //0x0000
}; //Size: 0x0008

class N00001242
{
public:
    char pad_0000[56]; //0x0000
}; //Size: 0x0038

class ContainerThing
{
public:
    class N00001283 *data; //0x0000
    uint32_t size; //0x0008
    char pad_000C[4]; //0x000C
}; //Size: 0x0010

class N00000AA4
{
public:
    class N00000CB2 N00000AA7[127]; //0x0008
    char pad_0400[280]; //0x0400
    class N00001242 N00000ACB[256]; //0x0518
    char pad_3D18[88]; //0x3D18
    class ContainerThing staticVariableLists[6]; //0x3D70
    char pad_3DD0[15008]; //0x3DD0

    virtual void Function0();
    virtual void Function1();
    virtual void Function2();
    virtual void Function3();
    virtual void Function4();
    virtual void Function5();
    virtual void Function6();
    virtual void Function7();
    virtual void Function8();
    virtual void Function9();
}; //Size: 0x7870

class N00001283
{
public:
    class StaticVariables *N00001284[100000]; //0x0000
}; //Size: 0xC3500

class StaticVariables
{
public:
    char pad_0000[72]; //0x0000
}; //Size: 0x0048

class StaticVariables_RopewayCameraSystem
{
public:
    int32_t N000012C8; //0x0000
    int32_t N000012D1; //0x0004
    float N000012C9; //0x0008
    float N000012D4; //0x000C
    float N000012CA; //0x0010
    float N000012D7; //0x0014
    float N000012CB; //0x0018
    char pad_001C[2148]; //0x001C
}; //Size: 0x0880

class RERenderLayer : public REManagedObject
{
public:
    char pad_0010[104]; //0x0010
}; //Size: 0x0078

class RenderBounds
{
public:
    Vector2f top; //0x0000
    Vector2f bottom; //0x0008
}; //Size: 0x0010

class RERenderLayerScene : public RERenderLayer
{
public:
    class RECamera *camera; //0x0078
    char pad_0080[8]; //0x0080
    class RenderTargetState *targetStates[16]; //0x0088
    char pad_0108[4544]; //0x0108
    class RenderBounds renderBounds[3]; //0x12C8
    char pad_12F8[224]; //0x12F8
}; //Size: 0x13D8

class RenderTargetState
{
public:
    char pad_0008[56]; //0x0008

    virtual void Function0();
    virtual void Function1();
    virtual void Function2();
    virtual void Function3();
    virtual void Function4();
    virtual void Function5();
    virtual void Function6();
    virtual void Function7();
    virtual void Function8();
    virtual void Function9();
}; //Size: 0x0040

class N000026F0
{
public:
    char pad_0000[76]; //0x0000
    float N0000270E; //0x004C
    class REJoint *joint; //0x0050
    char pad_0058[48]; //0x0058
}; //Size: 0x0088

class RopewaySystemRingBufferController : public REBehavior
{
public:
    char pad_0048[128]; //0x0048
}; //Size: 0x00C8

class RopewayIlluminationManagerPtr
{
public:
    class RopewayIlluminationManager *N000013CC; //0x0000
}; //Size: 0x0008

class RopewayIlluminationManager : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    class REManagedObject *N000013E2; //0x0050
    class DotNetGenericDictionary *mapIdsToIlluminationContainer; //0x0058
    uint32_t shouldUseFlashlight; //0x0060
    uint32_t someCounter; //0x0064
    bool shouldUseFlashlight2; //0x0068
    char pad_0069[7]; //0x0069
    class REManagedObject *N000013FB; //0x0070
    class DotNetGenericList *N000013F4; //0x0078
}; //Size: 0x0080

class RopewayStayAreaController : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    uint32_t N00001410; //0x0050
    uint32_t N00001444; //0x0054
    uint32_t areaId; //0x0058
    uint32_t N0000143A; //0x005C
    uint32_t N00001412; //0x0060
    uint32_t N00001446; //0x0064
    uint32_t previousAreaId; //0x0068
    uint32_t N0000143D; //0x006C
    uint32_t areaId3; //0x0070
    uint32_t N00001441; //0x0074
    uint32_t areaId4; //0x0078
    char pad_007C[116]; //0x007C
}; //Size: 0x00F0

class RopewayCameraSystemPtr
{
public:
    class RopewayCameraSystem *N000013EC; //0x0000
}; //Size: 0x0008

class CameraControllerList : public REManagedObject
{
public:
    char pad_0010[16]; //0x0010
    class RopewayCameraControllerInfo *N00001407[64]; //0x0020
}; //Size: 0x0220

class RopewayCameraControllerInfo : public REManagedObject
{
public:
    char pad_0010[8]; //0x0010
    class REGameObject *controllerObject; //0x0018
    char pad_0020[8]; //0x0020
}; //Size: 0x0028

class DotNetGenericDictionary : public REManagedObject
{
public:
    class N00001440 *N00001434; //0x0010
    class N0000144E *N00001435; //0x0018
    char pad_0020[16]; //0x0020
    class REManagedObject *equalityComparer; //0x0030
}; //Size: 0x0038

class N00001440 : public REManagedObject
{
public:
    char pad_0010[64]; //0x0010
}; //Size: 0x0050

class N0000144E : public REManagedObject
{
public:
    char pad_0010[64]; //0x0010
}; //Size: 0x0050

class RopewayCameraInterpolationPrame : public REManagedObject
{
public:
    class REGameObject *owner; //0x0010
    class RopewayCameraPositionParam *posParam1; //0x0018
    class RopewayCameraPositionParam *posParam2; //0x0020
    class DampingFloat *dampingFloat; //0x0028
}; //Size: 0x0030

class RopewayCameraPositionParam : public REManagedObject
{
public:
    class REGameObject *attachedOwner; //0x0010
    class REJoint *attachedJoint; //0x0018
    class RopewayCameraTransitionParam *transitionParam; //0x0020
}; //Size: 0x0028

class RopewayCameraTransitionParam : public REManagedObject
{
public:
    class RopewayPlayerCameraParam *playerCameraParam; //0x0010
    char pad_0018[136]; //0x0018
}; //Size: 0x00A0

class RopewayPlayerCameraParam : public REManagedObject
{
public:
    class REAnimationCurve *curve1; //0x0010
    Vector2f vec; //0x0018
    class REAnimationCurve *curve2; //0x0020
    class REAnimationCurve *curve3; //0x0028
    class SystemString *name; //0x0030
    char pad_0038[104]; //0x0038
}; //Size: 0x00A0

class RopewaySetPostEffectParam : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    uint32_t N0000156E; //0x0050
    uint32_t N000015AE; //0x0054
    char pad_0058[24]; //0x0058
    class DotNetGenericDictionary *N00001572; //0x0070
    char pad_0078[56]; //0x0078
}; //Size: 0x00B0

class RopewayPostEffectController : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    class REAnimationCurve *curve1; //0x0050
    class REAnimationCurve *curve2; //0x0058
    class DotNetGenericList *N000015B6; //0x0060
    char pad_0068[24]; //0x0068
}; //Size: 0x0080

class RESecondaryAnimation : public REComponent
{
public:
    class REMotion *motion; //0x0030
    char pad_0038[4]; //0x0038
    int16_t priority; //0x003C
    char pad_003E[2]; //0x003E
    int32_t updateTiming; //0x0040
    bool enabled; //0x0044
    char pad_0045[3]; //0x0045
}; //Size: 0x0048

class ActorLayerList
{
public:
    class N00001630 *data; //0x0000
    uint32_t numLayers; //0x0008
    uint32_t numAllocated; //0x000C
}; //Size: 0x0010

class REActorMotionCamera : public RESecondaryAnimation
{
public:
    class ActorLayerList layers; //0x0048
}; //Size: 0x0058

class N00001630
{
public:
    class REActorLayer *layers[2048]; //0x0000
}; //Size: 0x4000

class RopewaySweetLightController : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    uint32_t N00001470; //0x0050
    uint32_t N0000152A; //0x0054
    uint32_t N00001471; //0x0058
    uint32_t N0000152D; //0x005C
    class RERenderSpotLight *renderSpotLight; //0x0060
    class RERenderProjectionSpotlight *renderProjectionSpotLight; //0x0068
    class RopewaySpotLightParam *param1; //0x0070
    class RopewaySpotLightParam *param2; //0x0078
    char pad_0080[8]; //0x0080
}; //Size: 0x0088

class RERenderLight : public REComponent
{
public:
    Vector4f color; //0x0030
    char pad_0040[24]; //0x0040
    float brightness; //0x0058
    char pad_005C[4]; //0x005C
    float N000014A3; //0x0060
    float idkLodOrSomething; //0x0064
    char pad_0068[4]; //0x0068
    uint32_t N0000152E; //0x006C
    char pad_0070[48]; //0x0070
    Vector3f activeColor; //0x00A0
    float N00001546; //0x00AC
    char pad_00B0[192]; //0x00B0
}; //Size: 0x0170

class RERenderSpotLight : public RERenderLight
{
public:
    char pad_0170[456]; //0x0170
    float N0000150F; //0x0338
    float N00001597; //0x033C
    float radius; //0x0340
    float illuminanceThreshold; //0x0344
    float cone; //0x0348
    float N0000159D; //0x034C
    char pad_0350[48]; //0x0350
}; //Size: 0x0380

class RERenderProjectionSpotlight : public RERenderSpotLight
{
public:
    char pad_0380[16]; //0x0380
}; //Size: 0x0390

class RopewaySpotLightParam : public REManagedObject
{
public:
    char pad_0010[64]; //0x0010
}; //Size: 0x0050

class RopewaySweetLightManager : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    class SweetLightContextContainer *contexts; //0x0050
    class RopewaySweetLightParam *param1; //0x0058
    class RopewaySweetLightParam *param2; //0x0060
    uint8_t N0000154E; //0x0068
    char pad_0069[7]; //0x0069
    class REManagedObject *N0000154F; //0x0070
}; //Size: 0x0078

class RopewaySweetLightParam : public REManagedObject
{
public:
    class RopewaySpotLightParam *spotlight; //0x0010
}; //Size: 0x0018

class RopewaySweetLightManagerContext : public REManagedObject
{
public:
    class RopewaySweetLightController *controller; //0x0010
    class RopewaySpotLightParam *param1; //0x0018
    class RopewaySpotLightParam *param2; //0x0020
    char pad_0028[8]; //0x0028
    class DotNetGenericList *N000015B1; //0x0030
    class RopewaySpotLightParam *param3; //0x0038
    char pad_0040[8]; //0x0040
}; //Size: 0x0048

class N00002B03
{
public:
    char pad_0000[192]; //0x0000
}; //Size: 0x00C0

class REArrayBase : public REManagedObject
{
public:
    class REClassInfo *containedType; //0x0010
    int32_t num1; //0x0018
    int32_t numElements; //0x001C
}; //Size: 0x0020

class SweetLightContextContainer : public REArrayBase
{
public:
    class RopewaySweetLightManagerContext *data[2]; //0x0020
}; //Size: 0x0030

class REArrayThing : public REArrayBase
{
public:
    char pad_0020[72]; //0x0020
}; //Size: 0x0068

class RopewaySurvivorCharacterController : public REBehavior
{
public:
    char pad_0048[16]; //0x0048
    class REJoint *constJoint; //0x0058
    class DampingFloat *radius; //0x0060
    class DampingFloat *height; //0x0068
    class DampingVec3 *offset; //0x0070
    uint32_t offsetType; //0x0078
    char pad_007C[4]; //0x007C
    class DotNetGenericList *controllerRequests; //0x0080
    char pad_0088[8]; //0x0088
    class PhysicsCharacterController *physicsCharacterController; //0x0090
    class PhysicsRequestSetCollider *physicsRequestSetCollider; //0x0098
    class RopewaySurvivorCharacterControllerUserData *userData; //0x00A0
    float N00001647; //0x00A8
    char pad_00AC[4]; //0x00AC
    class PhysicsCollider *collider; //0x00B0
}; //Size: 0x00B8

class DampingVec3 : public REManagedObject
{
public:
    Vector3f N000015BC; //0x0010
    char pad_001C[20]; //0x001C
    float N000015BF; //0x0030
    float N000015DB; //0x0034
    char pad_0038[16]; //0x0038
    uint32_t N000015C2; //0x0048
    char pad_004C[4]; //0x004C
    Vector3f target; //0x0050
    char pad_005C[4]; //0x005C
    float N000015C4; //0x0060
    float N000015EA; //0x0064
    char pad_0068[8]; //0x0068
    float N000015C8; //0x0070
    char pad_0074[4]; //0x0074
}; //Size: 0x0078

class PhysicsRequestSetCollider : public PhysicsCollidableBase
{
public:
    char pad_0050[56]; //0x0050
}; //Size: 0x0088

class RopewaySurvivorCharacterControllerUserData : public UserData // what a NAME
{
public:
    class REPtrArray *data; //0x0030
    float N00001634; //0x0038
    float N0000165B; //0x003C
}; //Size: 0x0040

class REPtrArray : public REArrayBase
{
public:
    class REManagedObject *data[2048]; //0x0020
}; //Size: 0x4020

class SurvivorCharacterControllerUserDataParam : public REManagedObject
{
public:
    char pad_0010[8]; //0x0010
    class SystemString *string1; //0x0018
    char pad_0020[8]; //0x0020
    class SystemString *string2; //0x0028
    class SurvivorControllerUserDataShape *shape; //0x0030
}; //Size: 0x0038

class SurvivorControllerUserDataShape : public REManagedObject
{
public:
    class SystemString *jointName; //0x0010
    uint32_t N000016B6; //0x0018
    char pad_001C[20]; //0x001C
    float f1; //0x0030
    float f2; //0x0034
    uint32_t N000016BA; //0x0038
    char pad_003C[4]; //0x003C
}; //Size: 0x0040

class PhysicsColliderVector
{
public:
    class N000017C9 *data; //0x0000
    uint32_t numElements; //0x0008
    uint32_t numAllocated; //0x000C
}; //Size: 0x0010

class PhysicsCollidable : public REManagedObject
{
public:
    uint32_t N000016F4; //0x0010
    char pad_0014[4]; //0x0014
    class PhysicsCapsuleShape *shape; //0x0018
    class PhysicsCapsuleShape *transformedShape; //0x0020
    char pad_0028[8]; //0x0028
    class PhysicsFilterInfo *filterInfo; //0x0030
    class PhysicsUserData *userData; //0x0038
    class REGameObject *gameObject; //0x0040
    uint32_t N000016FB; //0x0048
    uint32_t N000017B7; //0x004C
    uint32_t N000016FC; //0x0050
    char pad_0054[4]; //0x0054
    class PhysicsColliderVector colliderControllers; //0x0058 interesting
    char pad_0068[16]; //0x0068
}; //Size: 0x0078

class PhysicsCollider : public PhysicsCollidable
{
public:
    bool updateShape; //0x0078
    char pad_0079[287]; //0x0079
}; //Size: 0x0198

class PhysicsShape : public REManagedObject
{
public:
    char pad_0010[24]; //0x0010
    class PhysicsCollider *parentCollider; //0x0028
    char pad_0030[16]; //0x0030
    Vector4f vec1; //0x0040
    Vector4f vec2; //0x0050
}; //Size: 0x0060

class PhysicsConvexShape : public PhysicsShape
{
public:

}; //Size: 0x0060

class PhysicsCapsuleShape : public PhysicsConvexShape
{
public:
    Vector4f vec3; //0x0060
    Vector4f vec4; //0x0070
    float radius; //0x0080
    float N0000176C; //0x0084
    float N00001734; //0x0088
    char pad_008C[4]; //0x008C
}; //Size: 0x0090

class PhysicsFilterInfo : public REManagedObject
{
public:
    uint32_t layer; //0x0010
    uint32_t group; //0x0014
    uint32_t subGroup; //0x0018
    uint32_t ignoreSubGroup; //0x001C
    int32_t maskBits; //0x0020
    char pad_0024[12]; //0x0024
}; //Size: 0x0030

class N000017C9
{
public:
    class REBehavior *element[2048]; //0x0000
}; //Size: 0x4000

class RopewaySurvivorTargetBankController : public REBehavior
{
public:
    char pad_0048[16]; //0x0048
    class REMotion *motion; //0x0058
    char pad_0060[32]; //0x0060
    class RopewaySurvivorPlayerCondition *playerCondition; //0x0080
}; //Size: 0x0088

class RopewaySurvivorPlayerCondition : public REBehavior
{
public:
    char pad_0048[12]; //0x0048
    int32_t survivorType; //0x0054
    bool ignoreBlow; //0x0058
    bool ignoreGrapple; //0x0059
    char pad_005A[6]; //0x005A
    class RopewaySurvivorDefineDamageParam *damageParam; //0x0060
    bool isEvent; //0x0068
    bool isLight; //0x0069
    bool isCombat; //0x006A
    bool isTension; //0x006B
    bool isTense; //0x006C
    bool useTranceiver; //0x006D
    bool forceUseFlashlight; //0x006E
    bool manuallyLight; //0x006F
    float autoHealStartTimer; //0x0070
    float autoHealTimer; //0x0074
    float poisonTimer; //0x0078
    float poisonAutoRecoveryTimer; //0x007C
    float combatTimer; //0x0080
    float coughTimer; //0x0084
    float warmTimer; //0x0088
    float unknownfloat; //0x008C
    class RopewayTimer *lightSwitchTimer; //0x0090
    class RopewayTimer *burnTimer; //0x0098
    class SurvivorConditionTimerTrigger *wetTimerTrigger; //0x00A0
    class SurvivorConditionTimerTrigger *dryTimerTrigger; //0x00A8
    class REGameObject *flashlight; //0x00B0
    char pad_00B8[40]; //0x00B8
    bool exceptional; //0x00E0
    char pad_00E1[3]; //0x00E1
    int32_t wallMaterial; //0x00E4
    class REJoint *playerJoint; //0x00E8
    char pad_00F0[8]; //0x00F0
    class SystemAction *actionVital; //0x00F8
    class RopewaySurvivorPlayerController *playerController; //0x0100
    class RopewaySurvivorPlayerActionOrderer *actionOrderer; //0x0108
    char pad_0110[16]; //0x0110
    class RopewaySurvivorInventory *inventory; //0x0120
    class RopewaySurvivorEquipment *equipment; //0x0128
    class REMotion *playerMotion; //0x0130
    char pad_0138[136]; //0x0138
    class ActionTriggerInt *hitPointVitalTrigger; //0x01C0
    class ActionTriggerInt *situationTrigger; //0x01C8
    bool onFlashlight; //0x01D0
    bool N00005FF6; //0x01D1
    char pad_01D2[134]; //0x01D2
    bool isPoison; //0x0258
    char pad_0259[3]; //0x0259
    float dopingTimer; //0x025C
    char pad_0260[64]; //0x0260
}; //Size: 0x02A0

class RopewayHandHeldItem : public REBehavior
{
public:
    char pad_0048[64]; //0x0048
}; //Size: 0x0088

class RopewayFlashLight : public RopewayHandHeldItem
{
public:
    char pad_0088[168]; //0x0088
    Vector4f targetPosition; //0x0130
    char pad_0140[60]; //0x0140
}; //Size: 0x017C

class RopewayTimerBase : public REManagedObject
{
public:
    int32_t mode; //0x0010
    float timeLimit; //0x0014
}; //Size: 0x0018

class RopewayTimer : public RopewayTimerBase
{
public:
    bool completeTrigger; //0x0018
    char pad_0019[3]; //0x0019
    int32_t functionType; //0x001C
    float transitTime; //0x0020
    bool completed; //0x0024
    char pad_0025[3]; //0x0025
    int16_t completedCount; //0x0028
    char pad_002A[2]; //0x002A
}; //Size: 0x002C

class RopewaySurvivorInventory : public REBehavior
{
public:
    char pad_0048[64]; //0x0048
    class RopewaySurvivorPlayerCondition *parentCondition; //0x0088
    char pad_0090[56]; //0x0090
}; //Size: 0x00C8

class RopewaySurvivorEquipment : public REBehavior
{
public:
    int32_t equipPartsForm; //0x0048
    char pad_004C[8]; //0x004C
    int32_t equipType; //0x0054
    int32_t forceEquipType; //0x0058
    char pad_005C[4]; //0x005C
    class RopewayImplementArm *mainWeapon; //0x0060
    class RopewayImplementArm *subWeapon; //0x0068
    class RopewayImplementGun *equippedWeapon; //0x0070
    class RopewayArmList *arms; //0x0078
    int32_t rapidFireNumber; //0x0080
    char pad_0084[12]; //0x0084
    class REManagedObject *valTrigger; //0x0090
    char pad_0098[104]; //0x0098
    class RopewaySurvivorPlayerCondition *parentCondition; //0x0100
    char pad_0108[24]; //0x0108
    float currentAccuracy; //0x0120 ReticleFitPoint
    bool isPerfectAccuracy; //0x0124 IsReticleFit
    char pad_0125[3]; //0x0125
}; //Size: 0x0128

class RopewayArmList : public REManagedObject
{
public:
    class RopewayArmList_Impl *data; //0x0010
}; //Size: 0x0018

class RopewayArmList_Impl : public REArrayBase
{
public:
    class RopewayImplementArm *arms[2048]; //0x0020
}; //Size: 0x4020

class RopewayImplement : public REBehavior
{
public:
    char pad_0048[56]; //0x0048
    class REJoint *joint; //0x0080
    class RopewayImplementVirtualJoint *virtualJoint; //0x0088
    class RERenderMesh *mesh; //0x0090
    class REMotion *implementMotion; //0x0098
    class REMotion *parentMotion; //0x00A0
    class RopewayStayAreaController *stayAreaController; //0x00A8
    class RopewayIkController *ikController; //0x00B0
    class RopewayCharacterHandler *characterHandler; //0x00B8
    class RopewayMotionEventHandler *implementMotionHandler; //0x00C0
    class RopewayMotionEventHandler *parentMotionHandler; //0x00C8
    class RopewayWwiseContainerApp *wwiseContainerApp; //0x00D0
    char pad_00D8[8]; //0x00D8
    class RopewaySurvivorEquipment *parentEquipment; //0x00E0
    class UserData *implementUserData; //0x00E8
}; //Size: 0x00F0

class RopewayImplementArm : public RopewayImplement
{
public:
    char pad_00F0[88]; //0x00F0
}; //Size: 0x0148

class RopewayCharacterHandler : public REBehavior
{
public:
    char pad_0048[184]; //0x0048
    class DotNetGenericDictionary *N00001A9D; //0x0100
    class DotNetGenericDictionary *N00001A9E; //0x0108
    class DotNetGenericDictionary *N00001A9F; //0x0110
    class DotNetGenericDictionary *N00001AA0; //0x0118
    char pad_0120[16]; //0x0120
    class PhysicsRequestSetCollider *requestSetCollider; //0x0130
    class RopewayIkController *ikController; //0x0138
    class RopewayPressController *pressController; //0x0140
    class RopewayGroundFixer *groundFixer; //0x0148
    class RopewayPlRainEffect *rainEffect; //0x0150
    char pad_0158[8]; //0x0158
    class REDynamicsRagdoll *ragdoll; //0x0160
    char pad_0168[24]; //0x0168
}; //Size: 0x0180

class RopewayPressController : public REBehavior
{
public:
    char pad_0048[88]; //0x0048
}; //Size: 0x00A0

class RopewayGroundFixer : public REBehavior
{
public:
    char pad_0048[280]; //0x0048
}; //Size: 0x0160

class RopewayPlRainEffect : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    uint8_t N00001B98; //0x0050
    bool isInRain; //0x0051
    char pad_0052[2]; //0x0052
    float N00001BC7; //0x0054
    float N00001B99; //0x0058
    float N00001BCA; //0x005C
    float wetAmount; //0x0060
    float N00001BC3; //0x0064
    char pad_0068[48]; //0x0068
    uint32_t wetState; //0x0098
    char pad_009C[4]; //0x009C
}; //Size: 0x00A0

class REDynamicsComponentBase : public REComponent
{
public:
    class REDynamicsWorld *world; //0x0030
    char pad_0038[8]; //0x0038
}; //Size: 0x0040

class REDynamicsRigidBodySet : public REDynamicsComponentBase
{
public:
    char pad_0040[144]; //0x0040
}; //Size: 0x00D0

class REDynamicsRagdoll : public REDynamicsRigidBodySet
{
public:
    char pad_00D0[32]; //0x00D0
    class REString name; //0x00F0
    char pad_0110[200]; //0x0110
}; //Size: 0x01D8

class REDynamicsWorld : public REManagedObject
{
public:
    char pad_0010[152]; //0x0010
    class REScene *scene; //0x00A8
    char pad_00B0[4448]; //0x00B0
    class REDynamicsWorld *world; //0x1210
    char pad_1218[504]; //0x1218
}; //Size: 0x1410

class RERenderMesh : public REComponent
{
public:
    char pad_0030[736]; //0x0030
}; //Size: 0x0310

class RopewayExtraJoint : public REManagedObject
{
public:
    char pad_0010[8]; //0x0010
    class REMotion *motion; //0x0018
    char pad_0020[8]; //0x0020
    class REManagedObject *param; //0x0028
    int32_t N00002152; //0x0030
    char pad_0034[4]; //0x0034
}; //Size: 0x0038

class RopewayImplementVirtualJoint : public RopewayExtraJoint
{
public:

}; //Size: 0x0038

class RopewayMotionEventHandler : public REBehavior
{
public:
    char pad_0048[16]; //0x0048
    class REMotion *motion; //0x0058
    class REManagedObject *N00002175; //0x0060
    class REPtrArray *layers; //0x0068
}; //Size: 0x0070

class N000021BE : public REBehavior
{
public:

}; //Size: 0x0048

class WwiseContainer : public N000021BE
{
public:
    char pad_0048[456]; //0x0048
}; //Size: 0x0210

class RopewayWwiseContainerApp : public WwiseContainer
{
public:
    char pad_0210[32]; //0x0210
    class REPtrArray *autoTriggerReceivers; //0x0230
}; //Size: 0x0238

class RopewayImplementGun : public RopewayImplementArm
{
public:
    char pad_0148[184]; //0x0148
}; //Size: 0x0200

class RopewaySurvivorDefineDamageParam : public REManagedObject
{
public:
    int32_t damageType; //0x0010
    bool isDead; //0x0014
    char pad_0015[3]; //0x0015
}; //Size: 0x0018

class RopewaySurvivorActionOrderer : public REBehavior
{
public:
    char pad_0048[12]; //0x0048
    uint32_t petient; //0x0054
    char pad_0058[8]; //0x0058
    Vector4f charDir; //0x0060
    Vector4f moveDir; //0x0070
    Vector4f watchDir; //0x0080
    char pad_0090[80]; //0x0090
}; //Size: 0x00E0

class RopewaySurvivorPlayerActionOrderer : public RopewaySurvivorActionOrderer
{
public:
    class RopewayTimer *timer1; //0x00E0
    class RopewayTimer *timer2; //0x00E8
    class RopewayTimer *timer3; //0x00F0
    char pad_00F8[8]; //0x00F8
    class RopewaySurvivorPlayerCondition *parentCondition; //0x0100
}; //Size: 0x0108

class RopewaySurvivorController : public REBehavior
{
public:
    char pad_0048[16]; //0x0048
    class DampingRangeFloat *charAngle; //0x0058
    class DampingRangeFloat *moveAngle; //0x0060
    class DampingRangeFloat *watchAngle; //0x0068
    class DampingRangeFloat *watchAngleX; //0x0070
    class DampingRangeFloat *wheelBlendRate; //0x0078
    class DampingRangeFloat *wheelSpeed; //0x0080
    class REAnimationCurve *wheelBlendRateCurve; //0x0088
    class REAnimationCurve *wheelSpeedCurve; //0x0090
    bool autoUpdateCharDir; //0x0098
    bool autoUpdateMoveDir; //0x0099
    bool autoUpdateWatchDir; //0x009A
    bool autoUpdateWheel; //0x009B
    char pad_009C[4]; //0x009C
    class REAnimationCurve *stairToSlopeCurve; //0x00A0
    class DampingFloat *dampingStairValue; //0x00A8
    class RopewaySurvivorControllerStepCheck *stepChecker; //0x00B0
    char pad_00B8[8]; //0x00B8
    class RopewaySurvivorPlayerCondition *parentCondition; //0x00C0
}; //Size: 0x00C8

class RopewaySurvivorPlayerController : public RopewaySurvivorController
{
public:
    float N00002361; //0x00C8
    float N00002390; //0x00CC
    float charDirDampingRate; //0x00D0
    float N00002393; //0x00D4
    float N00002363; //0x00D8
    float N00002396; //0x00DC
    float N00002364; //0x00E0
    float N00002399; //0x00E4
    class RopewaySurvivorPlayerCondition *parentCondition_; //0x00E8
}; //Size: 0x00F0

class RopewaySurvivorControllerStepCheck : public REManagedObject
{
public:
    Vector4f footLPos; //0x0010
    Vector4f footRPos; //0x0020
    float footDistance; //0x0030
    float weight; //0x0034
    class REJoint *leftJoint; //0x0038
    class REJoint *rightJoint; //0x0040
    class SystemString *leftName; //0x0048
    class SystemString *rightName; //0x0050
    char pad_0058[8]; //0x0058
}; //Size: 0x0060

class DampingRangeFloat : public DampingFloat
{
public:
    float low; //0x0044
    float high; //0x0048
    bool isLoop; //0x004C
    char pad_004D[3]; //0x004D
    float divertPriority; //0x0050
}; //Size: 0x0054

class N000024E5
{
public:
    float value; //0x0000
    int16_t N000024EA; //0x0004
    int16_t N000024F1; //0x0006
    char pad_0008[8]; //0x0008
}; //Size: 0x0010

class CurveKeyFrames
{
public:
    class N000024E5 data[2048]; //0x0000
}; //Size: 0x8000

class N000024E0
{
public:
    Vector4f N000024E1; //0x0000
}; //Size: 0x0010

class RopewayInputSystem : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    class RopewayInputSystemButtonMaskBit *maskbit1; //0x0050
    class RopewayInputSystemButtonMaskBit *maskbit2; //0x0058
    class DotNetGenericList *commandList; //0x0060
    float N00001E29; //0x0068
    float N00003AE0; //0x006C
    float N00001E2A; //0x0070
    char pad_0074[4]; //0x0074
    class DotNetGenericList *defineList; //0x0078
    class DotNetGenericList *lightBarColorParams; //0x0080
    class DotNetGenericList *keyAssignUserData; //0x0088
    char pad_0090[40]; //0x0090
    class HIDJoypadDevice *joypadDevice; //0x00B8
    class RopewayInputSystemAnalogStick *analogStick; //0x00C0
    class InputSystemButton *button; //0x00C8
    char pad_00D0[32]; //0x00D0
    Vector2f cameraSensitivity; //0x00F0
    char pad_00F8[64]; //0x00F8
    int16_t mouseMoveRingBufferSize; //0x0138
    char pad_013A[2]; //0x013A
    float mouseDelta; //0x013C
    float mouseMoveThreshold; //0x0140
    char pad_0144[44]; //0x0144
}; //Size: 0x0170

class RopewayInputSystemButtonMaskBit : public REManagedObject
{
public:
    int64_t bits; //0x0010
    bool reset; //0x0018
    char pad_0019[7]; //0x0019
}; //Size: 0x0020

class InputSystemButton : public REManagedObject
{
public:
    char pad_0010[24]; //0x0010
}; //Size: 0x0028

class HIDNativeDeviceBase : public REManagedObject
{
public:
    char pad_0010[272]; //0x0010
}; //Size: 0x0120

class HIDGamePadDevice : public HIDNativeDeviceBase
{
public:
    char pad_0120[384]; //0x0120
}; //Size: 0x02A0

class HIDJoypadDevice : public HIDGamePadDevice
{
public:
    char pad_02A0[16]; //0x02A0
}; //Size: 0x02B0

class RopewayInputSystemAnalogStick : public REManagedObject
{
public:
    Vector2f axis; //0x0010
    char pad_0018[8]; //0x0018
    Vector2f axisOld; //0x0020
    char pad_0028[8]; //0x0028
    Vector2f rawAxis; //0x0030
    char pad_0038[24]; //0x0038
    float angle; //0x0050
    float angleOld; //0x0054
    float angularVelocity; //0x0058
    float magnitude; //0x005C
    int32_t power; //0x0060
    int32_t powerH; //0x0064
    int32_t powerV; //0x0068
    float N00001F51; //0x006C
    int32_t N00001F58; //0x0070
    char pad_0074[12]; //0x0074
}; //Size: 0x0080

class RopewaySurvivorManager : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    class RopewaySurvivorPlayerCondition *playerCondition; //0x0050
    char pad_0058[8]; //0x0058
    class RopewaySurvivorCastingRequest *castingRequest; //0x0060
    class DotNetGenericList *instantiateRequests; //0x0068
    class DotNetGenericList *survivorPrefabInfo; //0x0070
    class DotNetGenericDictionary *survivorTypesToConditions; //0x0078 SurvivorType->ActorCondition
    class SystemAction *actionPlayerCondition; //0x0080
    class SystemAction *actionNpcCondition; //0x0088
    char pad_0090[8]; //0x0090
    class SystemAction *survivorTypeAction1; //0x0098
    class SystemAction *survivorTypeAction2; //0x00A0
    char pad_00A8[8]; //0x00A8
    class RopewaySurvivorCastingRequest *castingRequest2; //0x00B0
    void *uservarAccessor_Survivor; //0x00B8
    char pad_00C0[8]; //0x00C0
    class RopewaySettingFolder *playerFolder; //0x00C8
    class RopewaySettingFolder *npcFolder; //0x00D0
    class RopewaySettingFolder *actorFolder; //0x00D8
    class UserData *conditionSettingUserData; //0x00E0
    class DotNetGenericDictionary *survivorTypeToExistSurvivorInfo; //0x00E8
    class DotNetGenericList *existSurvivorInfos; //0x00F0
    class REManagedObject *N000058DF; //0x00F8
}; //Size: 0x0100

class RopewaySurvivorCastingRequest : public REManagedObject
{
public:
    char pad_0010[24]; //0x0010
}; //Size: 0x0028

class SystemDelegate : public REManagedObject
{
public:

}; //Size: 0x0010

class DelegateElement
{
public:
    class REManagedObject *obj; //0x0000
    void* funcPtr; //0x0008
}; //Size: 0x0010

class SystemMulticastDelegate : public SystemDelegate
{
public:
    int32_t numElements; //0x0010
    int32_t N00005A07; //0x0014
    class DelegateElement delegates[2048]; //0x0018
}; //Size: 0x8018

class SystemAction : public SystemMulticastDelegate
{
public:

}; //Size: 0x8018

class RopewaySettingFolder : public REManagedObject
{
public:
    class REFolder *folder; //0x0010
    class SystemString *name; //0x0018
}; //Size: 0x0020

class SurvivorConditionTimerTrigger : public REManagedObject
{
public:
    float old; //0x0010
    float current; //0x0014
    float threshold; //0x0018
}; //Size: 0x001C

class RERangeI
{
public:
    int32_t min; //0x0000
    int32_t max; //0x0004
}; //Size: 0x0008

class RopewayEnemyController : public REBehavior
{
public:
    char pad_0048[44]; //0x0048
    float baseMotionSpeed; //0x0074
    int32_t lod; //0x0078
    class RERangeI renderLodRange; //0x007C
    char pad_0084[12]; //0x0084
    class DotNetGenericList *temporaryEventStorage; //0x0090
    class RopewayEnemyEmSaveData *enemySaveData; //0x0098
    char pad_00A0[64]; //0x00A0
    float N00001E0B; //0x00E0
    float N00001E80; //0x00E4
    char pad_00E8[8]; //0x00E8
    Vector4f firstAvailablePos; //0x00F0
    char pad_0100[40]; //0x0100
    class SystemAction *thinkAction; //0x0128
    char pad_0130[16]; //0x0130
    class RopewayEnemyContextController *enemyContextController; //0x0140
    class RopewayEnemyEmCommonContext *enemyContext; //0x0148
    class RopewayBitFlag *conditionStateBitFlag; //0x0150
    class RopewayBitFlag *conditionStateBitFlag2; //0x0158
    char pad_0160[4]; //0x0160
    int32_t N0000201A; //0x0164
    class DotNetGenericList *meshes; //0x0168
    char pad_0170[8]; //0x0170
    class REManagedObject *N00001EB1; //0x0178
    class CollisionSystemAsyncCastRayHandle *castRayHandle; //0x0180
    char pad_0188[4]; //0x0188
    float attackAuthorityKeepRange; //0x018C
    Vector4f attackAuthorityRangeBasePositionOffset; //0x0190
    class RERenderMesh *mesh; //0x01A0
    class REMotion *motion; //0x01A8
    class REActorMotion *actorMotion; //0x01B0
    char pad_01B8[304]; //0x01B8
}; //Size: 0x02E8

class RopewayEnemyContextController : public REBehavior
{
public:
    char pad_0048[12]; //0x0048
    int32_t initialKindId; //0x0054
    char pad_0058[16]; //0x0058
}; //Size: 0x0068

class RopewayEnemyEmCommonContext : public REBehavior
{
public:
    char pad_0048[32]; //0x0048
    class REGameObject *enemyGameObject; //0x0068
    bool spawnStandby; //0x0070
    bool requestedPrefabStandby; //0x0071
    bool requestedSpawn; //0x0072
    bool isStayStandbyArea; //0x0073
    bool isStayActiveArea; //0x0074
    bool isEliminated; //0x0075
    bool N00002016; //0x0076
    bool N00002014; //0x0077
    int32_t instanceStatus; //0x0078 app.ropeway.enemy.EmCommonContext.InstanceStatus
    char pad_007C[44]; //0x007C
}; //Size: 0x00A8

class SystemGuid
{
public:
    char pad_0000[16]; //0x0000
}; //Size: 0x0010

class RopewayEnemyEmSaveData : public REManagedObject
{
public:
    uint32_t dataVersion; //0x0010
    char pad_0014[4]; //0x0014
    class SystemGuid guid; //0x0018
    int16_t inheritId; //0x0028
    char pad_002A[2]; //0x002A
    int32_t kindId; //0x002C
    float birthedTime; //0x0030
    float unloadedTime; //0x0034
    char pad_0038[72]; //0x0038
}; //Size: 0x0080

class RopewayBitFlag : public REManagedObject
{
public:
    int32_t flag; //0x0010
    char pad_0014[4]; //0x0014
}; //Size: 0x0018

class CollisionSystemAsyncCastHandleBase : public REManagedObject
{
public:
    bool isFinish; //0x0010
    char pad_0011[7]; //0x0011
    class REGameObject *owner; //0x0018
    int32_t castType; //0x0020
    int16_t id; //0x0024
    char pad_0026[10]; //0x0026
    Vector4f startPos; //0x0030
    Vector4f endPos; //0x0040
    class PhysicsFilterInfo *filterInfo; //0x0050
    uint32_t N00002199; //0x0058
    float period; //0x005C
    int16_t priority; //0x0060
    char pad_0062[2]; //0x0062
    bool isSkip; //0x0064
    char pad_0065[3]; //0x0065
    class AsyncCastHandleResultData *lastResultData; //0x0068
    char pad_0070[16]; //0x0070
}; //Size: 0x0080

class CollisionSystemAsyncCastRayHandle : public CollisionSystemAsyncCastHandleBase
{
public:

}; //Size: 0x0080

class RopewayEnemyManager : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    class DotNetGenericList *enemyInfos; //0x0050 app.ropeway.EnemyManager.RegisterEnemyInfo
    class DotNetGenericList *enemyControllers; //0x0058
    class DotNetGenericList *gameObjects; //0x0060
    bool isExistEnemyObject; //0x0068
    char pad_0069[3]; //0x0069
    float dataLoadAttackThroughSec; //0x006C
    bool isPausing; //0x0070
    bool isInvisible; //0x0071
    bool isTimelinePausing; //0x0072
    char pad_0073[5]; //0x0073
    class RopewayEnemyController *grapplingOwner; //0x0078
    float N00001F82; //0x0080
    char pad_0084[4]; //0x0084
    class RopewayEnemyLODSettingUserData *lodSettings; //0x0088
    float totalEnemyCost; //0x0090
    char pad_0094[4]; //0x0094
    class SystemAction *N00001F25; //0x0098
    class SystemAction *N00001F53; //0x00A0
    bool hasActiveEnemyUpdateRequest; //0x00A8
    char pad_00A9[7]; //0x00A9
    class REManagedObject *N00001F55; //0x00B0
    class RopewayCameraStatus *cameraStatus; //0x00B8
    class REManagedObject *N00001F57; //0x00C0
    class DotNetGenericList *instantiateRequests; //0x00C8
    class DotNetGenericList *destroyRequests; //0x00D0
    class DotNetGenericDictionary *mapIdsToSceneLoadStatus; //0x00D8
    char pad_00E0[8]; //0x00E0
    class DotNetGenericList *sceneLoadStatuses; //0x00E8
    class DotNetGenericList *mapIds; //0x00F0
    class REPtrArray *gameObjectsToControllers; //0x00F8 list of dictionaries
    class DotNetGenericList *em6200ChaserControllers; //0x0100
    class REManagedObject *N00002224; //0x0108
    char pad_0110[8]; //0x0110
    class RopewayEnemyManagerFrameTimer *frameTimers[6]; //0x0118 ThinkOffTimer, NoAttackTimer, AttackThroughTimer, NoHoldTimerEm4000, NoBackHoldTimerEm4000, NoAttackTimerEm4000
    class DotNetGenericDictionary *guidsToSaveData; //0x0148
    uint32_t totalEnemyKillCount; //0x0150
    char pad_0154[4]; //0x0154
    class REManagedObject *N0000222A; //0x0158
    class REManagedObject *N0000222B; //0x0160
    class REManagedObject *N0000222C; //0x0168
    class REManagedObject *N0000222D; //0x0170
    char pad_0178[8]; //0x0178
}; //Size: 0x0180

class RopewayEnemyLODSettingUserData : public UserData
{
public:
    float distanceLevels[5]; //0x0030
}; //Size: 0x0044

class RopewayEnemyManagerFrameTimer : public REManagedObject
{
public:
    bool run; //0x0010
    char pad_0011[7]; //0x0011
    uint64_t delayFrame; //0x0018
    uint64_t endFrame; //0x0020
    uint64_t frameCount; //0x0028
}; //Size: 0x0030

class RENativeArray // via.array
{
public:
    class RENativeArrayData *data; //0x0000
    int32_t size; //0x0008
    int32_t numAllocated; //0x000C
}; //Size: 0x0010

class RENativeArrayData
{
public:
    char pad_0000[8]; //0x0000
}; //Size: 0x0008

class RopewayCameraStatus : public REManagedObject
{
public:
    char pad_0010[384]; //0x0010
}; //Size: 0x0190

class RopewayCameraInterpolateSettings : public UserData
{
public:
    void *curveParamTable; //0x0030
}; //Size: 0x0038

class CollisionSystemAsyncCastSphereHandle : public CollisionSystemAsyncCastHandleBase
{
public:
    class RESphere *sphere; //0x0080
}; //Size: 0x0088

class RESphere
{
public:
    char pad_0000[8]; //0x0000
}; //Size: 0x0008

class RERenderCaptureToTexture : public REComponent
{
public:
    bool N00004053; //0x0030
    bool N00004069; //0x0031
    bool enableAlphaChannel; //0x0032
    char pad_0033[45]; //0x0033
    bool enableReplayScene; //0x0060
    char pad_0061[39]; //0x0061
    float N0000405E; //0x0088
    float N00004074; //0x008C
}; //Size: 0x0090

class AsyncCastHandleResultData : public REManagedObject
{
public:
    char pad_0010[32]; //0x0010
    Vector4f startPos; //0x0030
    Vector4f endPos; //0x0040
    char pad_0050[16]; //0x0050
}; //Size: 0x0060

class ValueTriggerInt : public REManagedObject
{
public:
    int32_t current; //0x0010
    int32_t old; //0x0014
    int32_t lastFrame; //0x0018
}; //Size: 0x001C

class ActionTriggerInt : public ValueTriggerInt
{
public:
    char pad_001C[4]; //0x001C
    class SystemAction *action; //0x0020
}; //Size: 0x0028

class IkControlStatusArray : public REArrayBase
{
public:
    class RopewayIkControlStatus *N00004982[6]; //0x0020
}; //Size: 0x0050

class RopewayIkControlStatus : public REManagedObject
{
public:
    int32_t ikKind; //0x0010
    float currentBlendRate; //0x0014
    bool hasChild; //0x0018
    bool activeAnyChild; //0x0019
    char pad_001A[14]; //0x001A
    bool hasExtraBlendRate; //0x0028
    char pad_0029[3]; //0x0029
    float currentExtraBlendRate; //0x002C
    float extraBlendLerpTime; //0x0030
    int32_t N00004A6E; //0x0034
    class RopewayIkWrapper *ik; //0x0038
    int32_t N00004ACF; //0x0040
    char pad_0044[4]; //0x0044
    class DotNetGenericDictionary *N00004A98; //0x0048
    class RopewayRangeLerpFloat *rangeLerp1; //0x0050
    class RopewayRangeLerpFloat *rangeLerp2; //0x0058
    class RopewayRangeLerpFloat *rangeLerp3; //0x0060
    void *N00004A9A; //0x0068
    void *N00004AD5; //0x0070
    int32_t N00004A45; //0x0078
    float N00004DBD; //0x007C
}; //Size: 0x0080

class RopewayIkWrapper : public REManagedObject
{
public:
    bool setupped; //0x0010
    char pad_0011[7]; //0x0011
    class REMotionIkLeg *N00004B67; //0x0018
    char pad_0020[88]; //0x0020
}; //Size: 0x0078

class REMotionIkLeg : public RESecondaryAnimation
{
public:
    char pad_0048[1904]; //0x0048
    float heelHeight; //0x07B8
    float toeHeight; //0x07BC
    float groundContactUpDistance; //0x07C0
    float groundContactDownDistance; //0x07C4
    float groundContactRotationAdjustDistance; //0x07C8
    char pad_07CC[4]; //0x07CC
    int32_t effectorTarget; //0x07D0
    int32_t centerAdjust; //0x07D4
    char pad_07D8[32]; //0x07D8
    int32_t toeControlOption; //0x07F8
    int32_t rayCastSkipOption; //0x07FC
    float blendRate; //0x0800
    float adjustFootRotRate; //0x0804
    float adjustFootDamping; //0x0808
    float N00004EF8; //0x080C
    float adjustFootBottomRate; //0x0810
    float adjustFootUpwardRate; //0x0814
    float N00004CB3; //0x0818
    float centerAdjustRate; //0x081C
    float centerDampingRate; //0x0820
    float N00004EFE; //0x0824
    float centerRXAdjustRate; //0x0828
    float centerRZAdjustRate; //0x082C
    float distanceLimitRate; //0x0830
    float centerAdjustUpwardRate; //0x0834
    float wallLimitAngle; //0x0838
    float N00004EF0; //0x083C
    float footRollLimit; //0x0840
    float N00004EF2; //0x0844
    int32_t jointNumBetweenHeelAndToe; //0x0848
    int32_t rayCastInterval; //0x084C
    char pad_0850[210]; //0x0850
    bool enabledGroundAdjust; //0x0922
    char pad_0923[2]; //0x0923
    bool skipIkIfNoHitDetail; //0x0925
    bool useIkSpine; //0x0926
    char pad_0927[361]; //0x0927
}; //Size: 0x0A90

class RopewayRangeLerpFloat : public REManagedObject
{
public:
    float lerpSpeed; //0x0010
    float min; //0x0014
    float max; //0x0018
    bool isLoop; //0x001C
    char pad_001D[3]; //0x001D
    float loopPriority; //0x0020
    bool isInterpolating; //0x0024
    char pad_0025[3]; //0x0025
    float target; //0x0028
    float current; //0x002C
}; //Size: 0x0030

class RopewayEquipmentManager : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    class DotNetGenericList *prefabInfoList; //0x0050
    class DotNetGenericList *characterMotionPrefabInfoList; //0x0058
    class DotNetGenericList *equipments; //0x0060
    class DotNetGenericList *instantiateRequestList; //0x0068
    class RopewayWeaponEquippedPositionUserData *weaponEquippedPositionData; //0x0070
    class RopewayWeaponBulletUserData *weaponBulletData; //0x0078
    char pad_0080[16]; //0x0080
}; //Size: 0x0090

class RopewayWeaponBulletUserData : public UserData
{
public:
    char pad_0030[8]; //0x0030
    bool enableDebug; //0x0038
    char pad_0039[3]; //0x0039
    int32_t debugWeaponType; //0x003C
    int32_t debugWeaponParts; //0x0040
    char pad_0044[4]; //0x0044
}; //Size: 0x0048

class RopewayWeaponEquippedPositionUserData : public UserData
{
public:
    char pad_0030[8]; //0x0030
}; //Size: 0x0038

class RopewayGameMaster : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    class UserData *difficultySettings; //0x0050
    class UserData *continueRankPoint; //0x0058
    bool isWindows10; //0x0060
    char pad_0061[3]; //0x0061
    int32_t machine; //0x0064
    int32_t machineDetail; //0x0068
    int32_t operatingSystem; //0x006C
    int32_t servicePlatform; //0x0070
    int32_t criticalRateController; //0x0074
}; //Size: 0x0078

class ManagedEnumValuesPtr
{
public:
    char *(*names)[8192]; //0x0000
}; //Size: 0x0008

class REEnumMap
{
public:
    class REEnumNode *first; //0x0000
    int32_t num; //0x0008
    char pad_000C[4]; //0x000C
}; //Size: 0x0010

class ManagedEnums : public REEnumMap
{
public:
    class RENativeArray enumValues; //0x0010
}; //Size: 0x0020

class N00006DF9
{
public:
    char pad_0000[16]; //0x0000
    class RENativeArray enumValues; //0x0010
    char pad_0020[304]; //0x0020
}; //Size: 0x0150

class N00006EE3
{
public:
    class REEnumDescriptor *N00006EE4; //0x0000
    class N00006F33 *N00006EE5; //0x0008
    class N00006F56 *N00006EE6; //0x0010
    char pad_0018[8]; //0x0018
}; //Size: 0x0020

class REEnumNode
{
public:
    class REEnumDescriptor *N00007017; //0x0000
    class REEnumDescriptor *N0000702A; //0x0008
    class REEnumDescriptor *N0000702B; //0x0010
    char pad_0018[8]; //0x0018
}; //Size: 0x0020

class REEnumData
{
public:
    char *name; //0x0000
    bool N000070A2; //0x0008
    char pad_0009[7]; //0x0009
    class REEnumValueNode *values; //0x0010
    void *N000070A4; //0x0018
    char pad_0020[24]; //0x0020
}; //Size: 0x0038

class REEnumPair
{
public:
    uint32_t first; //0x0000
    char pad_0004[4]; //0x0004
    class REEnumData second; //0x0008
}; //Size: 0x0040

class REEnumDescriptor : public REEnumNode
{
public:
    class REEnumPair data; //0x0020
    char pad_0060[332]; //0x0060
}; //Size: 0x01AC

class N00006F33
{
public:
    char pad_0000[8]; //0x0000
}; //Size: 0x0008

class N00006F56
{
public:
    char pad_0000[8]; //0x0000
}; //Size: 0x0008

class REEnumValueNode
{
public:
    char *name; //0x0000
    int64_t value; //0x0008
    class REEnumValueNode *next; //0x0010
}; //Size: 0x0018

class StdMap
{
public:
    class StdMapNode *_MyHead; //0x0000
    int32_t _MySize; //0x0008
    char pad_000C[4]; //0x000C
}; //Size: 0x0010

class StdMapData
{
public:
    char pad_0000[120]; //0x0000
}; //Size: 0x0078

class StdMapNode
{
public:
    class StdMapNode *N00008173; //0x0000
    class StdMapNode *N00008174; //0x0008
    class StdMapNode *N00008175; //0x0010
    char pad_0018[8]; //0x0018
    class StdMapData data; //0x0020
}; //Size: 0x0098

class TypeList // RENativeArray
{
public:
    class REType *(*data)[50000]; //0x0000
    int32_t size; //0x0008
    int32_t numAllocated; //0x000C
    char pad_0010[120]; //0x0010
}; //Size: 0x0088

class MethodParamInfo
{
public:
    char pad_0000[8]; //0x0000
    char *paramName; //0x0008
    uint32_t paramTypeFlag; //0x0010 AND 1f
    uint32_t typeIndex; //0x0014
    char *typeName; //0x0018
}; //Size: 0x0020

class AppCameraManager : public REBehavior
{
public:
    char pad_0048[16]; //0x0048
    class REGameObject *cameraObject; //0x0058
    class RECamera *camera; //0x0060
    class RETransform *cameraTransform; //0x0068
    Vector4f cameraDirection; //0x0070
    Vector4f cameraPosition; //0x0080
    class AppCameraParameterBlender *cameraParameterBlender; //0x0090
    class AppPlayerCameraController *cameraController; //0x0098
    char pad_00A0[48]; //0x00A0
}; //Size: 0x00D0

class AppCameraParameterBlender : public REBehavior
{
public:
    char pad_0048[32]; //0x0048
    bool manualMode; //0x0068
    char pad_0069[3]; //0x0069
    float manualBlendRate; //0x006C
    float blendLength; //0x0070
    int32_t target; //0x0074
    int32_t current; //0x0078
    char pad_007C[4]; //0x007C
    class DotNetGenericList *cameraParameterList; //0x0080
    class REAnimationCurve *blendCurve; //0x0088
    char pad_0090[24]; //0x0090
    float lerpTimer; //0x00A8
    float maxTime; //0x00AC
    bool isLerping; //0x00B0
    bool isHitTerrain; //0x00B1
    bool isCollisionCheck; //0x00B2
    char pad_00B3[21]; //0x00B3
}; //Size: 0x00C8

class AppPlayerCameraController : public REBehavior
{
public:
    char pad_0048[24]; //0x0048
    Vector4f DefaultCameraOffsetLow; //0x0060
    Vector4f PlayerOffset; //0x0070
    float OffsetZSlider; //0x0080
    char pad_0084[4]; //0x0084
    void *MissionStartCamera; //0x0088
    class DotNetGenericList *cameraParameters; //0x0090
    uint32_t DefaultCameraIndex; //0x0098
    char pad_009C[4]; //0x009C
    void *NightmareCamera; //0x00A0
    float AutoRotAccWallRateH; //0x00A8
    float StickRotAccH; //0x00AC
    float StickRotMaxSpdH; //0x00B0
    char pad_00B4[4]; //0x00B4
    void *StickRotSpeedCurveH; //0x00B8
    float StickRotAccV; //0x00C0
    float StickRotMaxV; //0x00C4
    float SlopeRotAccV; //0x00C8
    float AutoRotRedV; //0x00CC
    float RotLimitRedV; //0x00D0
    float WallOffsetY; //0x00D4
    float LockOnRange; //0x00D8
    float BumperRadius; //0x00DC
    float CollisionOffsetTimer; //0x00E0
    float CollisionOffsetDelay; //0x00E4
    float RecommendedCameraTimerDelay; //0x00E8
    bool InverseRotH; //0x00EC
    bool InverseRotV; //0x00ED
    bool InverseMouseRotH; //0x00EE
    bool InverseMouseRotV; //0x00EF
    float SlopeAdjustAngle; //0x00F0
    float OffsetYTime; //0x00F4
    float LockOnOffsetZRateSpeed; //0x00F8
    float LockOnOffsetZRateReduce; //0x00FC
    float LockOnOffsetY; //0x0100
    char pad_0104[4]; //0x0104
    void *CameraData; //0x0108
    void *FinishCameraCurve; //0x0110
    void *FinishCameraCurveFOV; //0x0118
    void *FinishCameraCurveWR; //0x0120
    class AppCameraParameter *DefaultParameter; //0x0128
    void *AutoRotationParameterDefault; //0x0130
    float WallHitRotationMax; //0x0138
    float WallHitRotationRate; //0x013C
    float NormalCameraTranslateSpeedToWait; //0x0140
    float NormalCameraTranslateSpeedToMove; //0x0144
    float BattleCamTranslateSpeedFrontToBack; //0x0148
    float BattleCamTranslateSpeedBackToFront; //0x014C
    Vector4f MovePOGOffsetFront; //0x0150
    Vector4f MovePOGOffsetLeft; //0x0160
    Vector4f MovePOGOffsetRight; //0x0170
    Vector4f MovePOGOffsetBack; //0x0180
    float MoveOffsetSpeed; //0x0190
    float CameraRollSpeed; //0x0194
    float CameraRollSpeedRed; //0x0198
    float CameraRollSpeedMax; //0x019C
    float LockOnWallMoveMaxLength; //0x01A0
    char pad_01A4[20]; //0x01A4
    bool IsForceAdjustCollider; //0x01B8
    bool IsIgnoreBattleManualverticalLimit; //0x01B9
    char pad_01BA[2]; //0x01BA
    float WarpInterpTime; //0x01BC
    float JumpZOffsetOffsetLength; //0x01C0
    float N01c4; //0x01C4
    float N01c5; //0x01C8
    char pad_01CC[4]; //0x01CC
    void *SettingData; //0x01D0
    void *LockOnParam; //0x01D8
    void *LegShakeParam; //0x01E0
    char pad_01E8[4]; //0x01E8
    uint32_t lastHitInfo; //0x01EC
    char pad_01F0[24]; //0x01F0
    bool CameraDebug; //0x0208
    char pad_0209[3]; //0x0209
    float DebugSlopeAngle; //0x020C
    bool TerrainCheck; //0x0210
    bool UseJumpCamera; //0x0211
    char pad_0212[6]; //0x0212
    void *FinishCameraLight; //0x0218
    char pad_0220[96]; //0x0220
    float angleHorizontal; //0x0280
    float forceAngleHorizontal; //0x0284
    float oldAngleHorizontal; //0x0288
    float N028c; //0x028C
    char pad_0290[4]; //0x0290
    float angleVertical; //0x0294
    float cameraRotationSpeedHorizontal; //0x0298
    float cameraRotationSpeedVertical; //0x029C
    float oldAngleVertical; //0x02A0
    float rightHitLevel; //0x02A4
    float leftHitLevel; //0x02A8
    float bumperLength; //0x02AC
    bool topHit; //0x02B0
    char pad_02B1[3]; //0x02B1
    float topHitRate; //0x02B4
    bool bottomHit; //0x02B8
    char pad_02B9[3]; //0x02B9
    float bottomHitRate; //0x02BC
    uint32_t cameraMode; //0x02C0
    char pad_02C4[12]; //0x02C4
    uint32_t finishMoveMode; //0x02D0
    bool isFinishCameraEndEnabled; //0x02D4
    char pad_02D5[27]; //0x02D5
    float forceCameraVerticalRotationSpeed; //0x02F0
    char pad_02F4[44]; //0x02F4
    class AppCameraParameter *parameter; //0x0320
    void *shakeCamera; //0x0328
    float massAmplifier; //0x0330
    char pad_0334[12]; //0x0334
    Vector4f forwardHorizontal; //0x0340
    Vector4f forward; //0x0350
    Vector4f controllerRotation; //0x0360
    char pad_0370[40]; //0x0370
    void *StageCameraShake; //0x0398
    void *CameraShake; //0x03A0
    void *EnemyCameraShake; //0x03A8
    char pad_03B0[16]; //0x03B0
    float finishCameraTime; //0x03C0
    char pad_03C4[12]; //0x03C4
    Vector4f cameraShakeHypocenter; //0x03D0
    char pad_03E0[24]; //0x03E0
    void *defaultCameraParameter; //0x03F8
    class AppCameraSingleTargetCamera *bossCamera; //0x0400
    void *bossCameraData; //0x0408
    void *bossCameraParameter; //0x0410
    void *bossCameraInterpolator; //0x0418
    bool isBossBattle; //0x0420
    bool isBossCameraSphericaInterp; //0x0421
    char pad_0422[2]; //0x0422
    float bossCameraInterpolationTimer; //0x0424
    float bossCameraInterpolationTime; //0x0428
    char pad_042C[4]; //0x042C
    void *PerformanceCamera; //0x0430
    void *motionCamera; //0x0438
    void *busterCamera; //0x0440
    void *fixedCamera; //0x0448
    char pad_0450[8]; //0x0450
    void *railCamera; //0x0458
    char pad_0460[4]; //0x0460
    float _LockonWallMaxTime; //0x0464
    char pad_0468[20]; //0x0468
    bool isOptionZOffsetCameraIgnore; //0x047C
    bool isLockOnCameraIgnore; //0x047D
    bool isBattleCameraIgnore; //0x047E
    bool isBehindLockOn; //0x047F
    bool isBehindOldLockOn; //0x0480
    bool owBehindLockOn; //0x0481
    char pad_0482[2]; //0x0482
    float owBehindLockOnHeight; //0x0484
    void *owBehindLockOnParam; //0x0488
    float behindLockOnTimer; //0x0490
    char pad_0494[92]; //0x0494
    float defaultCameraInterpolationTimerMax; //0x04F0
    float defaultCameraInterpolationTimer; //0x04F4
    bool isFrontCollisionHit; //0x04F8
    char pad_04F9[7]; //0x04F9
    class RECamera *camera; //0x0500
    void *cameraBlender; //0x0508
    class RETransform *cammeraTransform; //0x0510
    char pad_0518[8]; //0x0518
    class REMotionCamera *motionCamera_; //0x0520
    char pad_0528[8]; //0x0528
    Vector4f lookAtPos; //0x0530
    float oldPogMoveRateX; //0x0540
    float oldPogMoveRateZ; //0x0544
    class AppCollisionAsyncCastRayResult *castResults[16]; //0x0548
    char pad_05C8[40]; //0x05C8
    void *Smoothing; //0x05F0
    void *SmoothingY; //0x05F8
    void *SmoothingZ; //0x0600
    void *SlopeSmoothing; //0x0608
    void *BattleCameraSmoothing; //0x0610
    void *OffsetSmoothing; //0x0618
    void *WallPushOffsetSmoothing; //0x0620
    void *ServantPOGYSmoothing; //0x0628
    void *BehindHorizontalSmoothing; //0x0630
    void *BehindVerticalSmoothing; //0x0638
    void *AutoBehindHorizontalSmoothing; //0x0640
    void *ResetCameraSmoothing; //0x0648
    void *POGBasePositionSmoothing; //0x0650
    char pad_0658[24]; //0x0658
    void *aiMapController; //0x0670
    char pad_0678[280]; //0x0678
    Vector4f controllerPos1; //0x0790
    Vector4f controllerPos2; //0x07A0
    char pad_07B0[4]; //0x07B0
    float normalCameraBlendRate; //0x07B4
    float battleCameraBlendRate; //0x07B8
    float lockOnCameraBlendRate; //0x07BC
    float lockOnTargetAngle; //0x07C0
    float aimCameraBlendRate; //0x07C4
    void *ZOffsetController; //0x07C8
    void *fovController; //0x07D0
    char pad_07D8[96]; //0x07D8
    float finishMotionCameraTimer; //0x0838
    char pad_083C[4]; //0x083C
    uint32_t finishMotionCameraType; //0x0840
    char *finishMotionCameraType_1; //0x0844
    char pad_084C[12]; //0x084C
    float finishBusterCameraTimer; //0x0858
    char pad_085C[4]; //0x085C
    uint32_t BusterCameraInterpType; //0x0860
    char pad_0864[84]; //0x0864
    float originalLerpTime; //0x08B8
    float originalLerpTimer; //0x08BC
    bool reserveFinishCameraEnd; //0x08C0
    char pad_08C1[3]; //0x08C1
    float finishCameraSkipTimer; //0x08C4
    bool isFinishCameraPapetPlayer; //0x08C8
    char pad_08C9[15]; //0x08C9
    void *FinishSetting; //0x08D8
    void *EnemyLibrarySetting; //0x08E0
    void *ItemGetSetting; //0x08E8
    void *BossFinishTest; //0x08F0
    void *SpecialSetting; //0x08F8
    char pad_0900[8]; //0x0900
    bool isAllowFollowPapetPlayer; //0x0908
    char pad_0909[7]; //0x0909
    Vector3f cachedInputVecForPC; //0x0910
    char pad_091C[8]; //0x091C
    float WallHitSpeedMax; //0x0924
    float WallHitAccMax; //0x0928
    float WallHitSpeedHideMax; //0x092C
    float WallHitAccHideMax; //0x0930
    char pad_0934[4]; //0x0934
    void *HorizontalSmoothing; //0x0938
    char pad_0940[32]; //0x0940
    void *AutoHorizontalRotationSmoothing; //0x0960
    char pad_0968[120]; //0x0968
    void *ForceCameraRotationCurve; //0x09E0
    float MaxForceRotationSpeed; //0x09E8
    char pad_09EC[20]; //0x09EC
    class AppMathExExpDampSmoothing *VerticalSmoothing; //0x0A00
    class AppMathExExpDampSmoothing *FreeFallVerticalSmoothing; //0x0A08
    class AppMathExExpDampSmoothingVector *FinalPositionSmoothing; //0x0A10
    class AppMathExExpDampSmoothingVector *POGSmoothingXZ; //0x0A18
    class AppMathExExpDampSmoothing *POGSmoothingY; //0x0A20
    class AppMathExExpDampSmoothing *VerticalLimitSmoothing; //0x0A28
    class AppMathExExpDampSmoothing *POGOffsetRateSmoothing; //0x0A30
    class REAnimationCurve *PogSmoothingYCurve; //0x0A38
    char pad_0A40[72]; //0x0A40
    float screenRate; //0x0A88
    char pad_0A8C[196]; //0x0A8C
    void *_NightmareAppearParameterList; //0x0B50
    char pad_0B58[8]; //0x0B58
    float AICameraDamping; //0x0B60
    char pad_0B64[4]; //0x0B64
    void *AICameraParam; //0x0B68
    char pad_0B70[296]; //0x0B70
    void *SkillCameraSmoothing; //0x0C98
}; //Size: 0x0CA0

class AppCollisionAsyncCastBase : public REManagedObject
{
public:
    char pad_0010[12]; //0x0010
}; //Size: 0x001C

class AppCollisionAsyncCastRayHitOnly : public AppCollisionAsyncCastBase
{
public:
    char pad_001C[20]; //0x001C
}; //Size: 0x0030

class AppCollisionAsyncCastRayResult : public AppCollisionAsyncCastRayHitOnly
{
public:
    class AppCollisionSystemHitResult *result; //0x0030
}; //Size: 0x0038

class AppCollisionSystemHitResult : public REManagedObject
{
public:
    Vector4f position; //0x0010
    Vector4f normal; //0x0020
    char pad_0030[8]; //0x0030
    uint32_t materialId; //0x0038
    uint32_t materialAttribute; //0x003C
    uint32_t maskBits; //0x0040
    char pad_0044[4]; //0x0044
    class PhysicsCollider *collidable; //0x0048
    char pad_0050[24]; //0x0050
}; //Size: 0x0068

class PhysicsUserData : public REManagedObject
{
public:
    char pad_0010[64]; //0x0010
}; //Size: 0x0050

class AppCameraParameter : public REManagedObject
{
public:
    uint32_t LerpType; //0x0010
    float FOV; //0x0014
    char pad_0018[8]; //0x0018
    Vector4f Offset; //0x0020
    Vector4f PointOfGaze; //0x0030
    uint32_t RecommendedVerticalLimitType; //0x0040
    float RecommendedVerticalLimitHigh; //0x0044
    float RecommendedVerticalLimitLow; //0x0048
    float Roll; //0x004C
    float VerticalLimitRate; //0x0050
    float VerticalLimitHigh; //0x0054
    float VerticalLimitLow; //0x0058
    bool ResetRecommendedCameraTimer; //0x005C
    char pad_005D[3]; //0x005D
    float RotationAccVertical; //0x0060
    float LerpTime; //0x0064
    float PogOffsetYHigh; //0x0068
    float PogOffsetYLow; //0x006C
    class AppCameraLookTarget *Target; //0x0070
}; //Size: 0x0078

class AppCameraLookTarget : public REManagedObject
{
public:
    Vector4f pos; //0x0010
    bool isEnable; //0x0020
    char pad_0021[15]; //0x0021
}; //Size: 0x0030

class AppMathExExpDampSmoothing : public REManagedObject
{
public:
    float damping; //0x0010
    float currentPos; //0x0014
    float velocity; //0x0018
}; //Size: 0x001C

class AppMathExExpDampSmoothingVector : public REManagedObject
{
public:
    float damping; //0x0010
    char pad_0014[12]; //0x0014
    Vector4f lastPos; //0x0020
    Vector4f currentPos; //0x0030
}; //Size: 0x0040

class AppPlayerManager : public REBehavior
{
public:
    char pad_0048[16]; //0x0048
    class AppPlayerManagerNeroDataHolder *neroData; //0x0058
    class AppPlayer *manualPlayer; //0x0060
    char pad_0068[16]; //0x0068
    class DMCPlayerArray *players; //0x0078
}; //Size: 0x0080

class AppPlayerManagerNeroDataHolder : public REManagedObject
{
public:
    class UserData *gauntletCommonParameter; //0x0010
}; //Size: 0x0018

class DMCPlayerArray : public REArrayBase
{
public:
    class AppPlayer *data[100]; //0x0020
}; //Size: 0x0340

class AppNetworkBaseBehavior : public REBehavior
{
public:
    char pad_0048[12]; //0x0048
    uint32_t NetworkType; //0x0054
    char pad_0058[4]; //0x0058
    uint32_t NetworkCheckType; //0x005C
    void *NetworkSubType; //0x0060
    char pad_0068[8]; //0x0068
    void *NetworkAttribute; //0x0070
    char pad_0078[8]; //0x0078
    uint32_t GeneratorType; //0x0080
    char pad_0084[36]; //0x0084
    bool isPuppetSlow; //0x00A8
    char pad_00A9[31]; //0x00A9
    uint32_t networkWwiseType; //0x00C8
    char pad_00CC[4]; //0x00CC
    void *cachedNetworkOwner; //0x00D0
    char pad_00D8[16]; //0x00D8
    int32_t SelfIndex; //0x00E8
    int32_t MasterIndex; //0x00EC
    bool IsMediationPuppet; //0x00F0
    bool IsIntegrity; //0x00F1
    char pad_00F2[2]; //0x00F2
    uint32_t MediationType; //0x00F4
}; //Size: 0x00F8

class AppReplayBaseBehavior : public AppNetworkBaseBehavior
{
public:
    char pad_00F8[16]; //0x00F8
}; //Size: 0x0108

class AppGameModel : public AppReplayBaseBehavior
{
public:
    uint32_t group; //0x0108
    char pad_010C[68]; //0x010C
    void *groundCheckFilter; //0x0150
    char pad_0158[12]; //0x0158
    float moveSpeed; //0x0164
    char pad_0168[88]; //0x0168
    Vector4f innerPlayerCameraRate; //0x01C0
    char pad_01D0[2]; //0x01D0
    bool isMasterLockOnRestricted; //0x01D2
    bool isReplayLockOnRestricted; //0x01D3
    char pad_01D4[140]; //0x01D4
}; //Size: 0x0260

class AppCharacter : public AppGameModel
{
public:
    char pad_0260[8]; //0x0260
    float syncRandomF; //0x0268
    uint32_t syncRandomUI; //0x026C
    Vector4f subAdjustOffset; //0x0270
    float damageTimer; //0x0280
    float damageTimeMax; //0x0284
    float damageReactionTimer; //0x0288
    float gravityUp; //0x028C
    float gravityDown; //0x0290
    float speedY; //0x0294
    float gravityUpInit; //0x0298
    float gravityDownInit; //0x029C
    float slopeOffsetY; //0x02A0
    float snatchTimer; //0x02A4
    uint32_t snatchBankID; //0x02A8
    uint32_t snatchMotionID; //0x02AC
    char pad_02B0[8]; //0x02B0
    float bladeOffset; //0x02B8
    float groundFitLengthLow; //0x02BC
    float groundFitLengthHigh; //0x02C0
    float jumpAttackCount; //0x02C4
    char pad_02C8[232]; //0x02C8
    void *systemStatus; //0x03B0
    void *tempStatus; //0x03B8
    void *tempStatus2; //0x03C0
    char pad_03C8[96]; //0x03C8
    class AppLockOnObject *lockOnTarget; //0x0428
    char pad_0430[88]; //0x0430
    uint32_t actionLevel; //0x0488
    char pad_048C[20]; //0x048C
    void *navigationData; //0x04A0
    char pad_04A8[32]; //0x04A8
    void *generateIDInfo; //0x04C8
    char pad_04D0[1]; //0x04D0
    bool isRespawnDisable; //0x04D1
    char pad_04D2[22]; //0x04D2
    void *slopeDetector; //0x04E8
    char pad_04F0[160]; //0x04F0
    uint32_t wetType; //0x0590
    float safeTimer; //0x0594
    float fallTimer; //0x0598
    float slopeLimitOW; //0x059C
    char pad_05A0[16]; //0x05A0
    bool isHoldEnable; //0x05B0
    char pad_05B1[31]; //0x05B1
    Vector4f holdPosition; //0x05D0
    char pad_05E0[16]; //0x05E0
    bool isEnableDownDying; //0x05F0
    char pad_05F1[31]; //0x05F1
    float ikDamageStrength; //0x0610
    char pad_0614[28]; //0x0614
    void *cachedReactionSpecial; //0x0630
    char pad_0638[4]; //0x0638
    uint32_t deathCauseID; //0x063C
    bool noDamageReaction; //0x0640
    char pad_0641[63]; //0x0641
    bool isMarking; //0x0680
    bool isMarkingDummy; //0x0681
    bool isMarkingPrev; //0x0682
    bool isMarkingDummyPrev; //0x0683
    char pad_0684[28]; //0x0684
    uint32_t autoChainBlendRecovery; //0x06A0
    char pad_06A4[28]; //0x06A4
    Vector4f oldLeftPivotPos; //0x06C0
    Vector4f oldRightPivotPos; //0x06D0
    Vector4f finalPositionBeforeAdjust; //0x06E0
    char pad_06F0[8]; //0x06F0
    bool isFallStop; //0x06F8
    char pad_06F9[59]; //0x06F9
    float slopeLimit; //0x0734
    bool slopeSlide; //0x0738
    char pad_0739[3]; //0x0739
    float wetTimer; //0x073C
    float maxWetTimer; //0x0740
    bool isSetSafeTimer; //0x0744
    char pad_0745[35]; //0x0745
    void *cameraShakeData; //0x0768
    void *ColliderPreset; //0x0770
    char pad_0778[64]; //0x0778
    bool IsLockonRangeInfinity; //0x07B8
    char pad_07B9[7]; //0x07B9
    void *FinishCameraData; //0x07C0
    uint32_t ResetThinkLayer; //0x07C8
    bool IsRequestFinishCamera; //0x07CC
    char pad_07CD[83]; //0x07CD
    bool IsUseSmoothAnimator; //0x0820
    char pad_0821[63]; //0x0821
    bool IsAutoAdjustFootDamping; //0x0860
    bool IsAutoAdjustFootBottomRate; //0x0861
    bool IsAutoAdjustCenterDamping; //0x0862
    char pad_0863[85]; //0x0863
    bool isInterferenceObject; //0x08B8
    char pad_08B9[15]; //0x08B9
    uint32_t SkipEventProcessType; //0x08C8
    char pad_08CC[4]; //0x08CC
    Vector4f SkipEventprocessVec; //0x08D0
    char pad_08E0[16]; //0x08E0
    bool IsDisablePositionErrorCorrector; //0x08F0
    char pad_08F1[103]; //0x08F1
    void *SyncPosition; //0x0958
    void *SyncPosition2; //0x0960
    bool IsWarpSyncPosition; //0x0968
    bool IsMovePosition; //0x0969
    bool isSyncProgressDead; //0x096A
    char pad_096B[13]; //0x096B
    bool IsIntegrityMotion; //0x0978
    char pad_0979[7]; //0x0979
    void *SyncUserVariable; //0x0980
    char pad_0988[8]; //0x0988
}; //Size: 0x0990

class AppPlayerBase : public AppCharacter
{
public:
    uint32_t Shoes; //0x0990
    char pad_0994[12]; //0x0994
    float currentDevilTriggerID; //0x09A0
    uint32_t devilTriggerGaugeAddType; //0x09A4
    char pad_09A8[8]; //0x09A8
    uint32_t currentDevilTrigger; //0x09B0
    char pad_09B4[12]; //0x09B4
}; //Size: 0x09C0

class AppPlayer : public AppPlayerBase
{
public:
    char pad_09C0[8]; //0x09C0
    int32_t DefaultChainPresetID; //0x09C8
    float Reactivity; //0x09CC
    float MotionSpeed; //0x09D0
    uint32_t WallMaterialID; //0x09D4
    float RunBlendRate; //0x09D8
    float Gravity; //0x09DC
    void *CameraShake; //0x09E0
    void *StageCameraShake; //0x09E8
    void *CommonParamter; //0x09F0
    void *GlideSpeedY; //0x09F8
    void *GlideSpeedXZ; //0x0A00
    float GlideTimer; //0x0A08
    float GlideMaxSpeedY; //0x0A0C
    float GlideMaxSpeedXZ; //0x0A10
    float HipOffset; //0x0A14
    float GunCancelFrame; //0x0A18
    float BlendRateTop; //0x0A1C
    float BlendRateDown; //0x0A20
    float GunBlendRateV; //0x0A24
    float GunBlendRateH; //0x0A28
    char pad_0A2C[4]; //0x0A2C
    void *JumpDataTemplate; //0x0A30
    bool UseFootPositionCtrl; //0x0A38
    char pad_0A39[743]; //0x0A39
    void *playerData; //0x0D20
    char pad_0D28[124]; //0x0D28
    float forceTargetAngleInitH; //0x0DA4
    float forceTargetAngleInitV; //0x0DA8
    float catchTargetAngle; //0x0DAC
    char pad_0DB0[16]; //0x0DB0
    float gunShotInterpolationFrame; //0x0DC0
    float gunShotReserveFrame; //0x0DC4
    char pad_0DC8[24]; //0x0DC8
    Vector4f lockOnTargetPosPress; //0x0DE0
    float sideStepBanTimer; //0x0DF0
    float collisionIndexChangeTimer; //0x0DF4
    char pad_0DF8[108]; //0x0DF8
    uint32_t playerID; //0x0E64
    char pad_0E68[40]; //0x0E68
    float moveBlendRate; //0x0E90
    float limitRunBlendRate; //0x0E94
    char pad_0E98[24]; //0x0E98
    void *playerStatus; //0x0EB0
    void *playerStatusTmp; //0x0EB8
    void *playerStatusTmp2; //0x0EC0
    char pad_0EC8[2]; //0x0EC8
    bool isBattle; //0x0ECA
    char pad_0ECB[5]; //0x0ECB
    bool isManualLockOn; //0x0ED0
    bool isDoubleLockOn; //0x0ED1
    char pad_0ED2[74]; //0x0ED2
    uint32_t homingType; //0x0F1C
    bool isIgnoreRotationLock; //0x0F20
    char pad_0F21[23]; //0x0F21
    float gunShotReserveTimer; //0x0F38
    char pad_0F3C[48]; //0x0F3C
    float obstacleInterpolationFrame; //0x0F6C
    char pad_0F70[56]; //0x0F70
    void *oneActionLockOnTarget; //0x0FA8
    char pad_0FB0[72]; //0x0FB0
    int32_t forceConstShortWep; //0x0FF8
    int32_t forceConstLongWep; //0x0FFC
    char pad_1000[40]; //0x1000
    int32_t cancelReserveBit; //0x1028
    char pad_102C[12]; //0x102C
    uint32_t actionTag; //0x1038
    uint32_t commonActionTag; //0x103C
    float forbidEvadeTimer; //0x1040
    char pad_1044[100]; //0x1044
    float neckRotationLerp; //0x10A8
    char pad_10AC[28]; //0x10AC
    uint32_t playerControlType; //0x10C8
    char pad_10CC[20]; //0x10CC
    bool playerNameHudEnable; //0x10E0
    char pad_10E1[15]; //0x10E1
    void *gachaCon; //0x10F0
    char pad_10F8[52]; //0x10F8
    uint32_t jumpCount; //0x112C
    char pad_1130[16]; //0x1130
    Vector4f inertiaVector; //0x1140
    Vector4f inertiaVectorNew; //0x1150
    Vector4f movingFloorSpeed; //0x1160
    Vector4f inertiaTmp; //0x1170
    float upperBlendRate; //0x1180
    float maxMoveBlendRate; //0x1184
    float minMoveBlendRate; //0x1188
    bool obstacleJumpFlag; //0x118C
    char pad_118D[11]; //0x118D
    float upperBlendOffsetY; //0x1198
    char pad_119C[24]; //0x119C
    bool isWaitNoInterpolation; //0x11B4
    char pad_11B5[3]; //0x11B5
    uint32_t currentMaterialType; //0x11B8
    uint32_t currentExMaterialType; //0x11BC
    int32_t currentMaterialPriority; //0x11C0
    int32_t overwriteJumpResourceID; //0x11C4
    bool overwriteJumpParameterType; //0x11C8
    bool reserveJumpStatusInit; //0x11C9
    char pad_11CA[2]; //0x11CA
    float jumpMoveSpeedXZ; //0x11CC
    void *jumpMoveSpeedCurve; //0x11D0
    char pad_11D8[24]; //0x11D8
    bool isNoDie; //0x11F0
    char pad_11F1[3]; //0x11F1
    int32_t eventCameraID; //0x11F4
    char pad_11F8[8]; //0x11F8
    Vector4f simpleMoveVector; //0x1200
    bool isShortWeaponOfBanConst; //0x1210
    char pad_1211[7]; //0x1211
    void *easyAutomaticController; //0x1218
    void *aiModeController; //0x1220
    bool isAI; //0x1228
    char pad_1229[3]; //0x1229
    bool isChasing; //0x122C
    char pad_122D[3]; //0x122D
    float actionChangeTimer; //0x1230
    bool playerAITypeSet; //0x1234
    char pad_1235[3]; //0x1235
    float automaticTimer; //0x1238
    char pad_123C[60]; //0x123C
    uint32_t puppetStylishRank; //0x1278
    bool puppetIsBattle; //0x127C
    char pad_127D[3]; //0x127D
    class AppLockOnObject *lockOnProvokeTarget; //0x1280
    char pad_1288[48]; //0x1288
    bool forceDieBlown; //0x12B8
    char pad_12B9[15]; //0x12B9
    uint32_t finishCameraCategory; //0x12C8
    char pad_12CC[20]; //0x12CC
    float bankStability; //0x12E0
    char pad_12E4[4]; //0x12E4
    void *lockOnTargetReserved; //0x12E8
    float ignoreDamageTimer; //0x12F0
    char pad_12F4[84]; //0x12F4
    uint64_t commandType; //0x1348
    char pad_1350[32]; //0x1350
    float padAngularVelocity; //0x1370
    float oldPadAngularVelocity; //0x1374
    char pad_1378[88]; //0x1378
    float maxRunBlendRate; //0x13D0
    char pad_13D4[68]; //0x13D4
    uint32_t sideStepCount; //0x1418
    float sideStepResetTimer; //0x141C
    char pad_1420[48]; //0x1420
    bool isSpecialComboStarted; //0x1450
    bool isSpecialComboPushed; //0x1451
    bool isSpecialComboCheckOnce; //0x1452
    char pad_1453[325]; //0x1453
    void *oldLockOnTarget; //0x1598
    char pad_15A0[96]; //0x15A0
    Vector4f safePosition; //0x1600
    float safePositionTimer; //0x1610
    char pad_1614[140]; //0x1614
    float reviveInvincibleTimer; //0x16A0
    char pad_16A4[12]; //0x16A4
    float minimumDamageTimer; //0x16B0
    int32_t minimumDamageCount; //0x16B4
    void *minimumDamageOwner; //0x16B8
    uint32_t minimumDamageType; //0x16C0
    char pad_16C4[16]; //0x16C4
    float InterpolationFrameGroup0; //0x16D4
    float InterpolationFrameGroup1; //0x16D8
    float InterpolationFrameGroup2; //0x16DC
    float InterpolationFrameGroup3; //0x16E0
    bool ReserveInterp; //0x16E4
    bool ReserveWorldBlend; //0x16E5
    char pad_16E6[26]; //0x16E6
    void *LeanBlendController; //0x1700
    char pad_1708[74]; //0x1708
    bool IsPuppetEventMoveLockMission; //0x1752
    bool isPuppetEventMoveLock; //0x1753
    char pad_1754[12]; //0x1754
}; //Size: 0x1760

class AppLockOnObject : public REManagedObject
{
public:
    class AppLockOnTargetWork *target; //0x0010
    bool isActive; //0x0018
    char pad_0019[7]; //0x0019
    class RETransform *parent; //0x0020
}; //Size: 0x0028

class AppLockOnTargetWork : public REManagedObject
{
public:
    uint32_t LockOnType; //0x0010
    char pad_0014[12]; //0x0014
    Vector4f LockOnOffset; //0x0020
    char pad_0030[8]; //0x0030
    float MaxRange; //0x0038
    uint32_t RangeType; //0x003C
    bool IsForceIncludeShellTargetSearch; //0x0040
    char pad_0041[7]; //0x0041
    class REGameObject *owner; //0x0048
    char pad_0050[32]; //0x0050
    bool forceLockOn; //0x0070
    bool isValid; //0x0071
    char pad_0072[1]; //0x0072
    bool isWallPL; //0x0073
    bool isWallCam; //0x0074
    char pad_0075[27]; //0x0075
    uint32_t Category; //0x0090
    float snatchTimer; //0x0094
    uint32_t ownerType; //0x0098
    char pad_009C[48]; //0x009C
}; //Size: 0x00CC

class AppPadManager : public REBehavior
{
public:
    char pad_0048[16]; //0x0048
    void *keyAssign; //0x0058
    class AppPadInput *padInput; //0x0060
}; //Size: 0x0068

class AppPadInput : public REManagedObject
{
public:
    char pad_0010[8]; //0x0010
    float padLAngle; //0x0018
    float padLAnglePrev; //0x001C
    float padLRotation; //0x0020
    char pad_0024[2]; //0x0024
    bool isInteractionKeyMask; //0x0026
    char pad_0027[5]; //0x0027
    uint32_t N00019932; //0x002C
    Vector2f N030; //0x0030
    char pad_0038[4]; //0x0038
    uint32_t buttons1; //0x003C
    uint32_t buttons2; //0x0040
    char pad_0044[12]; //0x0044
    Vector2f leftAxis; //0x0050
    char pad_0058[8]; //0x0058
    Vector2f rightAxis; //0x0060
    char pad_0068[24]; //0x0068
    uint32_t leverStatusL; //0x0080
    char pad_0084[44]; //0x0084
}; //Size: 0x00B0

class AppCameraHelperParameter : public REManagedObject
{
public:
    Vector4f position; //0x0010
    Vector4f pog; //0x0020
    float fov; //0x0030
    float nearClipPlane; //0x0034
    float farClipPlane; //0x0038
    float roll; //0x003C
    Vector4f direction; //0x0040
    Vector4f upDirection; //0x0050
}; //Size: 0x0060

class AppHumanoid : public REBehavior
{
public:
    char pad_0048[9]; //0x0048
    bool isIKBlendHand; //0x0051
    bool isIKBlendFoot; //0x0052
    char pad_0053[13]; //0x0053
    class AppIK2Bone *N000029D0; //0x0060
    class AppIK2Bone *N000029D1; //0x0068
    class AppIK2Bone *N000029D2; //0x0070
    class AppIK2Bone *N000029D3; //0x0078
    bool isMultiBone; //0x0080
    char pad_0081[175]; //0x0081
    bool isDebugDraw; //0x0130
    char pad_0131[15]; //0x0131
}; //Size: 0x0140

class AppIkBase : public REBehavior
{
public:
    char pad_0048[48]; //0x0048
}; //Size: 0x0078

class AppIK2Bone : public AppIkBase
{
public:
    char pad_0078[200]; //0x0078
}; //Size: 0x0140

class AppCutSceneManager : public REBehavior
{
public:
    char pad_0048[16]; //0x0048
    class AppEventGauntletData *myEventGauntletData; //0x0058
    class AppPrefab *humanArmPrefab; //0x0060
    class AppPrefab *humanArmHarnessPrefab; //0x0068
    class AppEventWetData *wetData; //0x0070
    char pad_0078[24]; //0x0078
    bool useEventLightEnableLateUpdate; //0x0090
    char pad_0091[167]; //0x0091
}; //Size: 0x0138

class AppEventWetData : public UserData
{
public:
    void *pl00EventWetData; //0x0030
    void *pl01EventWetData; //0x0038
    void *pl02EventWetData; //0x0040
}; //Size: 0x0048

class AppPrefab : public REManagedObject
{
public:
    char pad_0010[48]; //0x0010
}; //Size: 0x0040

class AppEventGauntletData : public UserData
{
public:
    char pad_0030[16]; //0x0030
}; //Size: 0x0040

class AppPostEffectController : public REBehavior
{
public:
    char pad_0048[32]; //0x0048
    class DotNetGenericList *blendInfos; //0x0068
    class AppPostEffectControllerControlData *controlData; //0x0070
    class AppPostEffectControllerBlendInfo *blendInfo; //0x0078
    char pad_0080[56]; //0x0080
}; //Size: 0x00B8

class AppMotionBlurController : public AppPostEffectController
{
public:
    uint32_t cameraTypeParam; //0x00B8
    char pad_00BC[4]; //0x00BC
    class AppMotionBlurParamBlender *motionBlurParamBlender; //0x00C0
    class AppPostEffectControllerControlData *controlData1; //0x00C8
    class AppPostEffectControllerControlData *controlData2; //0x00D0
}; //Size: 0x00D8

class AppPostEffectControllerControlDataBase : public REManagedObject
{
public:
    float blendTime; //0x0010
}; //Size: 0x0014

class AppPostEffectControllerControlData : public AppPostEffectControllerControlDataBase
{
public:
    float shutterAngle; //0x0014
    uint32_t blurLoopCount; //0x0018
    bool enabled; //0x001C
    char pad_001D[3]; //0x001D
}; //Size: 0x0020

class AppPostEffectControllerBlendInfo : public REManagedObject
{
public:
    float nowBlendTime; //0x0010
    uint32_t N000035D9; //0x0014
    class AppPostEffectControllerControlData *controlData; //0x0018
    uint32_t order; //0x0020
    char pad_0024[4]; //0x0024
}; //Size: 0x0028

class AppPostEffectParamBlenderBase : public REBehavior
{
public:
    char pad_0048[16]; //0x0048
    class AppPostEffectOverWriteReturnBlendData *N00003671; //0x0058
}; //Size: 0x0060

class AppPostEffectParamBlenderMotionBlurControlData : public AppPostEffectParamBlenderBase
{
public:
    char pad_0060[104]; //0x0060
}; //Size: 0x00C8

class AppMotionBlurParamBlender : public AppPostEffectParamBlenderMotionBlurControlData
{
public:
    class RERenderMotionBlur *motionBlur; //0x00C8
}; //Size: 0x00D0

class AppPostEffectOverWriteReturnBlendData : public REManagedObject
{
public:

}; //Size: 0x0010

class RERenderMotionBlur : public REComponent
{
public:
    char pad_0030[8]; //0x0030
    bool enabled; //0x0038
    char pad_0039[7]; //0x0039
}; //Size: 0x0040

class AppCameraSingleTargetCamera : public REManagedObject
{
public:
    float fov; //0x0010
    bool IsEnable; //0x0014
    char pad_0015[3]; //0x0015
    void *target; //0x0018
    void *targetCameraExtension; //0x0020
    char pad_0028[8]; //0x0028
    float circularPOGAngle; //0x0030
    float circularPOGSpeed; //0x0034
    void *ResDefaultParam; //0x0038
    void *ResOuterParam; //0x0040
    char pad_0048[24]; //0x0048
    void *cameraModeAddChangeInterpCurve; //0x0060
    float cameraModeAddChangeRate; //0x0068
    char pad_006C[52]; //0x006C
    float LockonBlendRate; //0x00A0
    char pad_00A4[4]; //0x00A4
    void *LockonBlendSmoothing; //0x00A8
    void *CommandBlendSmoothing; //0x00B0
    float CommandBlendRate; //0x00B8
    char pad_00BC[4]; //0x00BC
    void *LookDownOffsetSmoothing; //0x00C0
    float LookDownOffset; //0x00C8
    char pad_00CC[4]; //0x00CC
    Vector4f position; //0x00D0
    Vector4f pointOfGaze; //0x00E0
    float ZOffset; //0x00F0
    char pad_00F4[12]; //0x00F4
    Vector4f originPosition; //0x0100
    Vector4f originPointOfGaze; //0x0110
    Vector4f prevPosition; //0x0120
    Vector4f prevPointOfGaze; //0x0130
    float roll; //0x0140
    float horizontalAngle; //0x0144
    float VerticalManualOffset; //0x0148
    float WallEscapeVerticalOffset; //0x014C
    float VerticalCollisionOffsetTimer; //0x0150
    float HorizontalManualOffset; //0x0154
    float WallEscapeHorizontalOffset; //0x0158
    float HorizontalCollisionOffsetTimer; //0x015C
    float horizontalReverSpeedRate; //0x0160
    float verticalReverSpeedRate; //0x0164
    float ReverNutoralDampingTime; //0x0168
    bool isHitCollision; //0x016C
    bool isHitCollisionWall; //0x016D
    char pad_016E[10]; //0x016E
    void *commandWork; //0x0178
    uint32_t CommandBlendMode; //0x0180
    bool IsCommandSetFrame; //0x0184
    char pad_0185[3]; //0x0185
    uint32_t LastCameraTriggerableRange; //0x0188
    uint32_t CommandTriggerStatus; //0x018C
    char pad_0190[8]; //0x0190
    float PogRate; //0x0198
    float EyeVerticalAngle; //0x019C
    bool IsEyeVerticalIgnoreInterp; //0x01A0
    char pad_01A1[79]; //0x01A1
    float FixJointTimer; //0x01F0
    char pad_01F4[12]; //0x01F4
    Vector4f PrevPlayerPos; //0x0200
    void *BaseBossPosSmoothing; //0x0210
    float BaseBossPosSmoothRate; //0x0218
    char pad_021C[4]; //0x021C
    void *EyeRotateVerticalSmoothing; //0x0220
    void *EyeRotateHorizontalSmoothing; //0x0228
    float EyeRotateHorizontalLastSign; //0x0230
    char pad_0234[4]; //0x0234
    void *EyeInterpCurve; //0x0238
    float EyeResetInterpTime; //0x0240
    float EyeInterpTime; //0x0244
    bool IsEyeReset; //0x0248
    char pad_0249[3]; //0x0249
    uint32_t EyeResetType; //0x024C
    void *ManualCameraReduceRate; //0x0250
    void *WallEscapeCameraReduceRate; //0x0258
    char pad_0260[24]; //0x0260
    void *ZOffsetSmoothing; //0x0278
    void *AirZOffsetSmoothing; //0x0280
    char pad_0288[24]; //0x0288
    bool IsThroughCameraReset; //0x02A0
    char pad_02A1[3]; //0x02A1
    uint32_t ThroughCameraStatus; //0x02A4
    char pad_02A8[8]; //0x02A8
    Vector4f ThroughCameraSideVector; //0x02B0
    void *ThroughCameraInterpCurve; //0x02C0
    void *RushThroughParam; //0x02C8
    float TargetDisableTimer; //0x02D0
    float TargetLostTimer; //0x02D4
    float TargetLostTimeDef; //0x02D8
    float TargetLostMoveTimeDef; //0x02DC
    float ReFindPogDelayTimeDef; //0x02E0
    float ReFindPogDelayTimer; //0x02E4
    char pad_02E8[8]; //0x02E8
    Vector4f LostTargetPos; //0x02F0
    bool IsTargetLostInit; //0x0300
    bool IsRotateTargetLost; //0x0301
    char pad_0302[14]; //0x0302
    Vector4f LostTargetPOG; //0x0310
    Vector4f LostTargetCameraPos; //0x0320
    bool IsLostTargetInterpIgnore; //0x0330
    char pad_0331[3]; //0x0331
    uint32_t CalcType; //0x0334
    bool IsApplyCalcResult; //0x0338
}; //Size: 0x0339

class REThreadContext {
public:
    char pad_0000[80];                    // 0x0000
    class UnkThreadContextObject* unkPtr; // 0x0050
    char pad_0058[32];                    // 0x0058
    int32_t referenceCount;               // 0x0078
    char pad_007C[4];                     // 0x007C
};                                        // Size: 0x0080

class UnkThreadContextObject {
public:
    char pad_0000[24];  // 0x0000
    void* unkPtr;       // 0x0018
    char pad_0020[240]; // 0x0020
};                      // Size: 0x0110