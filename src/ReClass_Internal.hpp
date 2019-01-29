// Created with ReClass.NET by KN4CK3R

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

class N0000005F
{
public:
    char pad_0000[16]; //0x0000
    class N0000007A* N00000062; //0x0010
    class N00000085* N00000063; //0x0018
    char pad_0020[168]; //0x0020
}; //Size: 0x00C8

class N0000000A
{
public:
    char pad_0000[24]; //0x0000
    class N0000005F* N0000000E; //0x0018
    char pad_0020[280]; //0x0020
}; //Size: 0x0138

class N0000039C
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

class N000001FF
{
public:
    char pad_0000[120]; //0x0000
}; //Size: 0x0078

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

class N0000019E
{
public:
    char pad_0000[104]; //0x0000
    class N00000369* N000001AC; //0x0068
    class N000001C9* N000001AD; //0x0070
    char pad_0078[208]; //0x0078
}; //Size: 0x0148

class ManagedTypes
{
public:
    class N0000019E* N00000175[2048]; //0x0000
}; //Size: 0x4000

class N000002D1
{
public:
    char pad_0000[72]; //0x0000
}; //Size: 0x0048

class N000002A6
{
public:
    char pad_0000[328]; //0x0000
}; //Size: 0x0148

class N0000027C
{
public:
    class N000002D1* N0000027D; //0x0000
    char pad_0008[104]; //0x0008
    class N000002A6* N0000028B; //0x0070
    char pad_0078[200]; //0x0078
}; //Size: 0x0140

class N000003DE
{
public:
    char pad_0000[328]; //0x0000
}; //Size: 0x0148

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

class FunctionHolder
{
public:
    class FunctionDescriptor* descriptor; //0x0000
    char pad_0008[24]; //0x0008
}; //Size: 0x0020

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

class REVector
{
public:
    char pad_0000[8]; //0x0000
    class REVector* nextVector; //0x0008 idk why this is here.
    class N0000074B* data; //0x0010
    uint32_t num; //0x0018
    uint32_t maxItems; //0x001C
    char pad_0020[8]; //0x0020
    void* N0000072F; //0x0028
    uint32_t N00000730; //0x0030
    char pad_0034[4]; //0x0034
}; //Size: 0x0038

class REObjectInfo
{
public:
    class REClassInfo* classInfo; //0x0000
    char pad_0008[96]; //0x0008
}; //Size: 0x0068

class REClassInfo
{
public:
    char pad_0000[48]; //0x0000
    uint32_t size; //0x0030
    char pad_0034[52]; //0x0034
    class REType* type; //0x0068
    class REObjectInfo* N000009C2; //0x0070
    char pad_0078[280]; //0x0078
}; //Size: 0x0190

class REType
{
public:
    uint32_t typeIdentifier; //0x0008
    char pad_000C[4]; //0x000C
    class N000003DE* N000003B6; //0x0010
    uint32_t typeIndexProbably; //0x0018
    char pad_001C[4]; //0x001C
    char* name; //0x0020
    char pad_0028[4]; //0x0028
    uint32_t size; //0x002C
    char pad_0030[8]; //0x0030
    class REType* super; //0x0038
    char pad_0040[8]; //0x0040
    class REType* memberTypeOrSomething; //0x0048
    class REVector* functions; //0x0050
    class REClassInfo* classInfo; //0x0058

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
}; //Size: 0x0060

class CameraTypePtr
{
public:
    class REType* N000003AA; //0x0000
}; //Size: 0x0008

class REObject
{
public:
    class REObjectInfo* info; //0x0000
}; //Size: 0x0008

class REManagedObject : public REObject
{
public:
    char pad_0008[8]; //0x0008
}; //Size: 0x0010

class REScene
{
public:
    class REObjectInfo* N000008E7; //0x0000
    char pad_0008[152]; //0x0008
    class REGameObject* N00000E4F; //0x00A0
    char pad_00A8[102272]; //0x00A8
}; //Size: 0x19028

class N00003730
{
public:
    char pad_0000[136]; //0x0000
}; //Size: 0x0088

class N00007EEE
{
public:
    class N00003730* N00007EEF; //0x0000
    char pad_0008[128]; //0x0008
}; //Size: 0x0088

class JointInfo
{
public:
    wchar_t* name; //0x0000
    uint32_t nameHash; //0x0008
    int16_t jointArrayIndex; //0x000C minus 1
    int16_t jointNumber; //0x000E
    char pad_0010[48]; //0x0010
}; //Size: 0x0040

class N000070F5
{
public:
    Matrix3x4f localMatrix; //0x0000
}; //Size: 0x0030

class REJoint : public REManagedObject
{
public:
    class RETransform* parentTransform; //0x0010
    class JointInfo* info; //0x0018
    N000070F5 N000070F6; //0x0020
    char pad_0050[16]; //0x0050
}; //Size: 0x0060

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

class REJointArray
{
public:
    class N00003745* data; //0x0000
    int32_t size; //0x0008
    int32_t numAllocated; //0x000C
    class JointMatrices* matrices; //0x0010
}; //Size: 0x0018

class REString
{
public:
    char pad_0000[24]; //0x0000
    int32_t length; //0x0018 if len >= 12, is a pointer
    int32_t maxLength; //0x001C
}; //Size: 0x0020

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

class REGameObject : public REManagedObject
{
public:
    char pad_0010[8]; //0x0010
    class RETransform* transform; //0x0018
    class REFolder* folder; //0x0020
    REString name; //0x0028 This can either be a pointer to the name or embedded directly
    uint32_t N00000DDA; //0x0048
    float N00000695; //0x004C
}; //Size: 0x0050

