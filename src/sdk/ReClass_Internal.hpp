// Created with ReClass.NET by KN4CK3R

class N0000000A
{
public:
    char pad_0000[24]; //0x0000
    class N0000005F* N0000000E; //0x0018
    char pad_0020[280]; //0x0020
}; //Size: 0x0138

class N0000005F
{
public:
    char pad_0000[16]; //0x0000
    class N0000007A* N00000062; //0x0010
    class N00000085* N00000063; //0x0018
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

class REClassInfo
{
public:
    uint16_t typeIndex; //0x0000 index into global type array
    char pad_0002[33]; //0x0002
    uint8_t N00002AFF; //0x0023
    char pad_0024[2]; //0x0024
    uint8_t objectType; //0x0026 1 == normal type ? ??
    char pad_0027[5]; //0x0027
    uint32_t sizeThing; //0x002C
    uint32_t size; //0x0030
    char pad_0034[44]; //0x0034
    class N00002B03* N000009C1; //0x0060
    class REType* type; //0x0068
    class REObjectInfo* parentInfo; //0x0070
}; //Size: 0x0078

class ManagedTypes
{
public:
    class REClassInfo* types[2048]; //0x0000
}; //Size: 0x4000

class N0000019E
{
public:
    char pad_0000[104]; //0x0000
    class N00000369* N000001AC; //0x0068
    class N000001C9* N000001AD; //0x0070
    char pad_0078[208]; //0x0078
}; //Size: 0x0148

class N000001C9
{
public:
    class N0000019E* parent; //0x0000
    void* N000001CB; //0x0008
    char pad_0010[40]; //0x0010
    void* N000001D1; //0x0038
    char pad_0040[8]; //0x0040
    void* N000001D3; //0x0048
    char pad_0050[16]; //0x0050
    class N000001FF* N000001D6; //0x0060
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
    class N000002D1* N0000027D; //0x0000
    char pad_0008[104]; //0x0008
    class N000002A6* N0000028B; //0x0070
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
    class N0000039C* N0000036A; //0x0000
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
    class REType* N000003AA; //0x0000
}; //Size: 0x0008

class REType
{
public:
    uint32_t typeIdentifier; //0x0008
    uint32_t N00000415; //0x000C
    char pad_0010[8]; //0x0010
    uint32_t typeIndexProbably; //0x0018
    char pad_001C[4]; //0x001C
    char* name; //0x0020
    char pad_0028[4]; //0x0028
    uint32_t size; //0x002C
    char pad_0030[8]; //0x0030
    class REType* super; //0x0038
    class REType* someSortOfType; //0x0040
    class REType* childType; //0x0048 pointed to at end
    class REFieldList* fields; //0x0050 getters, setters, and variables
    class REClassInfo* classInfo; //0x0058
    int32_t N0000AB48; //0x0060
    char pad_0064[4]; //0x0064

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
}; //Size: 0x0068

class N000003DE
{
public:
    char pad_0000[8]; //0x0000
    class N000008E5* N000003E0; //0x0008
    char pad_0010[312]; //0x0010
}; //Size: 0x0148

class REObject
{
public:
    class REObjectInfo* info; //0x0000
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
    class REGameObject* ownerGameObject; //0x0010
    class REComponent* childComponent; //0x0018 child component
    class REComponent* prevComponent; //0x0020
    class REComponent* nextComponent; //0x0028
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
    char pad_004C[4]; //0x004C
    int32_t cameraType; //0x0050
    char pad_0054[12]; //0x0054
    wchar_t* cameraName; //0x0060
    uint32_t N00000455; //0x0068
    char pad_006C[32]; //0x006C
    float N00000D40; //0x008C
    float N0000045A; //0x0090
    float N00000D43; //0x0094
    float N0000045B; //0x0098
    char pad_009C[252]; //0x009C
}; //Size: 0x0198

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
    char pad_0010[2]; //0x0010
    bool shouldUpdate; //0x0012
    bool shouldDraw; //0x0013
    bool shouldUpdateSelf; //0x0014
    bool shouldDrawSelf; //0x0015
    bool shouldSelect; //0x0016
    char pad_0017[1]; //0x0017
    class RETransform* transform; //0x0018
    class REFolder* folder; //0x0020
    REString name; //0x0028 This can either be a pointer to the name or embedded directly
    uint32_t N00000DDA; //0x0048
    float timescale; //0x004C
    char pad_0050[16]; //0x0050
}; //Size: 0x0060

class REFieldList
{
public:
    uint32_t N0000072A; //0x0000
    char pad_0004[4]; //0x0004
    class REFieldList* next; //0x0008 idk why this is here.
    class N0000074B* functions; //0x0010
    int32_t num; //0x0018
    int32_t maxItems; //0x001C
    class REVariableList* variables; //0x0020
    void* N0000072F; //0x0028
    uint32_t N00000730; //0x0030
    char pad_0034[4]; //0x0034
}; //Size: 0x0038