class REComponent : public REManagedObject
{
public:
    class REGameObject* ownerGameObject; //0x0010
    class REComponent* idkComponent; //0x0018
    class REComponent* prevComponent; //0x0020
    class REComponent* nextComponent; //0x0028
}; //Size: 0x0030

class RETransform : public REComponent
{
public:
    Vector3f position; //0x0030
    char pad_003C[4]; //0x003C
    Vector3f angles; //0x0040
    char pad_004C[20]; //0x004C
    class REScene* scene; //0x0060
    class RETransform* transform1; //0x0068
    class RETransform* transform2; //0x0070
    class RETransform* parentTransform; //0x0078
    Matrix4x4f worldTransform; //0x0080
    class N00007EEE* N000007D8; //0x00C0
    int32_t N00000804; //0x00C8
    uint32_t N00003743; //0x00CC
    char pad_00D0[8]; //0x00D0
    REJointArray joints; //0x00D8
    char pad_00F0[32]; //0x00F0
}; //Size: 0x0110

class RECamera : public REComponent
{
public:
    float N0000044E; //0x0030
    float farClipPlane; //0x0034
    float fov; //0x0038
    float lookAtDistance; //0x003C
    char pad_0040[4]; //0x0040
    float aspectRatio; //0x0044
    int32_t N00000451; //0x0048
    char pad_004C[4]; //0x004C
    uint32_t cameraType; //0x0050
    char pad_0054[20]; //0x0054
    uint32_t N00000455; //0x0068
    char pad_006C[32]; //0x006C
    float N00000D40; //0x008C
    float N0000045A; //0x0090
    float N00000D43; //0x0094
    float N0000045B; //0x0098
    char pad_009C[4]; //0x009C
}; //Size: 0x00A0

class N0000092A
{
public:
    char pad_0000[72]; //0x0000
}; //Size: 0x0048

class N0000091E
{
public:
    class N0000092A* N00000920; //0x0000
    char pad_0008[56]; //0x0008
}; //Size: 0x0040

class N00000965
{
public:
    class REObjectInfo* object; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

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
    uint16_t N000076CE; //0x0030
    uint16_t sdafasd; //0x0032
    char pad_0034[4]; //0x0034
    uint32_t N000076CF; //0x0038
    uint32_t N0000705D; //0x003C
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

class ValueTriggerBoolean : public REManagedObject
{
public:
    uint32_t N0000784F; //0x0010
    char pad_0014[4]; //0x0014
}; //Size: 0x0018

class DampingFloat : public REManagedObject
{
public:
    char pad_0010[40]; //0x0010
    float N00007886; //0x0038
    float N00007891; //0x003C
    float N00007887; //0x0040
}; //Size: 0x0044

class REAnimationCurve : public REManagedObject
{
public:
    char pad_0010[8]; //0x0010
    float N00007904; //0x0018
    char pad_001C[4]; //0x001C
    float N00007905; //0x0020
    char pad_0024[36]; //0x0024
}; //Size: 0x0048

class TwirlerCameraSettings : public REManagedObject
{
public:
    wchar_t* settingsPath; //0x0010
    char pad_0018[24]; //0x0018
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
    uint32_t N00007737; //0x0100
    char pad_0104[12]; //0x0104
    class DampingFloat* controlDamping3; //0x0110
    class DampingFloat* controlDamping4; //0x0118
    char pad_0120[136]; //0x0120
    class REManagedObject* N0000774E; //0x01A8
    class REManagedObject* N0000774F; //0x01B0
    char pad_01B8[24]; //0x01B8
}; //Size: 0x01D0

class N00007C3D
{
public:
    char pad_0000[32]; //0x0000
}; //Size: 0x0020

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

class REMotion : public REComponent
{
public:
    char pad_0030[176]; //0x0030
    uint32_t N00007983; //0x00E0
    uint8_t N000079F6; //0x00E4
    char pad_00E5[115]; //0x00E5
    class N00007C3D* N00007992; //0x0158
    char pad_0160[32]; //0x0160
    class JointList* joints; //0x0180
    uint32_t numJoints; //0x0188
    char pad_018C[532]; //0x018C
}; //Size: 0x03A0

class REActorLayer : public REManagedObject
{
public:
    char pad_0010[56]; //0x0010
}; //Size: 0x0048

class N00007A09
{
public:
    class REActorLayer* N00007A0A; //0x0000
    char pad_0008[64]; //0x0008
}; //Size: 0x0048

class REActorMotion : public REComponent
{
public:
    class REMotion* motion; //0x0030
    char pad_0038[16]; //0x0038
    class N00007A09* N00007950; //0x0048
    char pad_0050[32]; //0x0050
}; //Size: 0x0070

class DotNetString_Implementation : public REManagedObject
{
public:
    class REManagedObject* N000036B9; //0x0010
    char pad_0018[8]; //0x0018
}; //Size: 0x0020

class DotNetGenericList : public REManagedObject
{
public:
    class REManagedObject* N00003701; //0x0010
    char pad_0018[8]; //0x0018
}; //Size: 0x0020

class N00006EC4 : public REComponent
{
public:
    char pad_0030[32]; //0x0030
}; //Size: 0x0050

class PhysicsCharacterController : public N00006EC4
{
public:
    char pad_0050[480]; //0x0050
}; //Size: 0x0230

class RopewayCameraSystem : public REBehavior
{
public:
    char pad_0048[8]; //0x0048
    float N00006F39; //0x0050
    char pad_0054[4]; //0x0054
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
    char pad_00A0[8]; //0x00A0
    class REManagedObject* N00006F3D; //0x00A8
    char pad_00B0[136]; //0x00B0
}; //Size: 0x0138