class N00000756
{
public:
    class FunctionHolder* func; //0x0000
}; //Size: 0x0008

class N0000074B
{
public:
    class N00000756* N0000074C[2048]; //0x0000
}; //Size: 0x4000

class FunctionHolder
{
public:
    class FunctionDescriptor* descriptor; //0x0000
    char pad_0008[24]; //0x0008
}; //Size: 0x0020

class FunctionDescriptor
{
public:
    char* name; //0x0000
    char pad_0008[16]; //0x0008
    void* functionPtr; //0x0018
    char pad_0020[8]; //0x0020
    char* returnTypeName; //0x0028
    char pad_0030[8]; //0x0030
}; //Size: 0x0038

class REJointArray
{
public:
    class N00003745* data; //0x0000
    int32_t size; //0x0008
    int32_t numAllocated; //0x000C
    class JointMatrices* matrices; //0x0010
    char pad_0018[256]; //0x0018
}; //Size: 0x0118

class RETransform : public REComponent
{
public:
    Vector4f position; //0x0030
    Vector4f angles; //0x0040
    Vector4f something; //0x0050
    class REScene* scene; //0x0060
    class RETransform* firstChildTransform; //0x0068
    class RETransform* childTransform; //0x0070
    class RETransform* parentTransform; //0x0078
    Matrix4x4f worldTransform; //0x0080
    class N00007EEE* N000007D8; //0x00C0
    int32_t N00000804; //0x00C8
    uint32_t tickCount; //0x00CC
    char pad_00D0[1]; //0x00D0
    bool N0000081A; //0x00D1
    char pad_00D2[1]; //0x00D2
    bool N00000817; //0x00D3
    char pad_00D4[4]; //0x00D4
    REJointArray joints; //0x00D8
    char pad_01F0[8]; //0x01F0
}; //Size: 0x01F8

class N0000B6D9
{
public:
    class REManagedObject* object; //0x0000
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
    N0000B6D9 someHugeArray[65536]; //0x0028
    char pad_180028[1176]; //0x180028
    class RETransform* N0000B786; //0x1804C0
    class REFolder* firstFolder; //0x1804C8
    REString name; //0x1804D0
    char pad_1804F0[720]; //0x1804F0
    int64_t N0000B7E3; //0x1807C0
    class REManagedObject* N0000B7E4; //0x1807C8
}; //Size: 0x1807D0

class N0000091E
{
public:
    class N0000092A* N00000920; //0x0000
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
    class REObjectInfo* object; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class REObjectInfo
{
public:
    class REClassInfo* classInfo; //0x0000
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

class N00000CF1
{
public:
    class REObjectInfo* N00000CF2; //0x0000
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
    class REObjectInfo* object; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class REFolder : public REManagedObject
{
public:
    char pad_0010[24]; //0x0010
    REString name; //0x0028
    REString name2; //0x0048
    REString name3; //0x0068
    class REFolder* parentFolder; //0x0088
    class REFolder* childFolder; //0x0090
    class REFolder* childFolder2; //0x0098
    char pad_00A0[40]; //0x00A0
    class REScene* scene; //0x00C8
}; //Size: 0x00D0

class N00000D61
{
public:
    class REObjectInfo* N00000D62; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class N00000DFF
{
public:
    class REObjectInfo* N00000E00; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class N00000E15
{
public:
    class REObjectInfo* N00000E16; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class N00000E2B
{
public:
    class REObjectInfo* N00000E2C; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class N00004041
{
public:
    class REObjectInfo* N00004042; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class ASDF
{
public:
    class RETransform* N0000406D; //0x0000
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
    class REObjectInfo* N00004089; //0x0000
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
    class RECamera* activeCamera; //0x00A8
    class REJoint* joint; //0x00B0
    class RECameraParam* cameraParam; //0x00B8
    class ValueTriggerBoolean* N0000772F; //0x00C0
    char pad_00C8[8]; //0x00C8
    class REBehavior* cascade; //0x00D0
    class RECameraParam* cameraParam2; //0x00D8
    char pad_00E0[8]; //0x00E0
    class DampingFloat* controlDamping; //0x00E8
    class DampingFloat* controlDamping2; //0x00F0
    class TwirlerCameraSettings* cameraLimitSettings; //0x00F8
    bool isNotUsingWeapon; //0x0100 IDK, but it gets set to true when not using a weap.
    char pad_0101[7]; //0x0101
    float pitch; //0x0108
    float yaw; //0x010C
    class DampingFloat* controlDamping3; //0x0110
    class DampingFloat* controlDamping4; //0x0118
    char pad_0120[24]; //0x0120
    class DampingFloat* controlDamping5; //0x0138
    uint32_t cameraMode; //0x0140 3 == in weapon?
    char pad_0144[4]; //0x0144
    class RopewayCameraInterpolationPrame* N0000773F; //0x0148
    char pad_0150[72]; //0x0150
    uint32_t N00007748; //0x0198
    char pad_019C[60]; //0x019C
}; //Size: 0x01D8

class REJoint : public REManagedObject
{
public:
    class RETransform* parentTransform; //0x0010
    class REJointDesc* info; //0x0018
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
    uint32_t N0000784F; //0x0010
    char pad_0014[4]; //0x0014
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
    wchar_t* settingsPath; //0x0010
    char pad_0018[16]; //0x0018
    uint32_t N000078BE; //0x0028
    uint32_t N000078DE; //0x002C
    float N000078BF; //0x0030
    char pad_0034[4]; //0x0034
    class REAnimationCurve* animationCurve; //0x0038
    class REAnimationCurve* animationCurve2; //0x0040
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
    class CurveKeyFrames* keyframes; //0x0030
    class CurveKeyFrames* keyframes2; //0x0038
    char pad_0040[16]; //0x0040
}; //Size: 0x0050

class REActorMotion : public REComponent
{
public:
    class REMotion* motion; //0x0030
    char pad_0038[16]; //0x0038
    class N00007A09* N00007950; //0x0048
    char pad_0050[32]; //0x0050
}; //Size: 0x0070

class REMotion : public REComponent
{
public:
    char pad_0030[120]; //0x0030
    class N000026F0* N0000797C; //0x00A8
    char pad_00B0[48]; //0x00B0
    uint32_t N00007983; //0x00E0
    uint8_t N000079F6; //0x00E4
    char pad_00E5[115]; //0x00E5
    class N00007C3D* N00007992; //0x0158
    char pad_0160[32]; //0x0160
    class JointList* joints; //0x0180
    uint32_t numJoints; //0x0188
    char pad_018C[532]; //0x018C
}; //Size: 0x03A0

class N00007A09
{
public:
    class REActorLayer* N00007A0A; //0x0000
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
    N00007E99 joints[2048]; //0x0000
}; //Size: 0x20000

class N00007C3D
{
public:
    char pad_0000[32]; //0x0000
}; //Size: 0x0020

class N00007EEE
{
public:
    class N00003730* N00007EEF; //0x0000
    char pad_0008[128]; //0x0008
}; //Size: 0x0088

class DotNetString_Implementation : public REManagedObject
{
public:
    class REManagedObject* N000036B9; //0x0010
    char pad_0018[8]; //0x0018
}; //Size: 0x0020

class DotNetGenericList : public REManagedObject
{
public:
    class REArrayThing* data; //0x0010
    char pad_0018[72]; //0x0018
}; //Size: 0x0060

class N00003730
{
public:
    char pad_0000[136]; //0x0000
}; //Size: 0x0088

class N00003745
{
public:
    class REJoint* joints[2048]; //0x0000
}; //Size: 0x4000

class N0000379E
{
public:
    Matrix4x4f worldMatrix; //0x0000
}; //Size: 0x0040

class JointMatrices
{
public:
    N0000379E data[2048]; //0x0000
}; //Size: 0x20000

class REJointDesc
{
public:
    wchar_t* name; //0x0000
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
    class PhysicsCollidable* collidable; //0x01E8
    class PhysicsFilterInfo* filterInfo; //0x01F0
    char pad_01F8[56]; //0x01F8
}; //Size: 0x0230

class RopewayCameraSystem : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    float N00006F39; //0x0050
    char pad_0054[1]; //0x0054
    uint8_t N000009D7; //0x0055
    char pad_0056[2]; //0x0056
    class DotNetGenericList* N00006F3A; //0x0058
    class DotNetGenericList* N00006F3B; //0x0060
    float N00006F3C; //0x0068
    float N00007054; //0x006C
    class REManagedObject* castRayHandle; //0x0070
    class REManagedObject* castSphereHandle; //0x0078
    class REManagedObject* castRayHandle2; //0x0080
    class REGameObject* N00006F2E; //0x0088
    char pad_0090[8]; //0x0090
    class RopewayPlayerCameraController* cameraController; //0x0098
    class RopewayPlayerCameraController* previousController; //0x00A0
    class REManagedObject* N00006F3D; //0x00A8
    class DampingFloat* damping; //0x00B0
    class REManagedObject* N00006F3F; //0x00B8
    class RECamera* mainCamera; //0x00C0
    class REManagedObject* N00006F41; //0x00C8
    class DotNetGenericList* N00006F42; //0x00D0
    class REComponent* N00006F43; //0x00D8
    class REJoint* playerJoint; //0x00E0 Joint belonging to the camera pivot entity (player)
    class RopewayMainCameraController* mainCameraController; //0x00E8
}; //Size: 0x00F0

class N000070F5
{
public:
    Matrix3x4f localMatrix; //0x0000
}; //Size: 0x0030

class RopewayIkController : public REBehavior
{
public:
    char pad_0048[12]; //0x0048
    uint8_t ikFlags; //0x0054
    char pad_0055[3]; //0x0055
    uint8_t ikFlags2; //0x0058
    char pad_0059[7]; //0x0059
    class REArrayThing* N000071C9; //0x0060
    class REArrayThing* N000071CA; //0x0068
    char pad_0070[8]; //0x0070
    class SystemString* someString; //0x0078
    char pad_0080[32]; //0x0080
    class SystemString* someString2; //0x00A0
    char pad_00A8[40]; //0x00A8
    class SystemString* someString3; //0x00D0
    float N0000A946; //0x00D8
    float N0000AA6C; //0x00DC
    float N0000A947; //0x00E0
    float N0000AA6F; //0x00E4
    float N0000A948; //0x00E8
    float N0000AA72; //0x00EC
    char pad_00F0[8]; //0x00F0
    class N0000A9C1* armFitIkUserData; //0x00F8
    char pad_0100[52]; //0x0100
    float N0000AA84; //0x0134
    char pad_0138[48]; //0x0138
    class REArrayThing* N0000A958; //0x0168
    char pad_0170[8]; //0x0170
    class REArrayThing* N0000A95A; //0x0178
    char pad_0180[16]; //0x0180
    class REManagedObject* N0000A95D; //0x0190
    char pad_0198[24]; //0x0198
    class REManagedObject* N0000A961; //0x01B0
    class REManagedObject* N0000A962; //0x01B8
    char pad_01C0[8]; //0x01C0
    class ObjectPointer* jointPtr; //0x01C8
    char pad_01D0[216]; //0x01D0
}; //Size: 0x02A8

class UserData : public REManagedObject
{
public:
    REString name; //0x0010
}; //Size: 0x0030

class N0000A9C1 : public UserData
{
public:
    class REManagedObject* N0000A9C3; //0x0030
}; //Size: 0x0038

class SystemString : public REManagedObject
{
public:
    int32_t size; //0x0010
    wchar_t data[12]; //0x0014
    char pad_002C[96]; //0x002C
}; //Size: 0x008C

class N0000AA93 : public REManagedObject
{
public:
    class REClassInfo* N0000AA95; //0x0010
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
    class REClassInfo* classInfo; //0x0010
    uint32_t num1; //0x0018
    uint32_t num2; //0x001C
    class REManagedObject* object; //0x0020
    char pad_0028[112]; //0x0028
}; //Size: 0x0098

class REVariableList
{
public:
    char pad_0000[8]; //0x0000
    class N0000ADA4* data; //0x0008
    int32_t num; //0x0010
    int32_t maxItems; //0x0014
}; //Size: 0x0018

class VariableDescriptor
{
public:
    char* name; //0x0000
    char pad_0008[8]; //0x0008
    void* function; //0x0010
    uint32_t flags; //0x0018 (flags AND 0x1F) - 1 gives var type
    uint32_t flags2; //0x001C
    char* typeName; //0x0020
    char pad_0028[4]; //0x0028
    uint32_t variableType; //0x002C 1 == pointer? 3 == builtin?
    class StaticVariableDescriptor* staticVariableData; //0x0030
    char pad_0038[8]; //0x0038
}; //Size: 0x0040

class N0000ADA4
{
public:
    class VariableDescriptor* descriptors[256]; //0x0000
}; //Size: 0x0800

class N0000B627
{
public:
    char pad_0000[8]; //0x0000
    class N0000B632* N0000B629; //0x0008
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
    class N0000B89B* N0000B899; //0x0000
}; //Size: 0x0008

class JointDescDTbl
{
public:
    class JointDescData* data; //0x0000
    uint32_t num; //0x0008
    char pad_000C[4]; //0x000C
}; //Size: 0x0010

class REMotionDummySkeleton : public REMotionStructure
{
public:
    SkeletonResourceHandle skeletonResourceHandle; //0x0030
    JointDescDTbl jointDescTbl; //0x0038
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
    REJointDesc data[256]; //0x0000
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
    char pad_0078[872]; //0x0078
    Vector4f position; //0x03E0
    Vector4f orientation; //0x03F0
    char pad_0400[68]; //0x0400
}; //Size: 0x0444

class RopewayMainCameraController : public REBehavior
{
public:
    char pad_0048[92]; //0x0048
    float fov; //0x00A4
    bool controllerEnabled; //0x00A8
    char pad_00A9[15]; //0x00A9
    class REAnimationCurve* N00000817; //0x00B8
    class DotNetGenericList* cameraShakes; //0x00C0
    class REGameObject* mainCameraObject; //0x00C8
    class RECamera* mainCamera; //0x00D0
    class REJoint* N0000081B; //0x00D8
    class AppliedCameraShakeParam* appliedCameraShakeParam; //0x00E0
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
    class FieldInfoArray* N00000A1A; //0x0000
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
    FieldInfo N00000A1D[2000000]; //0x0000
}; //Size: 0x3D09000

class TypeListArrayPtr
{
public:
    class TypeListArray* N00000A47; //0x0000
}; //Size: 0x0008

class TypeListArray
{
public:
    REClassInfo N00000A51[100000]; //0x0000
}; //Size: 0xB71B00

class GlobalArrayData2
{
public:
    char pad_0000[104]; //0x0000
    class N00000A8A* N00000A6A; //0x0068
    class REObjectInfo* objectInfo; //0x0070
}; //Size: 0x0078

class N00000A8A
{
public:
    char pad_0000[72]; //0x0000
}; //Size: 0x0048

class SomeGlobalArrayThingPtr
{
public:
    class N00000AA4* N00000A9B; //0x0000
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
    class N00001283* data; //0x0000
    uint32_t size; //0x0008
    char pad_000C[4]; //0x000C
}; //Size: 0x0010

class N00000AA4
{
public:
    N00000CB2 N00000AA7[127]; //0x0008
    char pad_0400[280]; //0x0400
    N00001242 N00000ACB[256]; //0x0518
    char pad_3D18[88]; //0x3D18
    ContainerThing staticVariableLists[6]; //0x3D70
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

class StaticVariables
{
public:
    char pad_0000[72]; //0x0000
}; //Size: 0x0048

class N00001283
{
public:
    class StaticVariables* N00001284[100000]; //0x0000
}; //Size: 0xC3500

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

class RenderBounds
{
public:
    Vector2f top; //0x0000
    Vector2f bottom; //0x0008
}; //Size: 0x0010

class RERenderLayerScene : public RERenderLayer
{
public:
    class RECamera* camera; //0x0078
    char pad_0080[8]; //0x0080
    class RenderTargetState* targetStates[16]; //0x0088
    char pad_0108[4544]; //0x0108
    RenderBounds renderBounds[3]; //0x12C8
    char pad_12F8[224]; //0x12F8
}; //Size: 0x13D8

class N000026F0
{
public:
    char pad_0000[76]; //0x0000
    float N0000270E; //0x004C
    class REJoint* joint; //0x0050
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
    class RopewayIlluminationManager* N000013CC; //0x0000
}; //Size: 0x0008

class RopewayIlluminationManager : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    class REManagedObject* N000013E2; //0x0050
    class DotNetGenericDictionary* mapIdsToIlluminationContainer; //0x0058
    uint32_t shouldUseFlashlight; //0x0060
    uint32_t someCounter; //0x0064
    bool shouldUseFlashlight2; //0x0068
    char pad_0069[7]; //0x0069
    class REManagedObject* N000013FB; //0x0070
    class DotNetGenericList* N000013F4; //0x0078
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
    class RopewayCameraSystem* N000013EC; //0x0000
}; //Size: 0x0008

class RopewayCameraControllerInfo : public REManagedObject
{
public:
    char pad_0010[8]; //0x0010
    class REGameObject* controllerObject; //0x0018
    char pad_0020[8]; //0x0020
}; //Size: 0x0028

class CameraControllerList : public REManagedObject
{
public:
    char pad_0010[16]; //0x0010
    class RopewayCameraControllerInfo* N00001407[64]; //0x0020
}; //Size: 0x0220

class DotNetGenericDictionary : public REManagedObject
{
public:
    class N00001440* N00001434; //0x0010
    class N0000144E* N00001435; //0x0018
    char pad_0020[16]; //0x0020
    class REManagedObject* equalityComparer; //0x0030
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
    class REGameObject* owner; //0x0010
    class RopewayCameraPositionParam* posParam1; //0x0018
    class RopewayCameraPositionParam* posParam2; //0x0020
    class DampingFloat* dampingFloat; //0x0028
}; //Size: 0x0030

class RopewayCameraPositionParam : public REManagedObject
{
public:
    class REGameObject* attachedOwner; //0x0010
    class REJoint* attachedJoint; //0x0018
    class RopewayCameraTransitionParam* transitionParam; //0x0020
}; //Size: 0x0028

class RopewayCameraTransitionParam : public REManagedObject
{
public:
    class RopewayPlayerCameraParam* playerCameraParam; //0x0010
    char pad_0018[136]; //0x0018
}; //Size: 0x00A0

class RopewayPlayerCameraParam : public REManagedObject
{
public:
    class REAnimationCurve* curve1; //0x0010
    Vector2f vec; //0x0018
    class REAnimationCurve* curve2; //0x0020
    class REAnimationCurve* curve3; //0x0028
    class SystemString* name; //0x0030
    char pad_0038[104]; //0x0038
}; //Size: 0x00A0

class RopewaySetPostEffectParam : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    uint32_t N0000156E; //0x0050
    uint32_t N000015AE; //0x0054
    char pad_0058[24]; //0x0058
    class DotNetGenericDictionary* N00001572; //0x0070
    char pad_0078[56]; //0x0078
}; //Size: 0x00B0

class RopewayPostEffectController : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    class REAnimationCurve* curve1; //0x0050
    class REAnimationCurve* curve2; //0x0058
    class DotNetGenericList* N000015B6; //0x0060
    char pad_0068[24]; //0x0068
}; //Size: 0x0080

class RESecondaryAnimation : public REComponent
{
public:
    char pad_0030[16]; //0x0030
    uint32_t N000015EF; //0x0040
    char pad_0044[4]; //0x0044
}; //Size: 0x0048

class ActorLayerList
{
public:
    class N00001630* data; //0x0000
    uint32_t numLayers; //0x0008
    uint32_t numAllocated; //0x000C
}; //Size: 0x0010

class REActorMotionCamera : public RESecondaryAnimation
{
public:
    ActorLayerList layers; //0x0048
}; //Size: 0x0058

class N00001630
{
public:
    class REActorLayer* layers[2048]; //0x0000
}; //Size: 0x4000

class RopewaySweetLightController : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    uint32_t N00001470; //0x0050
    uint32_t N0000152A; //0x0054
    uint32_t N00001471; //0x0058
    uint32_t N0000152D; //0x005C
    class RERenderSpotLight* renderSpotLight; //0x0060
    class RERenderProjectionSpotlight* renderProjectionSpotLight; //0x0068
    class RopewaySpotLightParam* param1; //0x0070
    class RopewaySpotLightParam* param2; //0x0078
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
    class SweetLightContextContainer* contexts; //0x0050
    class RopewaySweetLightParam* param1; //0x0058
    class RopewaySweetLightParam* param2; //0x0060
    uint8_t N0000154E; //0x0068
    char pad_0069[7]; //0x0069
    class REManagedObject* N0000154F; //0x0070
}; //Size: 0x0078

class RopewaySweetLightParam : public REManagedObject
{
public:
    class RopewaySpotLightParam* spotlight; //0x0010
}; //Size: 0x0018

class RopewaySweetLightManagerContext : public REManagedObject
{
public:
    class RopewaySweetLightController* controller; //0x0010
    class RopewaySpotLightParam* param1; //0x0018
    class RopewaySpotLightParam* param2; //0x0020
    char pad_0028[8]; //0x0028
    class DotNetGenericList* N000015B1; //0x0030
    class RopewaySpotLightParam* param3; //0x0038
    char pad_0040[8]; //0x0040
}; //Size: 0x0048

class N00002B03
{
public:
    char pad_0000[200]; //0x0000
}; //Size: 0x00C8

class REArrayBase : public REManagedObject
{
public:
    class REClassInfo* containedType; //0x0010
    int32_t num1; //0x0018
    int32_t numElements; //0x001C
}; //Size: 0x0020

class SweetLightContextContainer : public REArrayBase
{
public:
    class RopewaySweetLightManagerContext* data[2]; //0x0020
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
    class REJoint* constJoint; //0x0058
    class DampingFloat* radius; //0x0060
    class DampingFloat* height; //0x0068
    class DampingVec3* offset; //0x0070
    uint32_t offsetType; //0x0078
    char pad_007C[4]; //0x007C
    class DotNetGenericList* controllerRequests; //0x0080
    char pad_0088[8]; //0x0088
    class PhysicsCharacterController* physicsCharacterController; //0x0090
    class PhysicsRequestSetCollider* physicsRequestSetCollider; //0x0098
    class RopewaySurvivorCharacterControllerUserData* userData; //0x00A0
    float N00001647; //0x00A8
    char pad_00AC[4]; //0x00AC
    class PhysicsCollider* collider; //0x00B0
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
    class REPtrArray* data; //0x0030
    float N00001634; //0x0038
    float N0000165B; //0x003C
}; //Size: 0x0040

class REPtrArray : public REArrayBase
{
public:
    class REManagedObject* data[2048]; //0x0020
}; //Size: 0x4020

class SurvivorCharacterControllerUserDataParam : public REManagedObject
{
public:
    char pad_0010[8]; //0x0010
    class SystemString* string1; //0x0018
    char pad_0020[8]; //0x0020
    class SystemString* string2; //0x0028
    class SurvivorControllerUserDataShape* shape; //0x0030
}; //Size: 0x0038

class SurvivorControllerUserDataShape : public REManagedObject
{
public:
    class SystemString* jointName; //0x0010
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
    class N000017C9* data; //0x0000
    uint32_t numElements; //0x0008
    uint32_t numAllocated; //0x000C
}; //Size: 0x0010

class PhysicsCollidable : public REManagedObject
{
public:
    uint32_t N000016F4; //0x0010
    char pad_0014[4]; //0x0014
    class PhysicsCapsuleShape* shape1; //0x0018
    class PhysicsCapsuleShape* shape2; //0x0020
    char pad_0028[8]; //0x0028
    class PhysicsFilterInfo* filterInfo; //0x0030
    char pad_0038[8]; //0x0038
    class REGameObject* owner; //0x0040
    uint32_t N000016FB; //0x0048
    uint32_t N000017B7; //0x004C
    uint32_t N000016FC; //0x0050
    char pad_0054[4]; //0x0054
    PhysicsColliderVector colliderControllers; //0x0058 interesting
    char pad_0068[16]; //0x0068
}; //Size: 0x0078

class PhysicsCollider : public PhysicsCollidable
{
public:
    char pad_0078[288]; //0x0078
}; //Size: 0x0198

class PhysicsShape : public REManagedObject
{
public:
    char pad_0010[24]; //0x0010
    class PhysicsCollider* parentCollider; //0x0028
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
    uint32_t maskBits; //0x0020
    char pad_0024[12]; //0x0024
}; //Size: 0x0030

class N000017C9
{
public:
    class REBehavior* element[2048]; //0x0000
}; //Size: 0x4000

class RopewaySurvivorTargetBankController : public REBehavior
{
public:
    char pad_0048[16]; //0x0048
    class REMotion* motion; //0x0058
    char pad_0060[32]; //0x0060
    class RopewaySurvivorPlayerCondition* playerCondition; //0x0080
}; //Size: 0x0088

class RopewaySurvivorPlayerCondition : public REBehavior
{
public:
    char pad_0048[24]; //0x0048
    class RopewaySurvivorDefineDamageParam* damageParam; //0x0060
    char pad_0068[6]; //0x0068
    bool forceUseFlashlight; //0x006E
    char pad_006F[17]; //0x006F
    float N0000185B; //0x0080
    char pad_0084[4]; //0x0084
    float sessionTime; //0x0088
    char pad_008C[4]; //0x008C
    class RopewayTimer* timer1; //0x0090
    class RopewayTimer* timer2; //0x0098
    char pad_00A0[16]; //0x00A0
    class REGameObject* flashlight; //0x00B0
    char pad_00B8[72]; //0x00B8
    class RopewaySurvivorPlayerController* playerController; //0x0100
    class RopewaySurvivorPlayerActionOrderer* actionOrderer; //0x0108
    char pad_0110[16]; //0x0110
    class RopewaySurvivorInventory* inventory; //0x0120
    class RopewaySurvivorEquipment* equipment; //0x0128
    class REMotion* playerMotion; //0x0130
    char pad_0138[360]; //0x0138
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
    char pad_0010[4]; //0x0010
    float interval; //0x0014
}; //Size: 0x0018

class RopewayTimer : public RopewayTimerBase
{
public:
    char pad_0018[24]; //0x0018
}; //Size: 0x0030

class RopewaySurvivorInventory : public REBehavior
{
public:
    char pad_0048[64]; //0x0048
    class RopewaySurvivorPlayerCondition* parentCondition; //0x0088
    char pad_0090[56]; //0x0090
}; //Size: 0x00C8

class RopewaySurvivorEquipment : public REBehavior
{
public:
    char pad_0048[24]; //0x0048
    class RopewayImplementArm* primaryWeapon; //0x0060
    class RopewayImplementArm* sideWeapon; //0x0068
    class RopewayImplementGun* currentWeapon; //0x0070
    class RopewayArmList* weaponList; //0x0078
    char pad_0080[16]; //0x0080
    class REManagedObject* valTrigger; //0x0090
    char pad_0098[104]; //0x0098
    class RopewaySurvivorPlayerCondition* parentCondition; //0x0100
    char pad_0108[24]; //0x0108
    float currentAccuracy; //0x0120
    bool isPerfectAccuracy; //0x0124
    char pad_0125[3]; //0x0125
}; //Size: 0x0128

class RopewayArmList : public REManagedObject
{
public:
    class RopewayArmList_Impl* data; //0x0010
}; //Size: 0x0018

class RopewayImplement : public REBehavior
{
public:
    char pad_0048[56]; //0x0048
    class REJoint* joint; //0x0080
    class RopewayImplementVirtualJoint* virtualJoint; //0x0088
    class RERenderMesh* mesh; //0x0090
    class REMotion* implementMotion; //0x0098
    class REMotion* parentMotion; //0x00A0
    class RopewayStayAreaController* stayAreaController; //0x00A8
    class RopewayIkController* ikController; //0x00B0
    class RopewayCharacterHandler* characterHandler; //0x00B8
    class RopewayMotionEventHandler* implementMotionHandler; //0x00C0
    class RopewayMotionEventHandler* parentMotionHandler; //0x00C8
    class RopewayWwiseContainerApp* wwiseContainerApp; //0x00D0
    char pad_00D8[8]; //0x00D8
    class RopewaySurvivorEquipment* parentEquipment; //0x00E0
    class UserData* implementUserData; //0x00E8
}; //Size: 0x00F0

class RopewayImplementArm : public RopewayImplement
{
public:
    char pad_00F0[88]; //0x00F0
}; //Size: 0x0148

class RopewayArmList_Impl : public REArrayBase
{
public:
    class RopewayImplementArm* arms[2048]; //0x0020
}; //Size: 0x4020

class RopewayCharacterHandler : public REBehavior
{
public:
    char pad_0048[184]; //0x0048
    class DotNetGenericDictionary* N00001A9D; //0x0100
    class DotNetGenericDictionary* N00001A9E; //0x0108
    class DotNetGenericDictionary* N00001A9F; //0x0110
    class DotNetGenericDictionary* N00001AA0; //0x0118
    char pad_0120[16]; //0x0120
    class PhysicsRequestSetCollider* requestSetCollider; //0x0130
    class RopewayIkController* ikController; //0x0138
    class RopewayPressController* pressController; //0x0140
    class RopewayGroundFixer* groundFixer; //0x0148
    class RopewayPlRainEffect* rainEffect; //0x0150
    char pad_0158[8]; //0x0158
    class REDynamicsRagdoll* ragdoll; //0x0160
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
    class REDynamicsWorld* world; //0x0030
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
    REString name; //0x00F0
    char pad_0110[200]; //0x0110
}; //Size: 0x01D8

class REDynamicsWorld : public REManagedObject
{
public:
    char pad_0010[152]; //0x0010
    class REScene* scene; //0x00A8
    char pad_00B0[4448]; //0x00B0
    class REDynamicsWorld* world; //0x1210
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
    class REMotion* motion; //0x0018
    char pad_0020[8]; //0x0020
    class REManagedObject* param; //0x0028
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
    class REMotion* motion; //0x0058
    class REManagedObject* N00002175; //0x0060
    class REPtrArray* layers; //0x0068
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
    class REPtrArray* autoTriggerReceivers; //0x0230
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
    class RopewayTimer* timer1; //0x00E0
    class RopewayTimer* timer2; //0x00E8
    class RopewayTimer* timer3; //0x00F0
    char pad_00F8[8]; //0x00F8
    class RopewaySurvivorPlayerCondition* parentCondition; //0x0100
}; //Size: 0x0108

class RopewaySurvivorController : public REBehavior
{
public:
    char pad_0048[16]; //0x0048
    class DampingRangeFloat* charAngle; //0x0058
    class DampingRangeFloat* moveAngle; //0x0060
    class DampingRangeFloat* watchAngle; //0x0068
    class DampingRangeFloat* watchAngleX; //0x0070
    class DampingRangeFloat* wheelBlendRate; //0x0078
    class DampingRangeFloat* wheelSpeed; //0x0080
    class REAnimationCurve* wheelBlendRateCurve; //0x0088
    class REAnimationCurve* wheelSpeedCurve; //0x0090
    bool autoUpdateCharDir; //0x0098
    bool autoUpdateMoveDir; //0x0099
    bool autoUpdateWatchDir; //0x009A
    bool autoUpdateWheel; //0x009B
    char pad_009C[4]; //0x009C
    class REAnimationCurve* stairToSlopeCurve; //0x00A0
    class DampingFloat* dampingStairValue; //0x00A8
    class RopewaySurvivorControllerStepCheck* stepChecker; //0x00B0
    char pad_00B8[8]; //0x00B8
    class RopewaySurvivorPlayerCondition* parentCondition; //0x00C0
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
    class RopewaySurvivorPlayerCondition* parentCondition_; //0x00E8
}; //Size: 0x00F0

class RopewaySurvivorControllerStepCheck : public REManagedObject
{
public:
    Vector4f footLPos; //0x0010
    Vector4f footRPos; //0x0020
    float footDistance; //0x0030
    float weight; //0x0034
    class REJoint* leftJoint; //0x0038
    class REJoint* rightJoint; //0x0040
    class SystemString* leftName; //0x0048
    class SystemString* rightName; //0x0050
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
    N000024E5 data[2048]; //0x0000
}; //Size: 0x8000

class N000024E0
{
public:
    Vector4f N000024E1; //0x0000
}; //Size: 0x0010