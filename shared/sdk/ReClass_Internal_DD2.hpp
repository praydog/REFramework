// Created with ReClass.NET 1.2 by KN4CK3R

class N0000000A
{
public:
	char pad_0000[24]; //0x0000
	class N0000005F *N0000000E; //0x0018
	char pad_0020[280]; //0x0020
}; //Size: 0x0138
static_assert(sizeof(N0000000A) == 0x138);

class N0000005F
{
public:
	char pad_0000[16]; //0x0000
	class N0000007A *N00000062; //0x0010
	class N00000085 *N00000063; //0x0018
	char pad_0020[168]; //0x0020
}; //Size: 0x00C8
static_assert(sizeof(N0000005F) == 0xC8);

class N0000007A
{
public:
	char pad_0000[40]; //0x0000
	uint32_t N00000080; //0x0028
	char pad_002C[28]; //0x002C
}; //Size: 0x0048
static_assert(sizeof(N0000007A) == 0x48);

class N00000085
{
public:
	char pad_0000[32]; //0x0000
	uint32_t N0000008A; //0x0020
	char pad_0024[36]; //0x0024
}; //Size: 0x0048
static_assert(sizeof(N00000085) == 0x48);

class ManagedTypes
{
public:
	class REClassInfo *types[2048]; //0x0000
}; //Size: 0x4000
static_assert(sizeof(ManagedTypes) == 0x4000);

class N0000019E
{
public:
	char pad_0000[104]; //0x0000
	class N00000369 *N000001AC; //0x0068
	class N000001C9 *N000001AD; //0x0070
	char pad_0078[208]; //0x0078
}; //Size: 0x0148
static_assert(sizeof(N0000019E) == 0x148);

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
static_assert(sizeof(N000001C9) == 0x70);

class N000001FF
{
public:
	char pad_0000[120]; //0x0000
}; //Size: 0x0078
static_assert(sizeof(N000001FF) == 0x78);

class N0000027C
{
public:
	class N000002D1 *N0000027D; //0x0000
	char pad_0008[104]; //0x0008
	class N000002A6 *N0000028B; //0x0070
	char pad_0078[200]; //0x0078
}; //Size: 0x0140
static_assert(sizeof(N0000027C) == 0x140);

class N000002A6
{
public:
	char pad_0000[328]; //0x0000
}; //Size: 0x0148
static_assert(sizeof(N000002A6) == 0x148);

class N000002D1
{
public:
	char pad_0000[72]; //0x0000
}; //Size: 0x0048
static_assert(sizeof(N000002D1) == 0x48);

class N00000369
{
public:
	class N0000039C *N0000036A; //0x0000
	char pad_0008[320]; //0x0008
}; //Size: 0x0148
static_assert(sizeof(N00000369) == 0x148);

class N0000039C
{
public:
	char pad_0000[72]; //0x0000
}; //Size: 0x0048
static_assert(sizeof(N0000039C) == 0x48);

class CameraTypePtr
{
public:
	class REType *N000003AA; //0x0000
}; //Size: 0x0008
static_assert(sizeof(CameraTypePtr) == 0x8);

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
static_assert(sizeof(REType) == 0x60);

class N000003DE
{
public:
	char pad_0000[8]; //0x0000
	class N000008E5 *N000003E0; //0x0008
	char pad_0010[312]; //0x0010
}; //Size: 0x0148
static_assert(sizeof(N000003DE) == 0x148);

class REObject
{
public:
	class REObjectInfo *info; //0x0000
}; //Size: 0x0008
static_assert(sizeof(REObject) == 0x8);

class REManagedObject : public REObject
{
public:
	uint32_t referenceCount; //0x0008
	int16_t N000071AE; //0x000C
	char pad_000E[2]; //0x000E
}; //Size: 0x0010
static_assert(sizeof(REManagedObject) == 0x10);

class REComponent : public REManagedObject
{
public:
	class REGameObject *ownerGameObject; //0x0010
	class REComponent *childComponent; //0x0018
	class REComponent *prevComponent; //0x0020
	class REComponent *nextComponent; //0x0028
}; //Size: 0x0030
static_assert(sizeof(REComponent) == 0x30);

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
	wchar_t *cameraName; //0x0060
	uint32_t N00000455; //0x0068
	char pad_006C[32]; //0x006C
	float N00000D40; //0x008C
	float N0000045A; //0x0090
	float N00000D43; //0x0094
	float N0000045B; //0x0098
	char pad_009C[252]; //0x009C
}; //Size: 0x0198
static_assert(sizeof(RECamera) == 0x198);

class REGameObject : public REManagedObject
{
public:
	bool shouldUpdate; //0x0010
	bool shouldDraw; //0x0011
	bool shouldUpdateSelf; //0x0012
	bool shouldDrawSelf; //0x0013
	bool shouldSelect; //0x0014
	char pad_0015[3]; //0x0015
	class RETransform *transform; //0x0018
	class REFolder *folder; //0x0020
	class SystemString *name; //0x0028 This can either be a pointer to the name or embedded directly
	uint32_t N00000DDA; //0x0030
	float timescale; //0x0034
}; //Size: 0x0038
static_assert(sizeof(REGameObject) == 0x38);

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
	void* deserializer; //0x0028
	uint32_t N00000730; //0x0030
}; //Size: 0x0034
static_assert(sizeof(REFieldList) == 0x34);

class N0000074B
{
public:
	class FunctionHolder **N0000074C[2048]; //0x0000
}; //Size: 0x4000
static_assert(sizeof(N0000074B) == 0x4000);

class N00000756
{
public:
	class FunctionHolder *func; //0x0000
}; //Size: 0x0008
static_assert(sizeof(N00000756) == 0x8);

class FunctionHolder
{
public:
	class FunctionDescriptor *descriptor; //0x0000
	char pad_0008[24]; //0x0008
}; //Size: 0x0020
static_assert(sizeof(FunctionHolder) == 0x20);

class FunctionDescriptor
{
public:
	char *name; //0x0000
	class MethodParamInfo (*params)[256]; //0x0008
	char pad_0010[4]; //0x0010
	int32_t numParams; //0x0014
	void* functionPtr; //0x0018
	uint32_t returnTypeFlag; //0x0020 AND 1F = via::reflection::TypeKind
	uint32_t typeIndex; //0x0024
	char pad_0028[8]; //0x0028
	char *returnTypeName; //0x0030
	char pad_0038[72]; //0x0038
}; //Size: 0x0080
static_assert(sizeof(FunctionDescriptor) == 0x80);

class REJointArray
{
public:
	class REArrayBase *data; //0x0000
	class JointMatrices *matrices; //0x0008
	char pad_0010[256]; //0x0010
}; //Size: 0x0110
static_assert(sizeof(REJointArray) == 0x110);

class RETransform : public REComponent
{
public:
	Vector4f position; //0x0030
	Vector4f angles; //0x0040
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
	char pad_00D2[1]; //0x00D2
	bool absoluteScaling; //0x00D3
	char pad_00D4[4]; //0x00D4
	class REJointArray joints; //0x00D8
	char pad_01E8[8]; //0x01E8
}; //Size: 0x01F0
static_assert(sizeof(RETransform) == 0x1F0);

class N0000B6D9
{
public:
	class REManagedObject *object; //0x0000
	uint32_t v1; //0x0008
	uint32_t v2; //0x000C
	uint32_t v3; //0x0010
	uint32_t v4; //0x0014
}; //Size: 0x0018
static_assert(sizeof(N0000B6D9) == 0x18);

class REString
{
public:
	char pad_0000[24]; //0x0000
	int32_t length; //0x0018 if len >= 12, is a pointer
	int32_t maxLength; //0x001C
}; //Size: 0x0020
static_assert(sizeof(REString) == 0x20);

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
static_assert(sizeof(REScene) == 0x1807D0);

class N0000091E
{
public:
	class N0000092A *N00000920; //0x0000
	char pad_0008[56]; //0x0008
}; //Size: 0x0040
static_assert(sizeof(N0000091E) == 0x40);

class N0000092A
{
public:
	char pad_0000[72]; //0x0000
}; //Size: 0x0048
static_assert(sizeof(N0000092A) == 0x48);

class N00000965
{
public:
	class REObjectInfo *object; //0x0000
	char pad_0008[64]; //0x0008
}; //Size: 0x0048
static_assert(sizeof(N00000965) == 0x48);

class REObjectInfo
{
public:
	class REClassInfo *classInfo; //0x0000
	void* validator; //0x0008
	void* N0000246A; //0x0010
	void* getType; //0x0018
	void* toString; //0x0020
	void* copy; //0x0028
	void* N00000995; //0x0030
	void* N00000996; //0x0038
	void* N00000997; //0x0040
	void* N00000998; //0x0048
	void* N00000999; //0x0050
	void* getSize; //0x0058
	void* N0000099B; //0x0060
	void* N0000099C; //0x0068
	void* N0000246C; //0x0070
	void* N0000246D; //0x0078
	void* N0000246E; //0x0080
	void* N0000246F; //0x0088
	void* N00002470; //0x0090
	void* N00002471; //0x0098
	void* N00002472; //0x00A0
	void* N00002473; //0x00A8
}; //Size: 0x00B0
static_assert(sizeof(REObjectInfo) == 0xB0);

class REClassInfo
{
public:
	uint16_t typeIndex; //0x0000 index into global type array
	char pad_0002[5]; //0x0002
	uint8_t objectFlags; //0x0007 flags >> 5 ==  1 == normal type ? ??
	uint32_t _; //0x0008
	uint32_t elementBitField; //0x000C >> 4 is the value type index (RETypeImpl)
	uint32_t typeFlags; //0x0010 System::Reflection::TypeAttributes or via::clr::TypeFlag
	uint32_t size; //0x0014
	uint32_t fqnHash; //0x0018
	uint32_t typeCRC; //0x001C
	uint32_t defaultCtor; //0x0020
	uint32_t vt; //0x0024 vtable byte pool
	uint32_t memberMethod; //0x0028
	uint32_t memberField; //0x002C
	uint32_t memberProp; //0x0030
	uint32_t memberEvent; //0x0034
	int32_t interfaces; //0x0038
	int32_t generics; //0x003C byte pool
	class RETypeCLR *type; //0x0040
	class REObjectInfo *parentInfo; //0x0048
}; //Size: 0x0050
static_assert(sizeof(REClassInfo) == 0x50);

class N00000CF1
{
public:
	class REObjectInfo *N00000CF2; //0x0000
	char pad_0008[64]; //0x0008
}; //Size: 0x0048
static_assert(sizeof(N00000CF1) == 0x48);

class RERenderOutput : public REComponent
{
public:
	char pad_0030[144]; //0x0030
}; //Size: 0x00C0
static_assert(sizeof(RERenderOutput) == 0xC0);

class N00000D1D
{
public:
	class REObjectInfo *object; //0x0000
	char pad_0008[64]; //0x0008
}; //Size: 0x0048
static_assert(sizeof(N00000D1D) == 0x48);

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
static_assert(sizeof(REFolder) == 0xD0);

class N00000D61
{
public:
	class REObjectInfo *N00000D62; //0x0000
	char pad_0008[64]; //0x0008
}; //Size: 0x0048
static_assert(sizeof(N00000D61) == 0x48);

class N00000DFF
{
public:
	class REObjectInfo *N00000E00; //0x0000
	char pad_0008[64]; //0x0008
}; //Size: 0x0048
static_assert(sizeof(N00000DFF) == 0x48);

class N00000E15
{
public:
	class REObjectInfo *N00000E16; //0x0000
	char pad_0008[64]; //0x0008
}; //Size: 0x0048
static_assert(sizeof(N00000E15) == 0x48);

class N00000E2B
{
public:
	class REObjectInfo *N00000E2C; //0x0000
	char pad_0008[64]; //0x0008
}; //Size: 0x0048
static_assert(sizeof(N00000E2B) == 0x48);

class N00004041
{
public:
	class REObjectInfo *N00004042; //0x0000
	char pad_0008[64]; //0x0008
}; //Size: 0x0048
static_assert(sizeof(N00004041) == 0x48);

class ASDF
{
public:
	class RETransform *N0000406D; //0x0000
	char pad_0008[80]; //0x0008
}; //Size: 0x0058
static_assert(sizeof(ASDF) == 0x58);

class SceneObjectList : public ASDF
{
public:
	char pad_0058[48]; //0x0058
}; //Size: 0x0088
static_assert(sizeof(SceneObjectList) == 0x88);

class N00004087
{
public:
	class REObjectInfo *N00004089; //0x0000
	char pad_0008[56]; //0x0008
}; //Size: 0x0040
static_assert(sizeof(N00004087) == 0x40);

class REToneMapping : public REComponent
{
public:
	void *N0000410F; //0x0030
	float N00004110; //0x0038
	bool enabled; //0x003C
	char pad_003D[3]; //0x003D
}; //Size: 0x0040
static_assert(sizeof(REToneMapping) == 0x40);

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
static_assert(sizeof(REBehavior) == 0x48);

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
static_assert(sizeof(RECameraParam) == 0x70);

class RopewayPlayerCameraController : public REBehavior
{
public:
	char pad_0048[24]; //0x0048
	Vector4f pivotPosition; //0x0060
	Vector4f pivotRotation; //0x0070
	Vector4f worldPosition; //0x0080
	Vector4f worldRotation; //0x0090 quaternion
	char pad_00A0[24]; //0x00A0
	class RECamera *activeCamera; //0x00B8
	class REJoint *joint; //0x00C0
	class RECameraParam *cameraParam; //0x00C8
	class ValueTriggerBoolean *N0000772F; //0x00D0
	char pad_00D8[8]; //0x00D8
	class REBehavior *cascade; //0x00E0
	class RECameraParam *cameraParam2; //0x00E8
	char pad_00F0[8]; //0x00F0
	class DampingFloat *controlDamping; //0x00F8
	class DampingFloat *controlDamping2; //0x0100
	class TwirlerCameraSettings *cameraLimitSettings; //0x0108
	bool isNotUsingWeapon; //0x0110 IDK, but it gets set to true when not using a weap.
	char pad_0111[7]; //0x0111
	float pitch; //0x0118
	float yaw; //0x011C
	float pitchVelocity; //0x0120
	float yawVelocity; //0x0124
	class DampingFloat *controlDamping3; //0x0128
	class DampingFloat *controlDamping4; //0x0130
	char pad_0138[32]; //0x0138
	class DampingFloat *controlDamping5; //0x0158
	char pad_0160[16]; //0x0160
	uint32_t cameraMode; //0x0170 3 == in weapon?
	char pad_0174[4]; //0x0174
	class RopewayCameraInterpolationPrame *N0000773F; //0x0178
	char pad_0180[72]; //0x0180
	uint32_t N00007748; //0x01C8
	char pad_01CC[52]; //0x01CC
	float N00002602; //0x0200
	char pad_0204[28]; //0x0204
}; //Size: 0x0220
static_assert(sizeof(RopewayPlayerCameraController) == 0x220);

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
static_assert(sizeof(REJoint) == 0x60);

class ValueTriggerBoolean : public REManagedObject
{
public:
	bool current; //0x0010
	bool old; //0x0011
	char pad_0012[2]; //0x0012
	uint32_t N0000785B; //0x0014
}; //Size: 0x0018
static_assert(sizeof(ValueTriggerBoolean) == 0x18);

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
static_assert(sizeof(DampingFloat) == 0x44);

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
static_assert(sizeof(TwirlerCameraSettings) == 0x78);

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
static_assert(sizeof(REAnimationCurve) == 0x50);

class REActorMotion : public REComponent
{
public:
	class REMotion *motion; //0x0030
	char pad_0038[16]; //0x0038
	class N00007A09 *N00007950; //0x0048
	char pad_0050[32]; //0x0050
}; //Size: 0x0070
static_assert(sizeof(REActorMotion) == 0x70);

class REAnimation : public REComponent
{
public:
	char pad_0030[20]; //0x0030
	uint32_t playState; //0x0044
	char pad_0048[4]; //0x0048
	float currentPlaySpeed; //0x004C
	float secondaryPlaySpeed; //0x0050
	float unk54; //0x0054
	char pad_0058[4]; //0x0058
	float updateIntervalFrame; //0x005C
	char pad_0060[8]; //0x0060
	bool enabled; //0x0068
	bool playReset; //0x0069
	bool N00002408; //0x006A
	bool N00002411; //0x006B
	bool N00002409; //0x006C
	bool visualUpdate; //0x006D
	bool skipUpdate; //0x006E
	bool N000023ED; //0x006F
	bool N0000B974; //0x0070
	bool N00002415; //0x0071
	bool alwaysCallBeginEnd; //0x0072
	bool N00002402; //0x0073
	bool afterParentAnimation; //0x0074
	bool forceSkipUpdate; //0x0075
	bool enableIntervalUpdateScondaryAnimation; //0x0076
	char pad_0077[1]; //0x0077
}; //Size: 0x0078
static_assert(sizeof(REAnimation) == 0x78);

class REMotion : public REAnimation
{
public:
	char pad_0078[48]; //0x0078
	class N0000256E *joints; //0x00A8
	uint32_t numJoints; //0x00B0
	char pad_00B4[4]; //0x00B4
	class REGameObject *gameObject; //0x00B8
	char pad_00C0[89]; //0x00C0
	bool updateRootOnly; //0x0119
	char pad_011A[174]; //0x011A
	class JointList *joints2; //0x01C8
	int32_t numJoints2; //0x01D0
	char pad_01D4[512]; //0x01D4
	float N00002458; //0x03D4
	char pad_03D8[32]; //0x03D8
	uint32_t intervalUpdateOption; //0x03F8
	char pad_03FC[4]; //0x03FC
}; //Size: 0x0400
static_assert(sizeof(REMotion) == 0x400);

class N00007A09
{
public:
	class REActorLayer *N00007A0A; //0x0000
	char pad_0008[64]; //0x0008
}; //Size: 0x0048
static_assert(sizeof(N00007A09) == 0x48);

class REActorLayer : public REManagedObject
{
public:
	char pad_0010[56]; //0x0010
}; //Size: 0x0048
static_assert(sizeof(REActorLayer) == 0x48);

class JointList
{
public:
	Matrix4x4f joints[2048]; //0x0000
}; //Size: 0x20000
static_assert(sizeof(JointList) == 0x20000);

class N00007E99
{
public:
	Matrix4x4f N00007E9A; //0x0000
}; //Size: 0x0040
static_assert(sizeof(N00007E99) == 0x40);

class N00007EEE
{
public:
	class N00003730 *N00007EEF; //0x0000
	char pad_0008[128]; //0x0008
}; //Size: 0x0088
static_assert(sizeof(N00007EEE) == 0x88);

class DotNetString_Implementation : public REManagedObject
{
public:
	class REManagedObject *N000036B9; //0x0010
	char pad_0018[8]; //0x0018
}; //Size: 0x0020
static_assert(sizeof(DotNetString_Implementation) == 0x20);

class DotNetGenericList : public REManagedObject
{
public:
	class REArrayBase *data; //0x0010
	char pad_0018[72]; //0x0018
}; //Size: 0x0060
static_assert(sizeof(DotNetGenericList) == 0x60);

class N00003730
{
public:
	char pad_0000[136]; //0x0000
}; //Size: 0x0088
static_assert(sizeof(N00003730) == 0x88);

class N00003745
{
public:
	class REJoint *joints[2048]; //0x0000
}; //Size: 0x4000
static_assert(sizeof(N00003745) == 0x4000);

class N0000379E
{
public:
	Matrix4x4f worldMatrix; //0x0000
}; //Size: 0x0040
static_assert(sizeof(N0000379E) == 0x40);

class JointMatrices
{
public:
	class N0000379E data[2048]; //0x0000
}; //Size: 0x20000
static_assert(sizeof(JointMatrices) == 0x20000);

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
static_assert(sizeof(REJointDesc) == 0x40);

class PhysicsCollidableBase : public REComponent
{
public:
	char pad_0030[32]; //0x0030
}; //Size: 0x0050
static_assert(sizeof(PhysicsCollidableBase) == 0x50);

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
static_assert(sizeof(PhysicsCharacterController) == 0x230);

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
	char pad_0101[71]; //0x0101
	class DotNetGenericList *stringList; //0x0148
	char pad_0150[8]; //0x0150
}; //Size: 0x0158
static_assert(sizeof(RopewayCameraSystem) == 0x158);

class N000070F5
{
public:
	Matrix3x4f localMatrix; //0x0000
}; //Size: 0x0030
static_assert(sizeof(N000070F5) == 0x30);

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
static_assert(sizeof(RopewayIkController) == 0x2A8);

class UserData : public REManagedObject
{
public:
	class REString name; //0x0010
}; //Size: 0x0030
static_assert(sizeof(UserData) == 0x30);

class N0000A9C1 : public UserData
{
public:
	class REManagedObject *N0000A9C3; //0x0030
}; //Size: 0x0038
static_assert(sizeof(N0000A9C1) == 0x38);

class SystemString : public REManagedObject
{
public:
	int32_t size; //0x0010
	wchar_t data[256]; //0x0014
}; //Size: 0x0214
static_assert(sizeof(SystemString) == 0x214);

class N0000AA93 : public REManagedObject
{
public:
	class REClassInfo *N0000AA95; //0x0010
	char pad_0018[112]; //0x0018
}; //Size: 0x0088
static_assert(sizeof(N0000AA93) == 0x88);

class N0000AAAC : public REManagedObject
{
public:
	char pad_0010[48]; //0x0010
}; //Size: 0x0040
static_assert(sizeof(N0000AAAC) == 0x40);

class ObjectPointer : public REManagedObject
{
public:
	class REClassInfo *classInfo; //0x0010
	uint32_t num1; //0x0018
	uint32_t num2; //0x001C
	class REManagedObject *object; //0x0020
	char pad_0028[112]; //0x0028
}; //Size: 0x0098
static_assert(sizeof(ObjectPointer) == 0x98);

class REVariableList
{
public:
	char pad_0000[8]; //0x0000
	class N0000ADA4 *data; //0x0008
	int32_t num; //0x0010
	int32_t maxItems; //0x0014
}; //Size: 0x0018
static_assert(sizeof(REVariableList) == 0x18);

class N0000ADA4
{
public:
	class VariableDescriptor *descriptors[256]; //0x0000
}; //Size: 0x0800
static_assert(sizeof(N0000ADA4) == 0x800);

class VariableDescriptor
{
public:
	char *name; //0x0000
	uint32_t nameHash; //0x0008
	uint16_t flags1; //0x000C
	uint16_t N00008140; //0x000E
	void* function; //0x0010
	int32_t flags; //0x0018 (flags AND 0x1F) gives var type (via::clr::reflection::TypeKind)
	uint32_t typeFqn; //0x001C
	char *typeName; //0x0020
	int32_t getter; //0x0028
	union //0x002C 1 == pointer? 3 == builtin?
	{
		uint32_t variableType; //0x0000
		uint32_t destructor; //0x0000
	};
	class StaticVariableDescriptor *staticVariableData; //0x0030
	int32_t setter; //0x0038
	int32_t attributes; //0x003C
	char pad_0040[8]; //0x0040
}; //Size: 0x0048
static_assert(sizeof(VariableDescriptor) == 0x48);

class N0000B627
{
public:
	char pad_0000[8]; //0x0000
	class N0000B632 *N0000B629; //0x0008
	char pad_0010[56]; //0x0010
}; //Size: 0x0048
static_assert(sizeof(N0000B627) == 0x48);

class N0000B632
{
public:
	char pad_0000[136]; //0x0000
}; //Size: 0x0088
static_assert(sizeof(N0000B632) == 0x88);

class REMotionStructure : public REComponent
{
public:
}; //Size: 0x0030
static_assert(sizeof(REMotionStructure) == 0x30);

class SkeletonResourceHandle
{
public:
	class N0000B89B *N0000B899; //0x0000
}; //Size: 0x0008
static_assert(sizeof(SkeletonResourceHandle) == 0x8);

class JointDescDTbl
{
public:
	class JointDescData *data; //0x0000
	uint32_t num; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
static_assert(sizeof(JointDescDTbl) == 0x10);

class REMotionDummySkeleton : public REMotionStructure
{
public:
	class SkeletonResourceHandle skeletonResourceHandle; //0x0030
	class JointDescDTbl jointDescTbl; //0x0038
	char pad_0048[8]; //0x0048
}; //Size: 0x0050
static_assert(sizeof(REMotionDummySkeleton) == 0x50);

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
static_assert(sizeof(N0000B89B) == 0x88);

class JointDescData
{
public:
	class REJointDesc data[256]; //0x0000
}; //Size: 0x4000
static_assert(sizeof(JointDescData) == 0x4000);

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
static_assert(sizeof(StaticVariableDescriptor) == 0x15);

class REMotionCamera : public REAnimation
{
public:
	char pad_0078[872]; //0x0078
	Vector4f position; //0x03E0
	Vector4f orientation; //0x03F0
	char pad_0400[68]; //0x0400
}; //Size: 0x0444
static_assert(sizeof(REMotionCamera) == 0x444);

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
	char pad_00E8[88]; //0x00E8
}; //Size: 0x0140
static_assert(sizeof(RopewayMainCameraController) == 0x140);

class N00000878
{
public:
	char pad_0000[8]; //0x0000
}; //Size: 0x0008
static_assert(sizeof(N00000878) == 0x8);

class N0000087A : public N00000878
{
public:
}; //Size: 0x0008
static_assert(sizeof(N0000087A) == 0x8);

class N000008E5
{
public:
	char pad_0000[328]; //0x0000
}; //Size: 0x0148
static_assert(sizeof(N000008E5) == 0x148);

class AppliedCameraShakeParam : public REManagedObject
{
public:
	char pad_0010[48]; //0x0010
}; //Size: 0x0040
static_assert(sizeof(AppliedCameraShakeParam) == 0x40);

class FieldInfoPtr
{
public:
	class FieldInfoArray *N00000A1A; //0x0000
}; //Size: 0x0008
static_assert(sizeof(FieldInfoPtr) == 0x8);

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
static_assert(sizeof(FieldInfo) == 0x20);

class FieldInfoArray
{
public:
	class FieldInfo N00000A1D[2000000]; //0x0000
}; //Size: 0x3D09000
static_assert(sizeof(FieldInfoArray) == 0x3D09000);

class TypeListArrayPtr
{
public:
	class TypeListArray *N00000A47; //0x0000
}; //Size: 0x0008
static_assert(sizeof(TypeListArrayPtr) == 0x8);

class TypeListArray
{
public:
	class REClassInfo N00000A51[100000]; //0x0000
}; //Size: 0x7A1200
static_assert(sizeof(TypeListArray) == 0x7A1200);

class GlobalArrayData2
{
public:
	char pad_0000[104]; //0x0000
	class N00000A8A *N00000A6A; //0x0068
	class REObjectInfo *objectInfo; //0x0070
}; //Size: 0x0078
static_assert(sizeof(GlobalArrayData2) == 0x78);

class N00000A8A
{
public:
	char pad_0000[72]; //0x0000
}; //Size: 0x0048
static_assert(sizeof(N00000A8A) == 0x48);

class SomeGlobalArrayThingPtr
{
public:
	class N00000AA4 *N00000A9B; //0x0000
}; //Size: 0x0008
static_assert(sizeof(SomeGlobalArrayThingPtr) == 0x8);

class N00000CB2
{
public:
	char pad_0000[8]; //0x0000
}; //Size: 0x0008
static_assert(sizeof(N00000CB2) == 0x8);

class N00001242
{
public:
	char pad_0000[56]; //0x0000
}; //Size: 0x0038
static_assert(sizeof(N00001242) == 0x38);

class ContainerThing
{
public:
	class N00001283 *data; //0x0000
	uint32_t size; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
static_assert(sizeof(ContainerThing) == 0x10);

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
static_assert(sizeof(N00000AA4) == 0x7870);

class N00001283
{
public:
	class StaticVariables *N00001284[100000]; //0x0000
}; //Size: 0xC3500
static_assert(sizeof(N00001283) == 0xC3500);

class StaticVariables
{
public:
	char pad_0000[72]; //0x0000
}; //Size: 0x0048
static_assert(sizeof(StaticVariables) == 0x48);

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
static_assert(sizeof(StaticVariables_RopewayCameraSystem) == 0x880);

class RERenderLayer : public REManagedObject
{
public:
	char pad_0010[104]; //0x0010
}; //Size: 0x0078
static_assert(sizeof(RERenderLayer) == 0x78);

class RenderBounds
{
public:
	Vector2f top; //0x0000
	Vector2f bottom; //0x0008
}; //Size: 0x0010
static_assert(sizeof(RenderBounds) == 0x10);

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
static_assert(sizeof(RERenderLayerScene) == 0x13D8);

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
static_assert(sizeof(RenderTargetState) == 0x40);

class N000026F0
{
public:
	char pad_0000[76]; //0x0000
	float N0000270E; //0x004C
	class REJoint *joint; //0x0050
	char pad_0058[48]; //0x0058
}; //Size: 0x0088
static_assert(sizeof(N000026F0) == 0x88);

class RopewaySystemRingBufferController : public REBehavior
{
public:
	char pad_0048[128]; //0x0048
}; //Size: 0x00C8
static_assert(sizeof(RopewaySystemRingBufferController) == 0xC8);

class RopewayIlluminationManagerPtr
{
public:
	class RopewayIlluminationManager *N000013CC; //0x0000
}; //Size: 0x0008
static_assert(sizeof(RopewayIlluminationManagerPtr) == 0x8);

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
static_assert(sizeof(RopewayIlluminationManager) == 0x80);

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
static_assert(sizeof(RopewayStayAreaController) == 0xF0);

class RopewayCameraSystemPtr
{
public:
	class RopewayCameraSystem *N000013EC; //0x0000
}; //Size: 0x0008
static_assert(sizeof(RopewayCameraSystemPtr) == 0x8);

class CameraControllerList : public REManagedObject
{
public:
	char pad_0010[16]; //0x0010
	class RopewayCameraControllerInfo *N00001407[64]; //0x0020
}; //Size: 0x0220
static_assert(sizeof(CameraControllerList) == 0x220);

class RopewayCameraControllerInfo : public REManagedObject
{
public:
	char pad_0010[8]; //0x0010
	class REGameObject *controllerObject; //0x0018
	char pad_0020[8]; //0x0020
}; //Size: 0x0028
static_assert(sizeof(RopewayCameraControllerInfo) == 0x28);

class DotNetGenericDictionary : public REManagedObject
{
public:
	class REArrayBase *keys; //0x0010
	class REArrayBase *values; //0x0018
	char pad_0020[16]; //0x0020
	class REManagedObject *equalityComparer; //0x0030
}; //Size: 0x0038
static_assert(sizeof(DotNetGenericDictionary) == 0x38);

class N00001440 : public REManagedObject
{
public:
	char pad_0010[64]; //0x0010
}; //Size: 0x0050
static_assert(sizeof(N00001440) == 0x50);

class N0000144E : public REManagedObject
{
public:
	char pad_0010[64]; //0x0010
}; //Size: 0x0050
static_assert(sizeof(N0000144E) == 0x50);

class RopewayCameraInterpolationPrame : public REManagedObject
{
public:
	class REGameObject *owner; //0x0010
	class RopewayCameraPositionParam *posParam1; //0x0018
	class RopewayCameraPositionParam *posParam2; //0x0020
	class DampingFloat *dampingFloat; //0x0028
}; //Size: 0x0030
static_assert(sizeof(RopewayCameraInterpolationPrame) == 0x30);

class RopewayCameraPositionParam : public REManagedObject
{
public:
	class REGameObject *attachedOwner; //0x0010
	class REJoint *attachedJoint; //0x0018
	class RopewayCameraTransitionParam *transitionParam; //0x0020
}; //Size: 0x0028
static_assert(sizeof(RopewayCameraPositionParam) == 0x28);

class RopewayCameraTransitionParam : public REManagedObject
{
public:
	class RopewayPlayerCameraParam *playerCameraParam; //0x0010
	char pad_0018[136]; //0x0018
}; //Size: 0x00A0
static_assert(sizeof(RopewayCameraTransitionParam) == 0xA0);

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
static_assert(sizeof(RopewayPlayerCameraParam) == 0xA0);

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
static_assert(sizeof(RopewaySetPostEffectParam) == 0xB0);

class RopewayPostEffectController : public REBehavior
{
public:
	char pad_0048[8]; //0x0048
	class REAnimationCurve *curve1; //0x0050
	class REAnimationCurve *curve2; //0x0058
	class DotNetGenericList *N000015B6; //0x0060
	char pad_0068[24]; //0x0068
}; //Size: 0x0080
static_assert(sizeof(RopewayPostEffectController) == 0x80);

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
static_assert(sizeof(RESecondaryAnimation) == 0x48);

class ActorLayerList
{
public:
	class N00001630 *data; //0x0000
	uint32_t numLayers; //0x0008
	uint32_t numAllocated; //0x000C
}; //Size: 0x0010
static_assert(sizeof(ActorLayerList) == 0x10);

class REActorMotionCamera : public RESecondaryAnimation
{
public:
	class ActorLayerList layers; //0x0048
}; //Size: 0x0058
static_assert(sizeof(REActorMotionCamera) == 0x58);

class N00001630
{
public:
	class REActorLayer *layers[2048]; //0x0000
}; //Size: 0x4000
static_assert(sizeof(N00001630) == 0x4000);

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
static_assert(sizeof(RopewaySweetLightController) == 0x88);

class RERenderLight : public REComponent
{
public:
	Vector4f color; //0x0030
	char pad_0040[24]; //0x0040
	float brightness; //0x0058
	float bounceIntensity; //0x005C
	float tempature; //0x0060
	float minRoughness; //0x0064
	char pad_0068[4]; //0x0068
	uint32_t importantLevel; //0x006C
	char pad_0070[48]; //0x0070
	Vector3f activeColor; //0x00A0
	float N00001546; //0x00AC
	char pad_00B0[82]; //0x00B0
	bool forceShadowCacheEnable; //0x0102
	char pad_0103[1]; //0x0103
	bool blackBodyRadiation; //0x0104
	char pad_0105[107]; //0x0105
}; //Size: 0x0170
static_assert(sizeof(RERenderLight) == 0x170);

class RERenderSpotLight : public RERenderLight
{
public:
	char pad_0170[9]; //0x0170
	bool shadowEnable; //0x0179
	char pad_017A[22]; //0x017A
	int32_t shadowCastFlags; //0x0190
	char pad_0194[420]; //0x0194
	float N0000150F; //0x0338
	float N00001597; //0x033C
	float radius; //0x0340
	float illuminanceThreshold; //0x0344
	float cone; //0x0348
	float N0000159D; //0x034C
	float N00001512; //0x0350
	float shadowNearPlane; //0x0354
	float detailShadow; //0x0358
	bool N00002304; //0x035C
	char pad_035D[3]; //0x035D
	int32_t N00001514; //0x0360
	char pad_0364[28]; //0x0364
}; //Size: 0x0380
static_assert(sizeof(RERenderSpotLight) == 0x380);

class RERenderProjectionSpotlight : public RERenderSpotLight
{
public:
	char pad_0380[16]; //0x0380
}; //Size: 0x0390
static_assert(sizeof(RERenderProjectionSpotlight) == 0x390);

class RopewaySpotLightParam : public REManagedObject
{
public:
	char pad_0010[80]; //0x0010
	uint32_t N00002282; //0x0060
	char pad_0064[92]; //0x0064
	float N000022BD; //0x00C0
	float N000022DF; //0x00C4
	float N000022BE; //0x00C8
	float N000022E3; //0x00CC
	float N000022BF; //0x00D0
	float N000022E5; //0x00D4
	char pad_00D8[184]; //0x00D8
}; //Size: 0x0190
static_assert(sizeof(RopewaySpotLightParam) == 0x190);

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
static_assert(sizeof(RopewaySweetLightManager) == 0x78);

class RopewaySweetLightParam : public REManagedObject
{
public:
	class RopewaySpotLightParam *spotlight; //0x0010
}; //Size: 0x0018
static_assert(sizeof(RopewaySweetLightParam) == 0x18);

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
static_assert(sizeof(RopewaySweetLightManagerContext) == 0x48);

class REArrayBase : public REManagedObject
{
public:
	class REClassInfo *containedType; //0x0010
	int32_t num1; //0x0018
	int32_t numElements; //0x001C
}; //Size: 0x0020
static_assert(sizeof(REArrayBase) == 0x20);

class SweetLightContextContainer : public REArrayBase
{
public:
	class RopewaySweetLightManagerContext *data[2]; //0x0020
}; //Size: 0x0030
static_assert(sizeof(SweetLightContextContainer) == 0x30);

class REArrayThing : public REArrayBase
{
public:
	char pad_0020[1096]; //0x0020
}; //Size: 0x0468
static_assert(sizeof(REArrayThing) == 0x468);

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
static_assert(sizeof(RopewaySurvivorCharacterController) == 0xB8);

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
static_assert(sizeof(DampingVec3) == 0x78);

class PhysicsRequestSetCollider : public PhysicsCollidableBase
{
public:
	char pad_0050[56]; //0x0050
}; //Size: 0x0088
static_assert(sizeof(PhysicsRequestSetCollider) == 0x88);

class RopewaySurvivorCharacterControllerUserData : public UserData // what a NAME
{
public:
	class REPtrArray *data; //0x0030
	float N00001634; //0x0038
	float N0000165B; //0x003C
}; //Size: 0x0040
static_assert(sizeof(RopewaySurvivorCharacterControllerUserData) == 0x40);

class REPtrArray : public REArrayBase
{
public:
	class REManagedObject *data[2048]; //0x0020
}; //Size: 0x4020
static_assert(sizeof(REPtrArray) == 0x4020);

class SurvivorCharacterControllerUserDataParam : public REManagedObject
{
public:
	char pad_0010[8]; //0x0010
	class SystemString *string1; //0x0018
	char pad_0020[8]; //0x0020
	class SystemString *string2; //0x0028
	class SurvivorControllerUserDataShape *shape; //0x0030
}; //Size: 0x0038
static_assert(sizeof(SurvivorCharacterControllerUserDataParam) == 0x38);

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
static_assert(sizeof(SurvivorControllerUserDataShape) == 0x40);

class PhysicsColliderVector
{
public:
	class N000017C9 *data; //0x0000
	uint32_t numElements; //0x0008
	uint32_t numAllocated; //0x000C
}; //Size: 0x0010
static_assert(sizeof(PhysicsColliderVector) == 0x10);

class PhysicsCollidable : public REManagedObject
{
public:
	uint32_t N000016F4; //0x0010
	char pad_0014[4]; //0x0014
	class PhysicsCapsuleShape *shape1; //0x0018
	class PhysicsCapsuleShape *shape2; //0x0020
	char pad_0028[8]; //0x0028
	class PhysicsFilterInfo *filterInfo; //0x0030
	char pad_0038[8]; //0x0038
	class REGameObject *owner; //0x0040
	uint32_t N000016FB; //0x0048
	uint32_t N000017B7; //0x004C
	uint32_t N000016FC; //0x0050
	char pad_0054[4]; //0x0054
	class PhysicsColliderVector colliderControllers; //0x0058 interesting
	char pad_0068[16]; //0x0068
}; //Size: 0x0078
static_assert(sizeof(PhysicsCollidable) == 0x78);

class PhysicsCollider : public PhysicsCollidable
{
public:
	char pad_0078[288]; //0x0078
}; //Size: 0x0198
static_assert(sizeof(PhysicsCollider) == 0x198);

class PhysicsShape : public REManagedObject
{
public:
	char pad_0010[24]; //0x0010
	class PhysicsCollider *parentCollider; //0x0028
	char pad_0030[16]; //0x0030
	Vector4f vec1; //0x0040
	Vector4f vec2; //0x0050
}; //Size: 0x0060
static_assert(sizeof(PhysicsShape) == 0x60);

class PhysicsConvexShape : public PhysicsShape
{
public:
}; //Size: 0x0060
static_assert(sizeof(PhysicsConvexShape) == 0x60);

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
static_assert(sizeof(PhysicsCapsuleShape) == 0x90);

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
static_assert(sizeof(PhysicsFilterInfo) == 0x30);

class N000017C9
{
public:
	class REBehavior *element[2048]; //0x0000
}; //Size: 0x4000
static_assert(sizeof(N000017C9) == 0x4000);

class RopewaySurvivorTargetBankController : public REBehavior
{
public:
	char pad_0048[16]; //0x0048
	class REMotion *motion; //0x0058
	char pad_0060[32]; //0x0060
	class RopewaySurvivorPlayerCondition *playerCondition; //0x0080
}; //Size: 0x0088
static_assert(sizeof(RopewaySurvivorTargetBankController) == 0x88);

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
	bool isForceCaution; //0x006D
	bool isForceDanger; //0x006E
	char pad_006F[5]; //0x006F
	bool enableFPSCamera; //0x0074
	char pad_0075[7]; //0x0075
	bool useTranceiver; //0x007C
	char pad_007D[1]; //0x007D
	bool forceUseFlashlight; //0x007E
	bool manuallyLight; //0x007F
	float autoHealStartTimer; //0x0080
	float autoHealTimer; //0x0084
	float poisonTimer; //0x0088
	float poisonAutoRecoveryTimer; //0x008C
	float combatTimer; //0x0090
	float coughTimer; //0x0094
	float warmTimer; //0x0098
	float unknownfloat; //0x009C
	class RopewayTimer *lightSwitchTimer; //0x00A0
	class RopewayTimer *burnTimer; //0x00A8
	class SurvivorConditionTimerTrigger *wetTimerTrigger; //0x00B0
	class SurvivorConditionTimerTrigger *dryTimerTrigger; //0x00B8
	char pad_00C0[16]; //0x00C0
	class REGameObject *flashlight; //0x00D0
	char pad_00D8[88]; //0x00D8
	void *N0000828D; //0x0130
	void *N0000828E; //0x0138
	void *N0000828F; //0x0140
	void *N00008290; //0x0148
	void *N00008291; //0x0150
	bool exceptional; //0x0158
	char pad_0159[3]; //0x0159
	int32_t wallMaterial; //0x015C
	class REJoint *playerJoint; //0x0160
	char pad_0168[8]; //0x0168
	class SystemAction *actionVital; //0x0170
	class RopewaySurvivorPlayerController *playerController; //0x0178
	class RopewaySurvivorPlayerActionOrderer *actionOrderer; //0x0180
	char pad_0188[16]; //0x0188
	class RopewaySurvivorInventory *inventory; //0x0198
	class RopewaySurvivorEquipment *equipment; //0x01A0
	class REMotion *playerMotion; //0x01A8
	char pad_01B0[136]; //0x01B0
	class ActionTriggerInt *hitPointVitalTrigger; //0x0238
	class ActionTriggerInt *situationTrigger; //0x0240
	bool onFlashlight; //0x0248
	bool N00005FF6; //0x0249
	char pad_024A[134]; //0x024A
	bool isPoison; //0x02D0
	char pad_02D1[3]; //0x02D1
	float dopingTimer; //0x02D4
	char pad_02D8[64]; //0x02D8
}; //Size: 0x0318
static_assert(sizeof(RopewaySurvivorPlayerCondition) == 0x318);

class RopewayHandHeldItem : public REBehavior
{
public:
	char pad_0048[64]; //0x0048
}; //Size: 0x0088
static_assert(sizeof(RopewayHandHeldItem) == 0x88);

class RopewayFlashLight : public RopewayHandHeldItem
{
public:
	char pad_0088[168]; //0x0088
	Vector4f targetPosition; //0x0130
	char pad_0140[60]; //0x0140
}; //Size: 0x017C
static_assert(sizeof(RopewayFlashLight) == 0x17C);

class RopewayTimerBase : public REManagedObject
{
public:
	int32_t mode; //0x0010
	float timeLimit; //0x0014
}; //Size: 0x0018
static_assert(sizeof(RopewayTimerBase) == 0x18);

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
static_assert(sizeof(RopewayTimer) == 0x2C);

class RopewaySurvivorInventory : public REBehavior
{
public:
	char pad_0048[64]; //0x0048
	class RopewaySurvivorPlayerCondition *parentCondition; //0x0088
	char pad_0090[56]; //0x0090
}; //Size: 0x00C8
static_assert(sizeof(RopewaySurvivorInventory) == 0xC8);

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
static_assert(sizeof(RopewaySurvivorEquipment) == 0x128);

class RopewayArmList : public REManagedObject
{
public:
	class RopewayArmList_Impl *data; //0x0010
}; //Size: 0x0018
static_assert(sizeof(RopewayArmList) == 0x18);

class RopewayArmList_Impl : public REArrayBase
{
public:
	class RopewayImplementArm *arms[2048]; //0x0020
}; //Size: 0x4020
static_assert(sizeof(RopewayArmList_Impl) == 0x4020);

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
static_assert(sizeof(RopewayImplement) == 0xF0);

class RopewayImplementArm : public RopewayImplement
{
public:
	char pad_00F0[88]; //0x00F0
}; //Size: 0x0148
static_assert(sizeof(RopewayImplementArm) == 0x148);

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
static_assert(sizeof(RopewayCharacterHandler) == 0x180);

class RopewayPressController : public REBehavior
{
public:
	char pad_0048[88]; //0x0048
}; //Size: 0x00A0
static_assert(sizeof(RopewayPressController) == 0xA0);

class RopewayGroundFixer : public REBehavior
{
public:
	char pad_0048[280]; //0x0048
}; //Size: 0x0160
static_assert(sizeof(RopewayGroundFixer) == 0x160);

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
static_assert(sizeof(RopewayPlRainEffect) == 0xA0);

class REDynamicsComponentBase : public REComponent
{
public:
	class REDynamicsWorld *world; //0x0030
	char pad_0038[8]; //0x0038
}; //Size: 0x0040
static_assert(sizeof(REDynamicsComponentBase) == 0x40);

class REDynamicsRigidBodySet : public REDynamicsComponentBase
{
public:
	char pad_0040[144]; //0x0040
}; //Size: 0x00D0
static_assert(sizeof(REDynamicsRigidBodySet) == 0xD0);

class REDynamicsRagdoll : public REDynamicsRigidBodySet
{
public:
	char pad_00D0[32]; //0x00D0
	class REString name; //0x00F0
	char pad_0110[200]; //0x0110
}; //Size: 0x01D8
static_assert(sizeof(REDynamicsRagdoll) == 0x1D8);

class REDynamicsWorld : public REManagedObject
{
public:
	char pad_0010[152]; //0x0010
	class REScene *scene; //0x00A8
	char pad_00B0[4448]; //0x00B0
	class REDynamicsWorld *world; //0x1210
	char pad_1218[504]; //0x1218
}; //Size: 0x1410
static_assert(sizeof(REDynamicsWorld) == 0x1410);

class RERenderMesh : public REComponent
{
public:
	char pad_0030[736]; //0x0030
}; //Size: 0x0310
static_assert(sizeof(RERenderMesh) == 0x310);

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
static_assert(sizeof(RopewayExtraJoint) == 0x38);

class RopewayImplementVirtualJoint : public RopewayExtraJoint
{
public:
}; //Size: 0x0038
static_assert(sizeof(RopewayImplementVirtualJoint) == 0x38);

class RopewayMotionEventHandler : public REBehavior
{
public:
	char pad_0048[16]; //0x0048
	class REMotion *motion; //0x0058
	class REManagedObject *N00002175; //0x0060
	class REPtrArray *layers; //0x0068
}; //Size: 0x0070
static_assert(sizeof(RopewayMotionEventHandler) == 0x70);

class N000021BE : public REBehavior
{
public:
}; //Size: 0x0048
static_assert(sizeof(N000021BE) == 0x48);

class WwiseContainer : public N000021BE
{
public:
	char pad_0048[456]; //0x0048
}; //Size: 0x0210
static_assert(sizeof(WwiseContainer) == 0x210);

class RopewayWwiseContainerApp : public WwiseContainer
{
public:
	char pad_0210[32]; //0x0210
	class REPtrArray *autoTriggerReceivers; //0x0230
}; //Size: 0x0238
static_assert(sizeof(RopewayWwiseContainerApp) == 0x238);

class RopewayImplementGun : public RopewayImplementArm
{
public:
	char pad_0148[184]; //0x0148
}; //Size: 0x0200
static_assert(sizeof(RopewayImplementGun) == 0x200);

class RopewaySurvivorDefineDamageParam : public REManagedObject
{
public:
	int32_t damageType; //0x0010
	bool isDead; //0x0014
	char pad_0015[3]; //0x0015
}; //Size: 0x0018
static_assert(sizeof(RopewaySurvivorDefineDamageParam) == 0x18);

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
static_assert(sizeof(RopewaySurvivorActionOrderer) == 0xE0);

class RopewaySurvivorPlayerActionOrderer : public RopewaySurvivorActionOrderer
{
public:
	class RopewayTimer *timer1; //0x00E0
	class RopewayTimer *timer2; //0x00E8
	class RopewayTimer *timer3; //0x00F0
	char pad_00F8[8]; //0x00F8
	class RopewaySurvivorPlayerCondition *parentCondition; //0x0100
}; //Size: 0x0108
static_assert(sizeof(RopewaySurvivorPlayerActionOrderer) == 0x108);

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
static_assert(sizeof(RopewaySurvivorController) == 0xC8);

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
static_assert(sizeof(RopewaySurvivorPlayerController) == 0xF0);

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
static_assert(sizeof(RopewaySurvivorControllerStepCheck) == 0x60);

class DampingRangeFloat : public DampingFloat
{
public:
	float low; //0x0044
	float high; //0x0048
	bool isLoop; //0x004C
	char pad_004D[3]; //0x004D
	float divertPriority; //0x0050
}; //Size: 0x0054
static_assert(sizeof(DampingRangeFloat) == 0x54);

class N000024E5
{
public:
	float value; //0x0000
	int16_t N000024EA; //0x0004
	int16_t N000024F1; //0x0006
	char pad_0008[8]; //0x0008
}; //Size: 0x0010
static_assert(sizeof(N000024E5) == 0x10);

class CurveKeyFrames
{
public:
	class N000024E5 data[2048]; //0x0000
}; //Size: 0x8000
static_assert(sizeof(CurveKeyFrames) == 0x8000);

class N000024E0
{
public:
	Vector4f N000024E1; //0x0000
}; //Size: 0x0010
static_assert(sizeof(N000024E0) == 0x10);

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
static_assert(sizeof(RopewayInputSystem) == 0x170);

class RopewayInputSystemButtonMaskBit : public REManagedObject
{
public:
	int64_t bits; //0x0010
	bool reset; //0x0018
	char pad_0019[7]; //0x0019
}; //Size: 0x0020
static_assert(sizeof(RopewayInputSystemButtonMaskBit) == 0x20);

class InputSystemButton : public REManagedObject
{
public:
	char pad_0010[24]; //0x0010
}; //Size: 0x0028
static_assert(sizeof(InputSystemButton) == 0x28);

class HIDNativeDeviceBase : public REManagedObject
{
public:
	char pad_0010[120]; //0x0010
	bool connecting; //0x0088
	char pad_0089[7]; //0x0089
	class REString name; //0x0090
	char pad_00B0[80]; //0x00B0
	uint32_t button; //0x0100 via.hid.GamePadButton
	char pad_0104[4]; //0x0104
	uint32_t buttonDown; //0x0108
	uint32_t buttonUp; //0x010C
	uint32_t buttonRepeat; //0x0110
	char pad_0114[12]; //0x0114
}; //Size: 0x0120
static_assert(sizeof(HIDNativeDeviceBase) == 0x120);

class HIDGamePadDevice : public HIDNativeDeviceBase
{
public:
	char pad_0120[160]; //0x0120
	Vector2f rawAxisL; //0x01C0
	char pad_01C8[8]; //0x01C8
	Vector2f rawAxisR; //0x01D0
	char pad_01D8[8]; //0x01D8
	Vector2f axisL; //0x01E0
	char pad_01E8[8]; //0x01E8
	Vector2f axisR; //0x01F0
	char pad_01F8[8]; //0x01F8
	float analogL; //0x0200
	float analogR; //0x0204
	char pad_0208[24]; //0x0208
	Vector3f acceleration; //0x0220
	char pad_022C[4]; //0x022C
	Vector3f angularVelocity; //0x0230
	char pad_023C[116]; //0x023C
}; //Size: 0x02B0
static_assert(sizeof(HIDGamePadDevice) == 0x2B0);

class HIDJoypadDevice : public HIDGamePadDevice
{
public:
	char pad_02B0[16]; //0x02B0
}; //Size: 0x02C0
static_assert(sizeof(HIDJoypadDevice) == 0x2C0);

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
static_assert(sizeof(RopewayInputSystemAnalogStick) == 0x80);

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
static_assert(sizeof(RopewaySurvivorManager) == 0x100);

class RopewaySurvivorCastingRequest : public REManagedObject
{
public:
	char pad_0010[24]; //0x0010
}; //Size: 0x0028
static_assert(sizeof(RopewaySurvivorCastingRequest) == 0x28);

class SystemDelegate : public REManagedObject
{
public:
}; //Size: 0x0010
static_assert(sizeof(SystemDelegate) == 0x10);

class DelegateElement
{
public:
	class REManagedObject *obj; //0x0000
	void* funcPtr; //0x0008
}; //Size: 0x0010
static_assert(sizeof(DelegateElement) == 0x10);

class SystemMulticastDelegate : public SystemDelegate
{
public:
	int32_t numElements; //0x0010
	int32_t N00005A07; //0x0014
	class DelegateElement delegates[2048]; //0x0018
}; //Size: 0x8018
static_assert(sizeof(SystemMulticastDelegate) == 0x8018);

class SystemAction : public SystemMulticastDelegate
{
public:
}; //Size: 0x8018
static_assert(sizeof(SystemAction) == 0x8018);

class RopewaySettingFolder : public REManagedObject
{
public:
	class REFolder *folder; //0x0010
	class SystemString *name; //0x0018
}; //Size: 0x0020
static_assert(sizeof(RopewaySettingFolder) == 0x20);

class SurvivorConditionTimerTrigger : public REManagedObject
{
public:
	float old; //0x0010
	float current; //0x0014
	float threshold; //0x0018
}; //Size: 0x001C
static_assert(sizeof(SurvivorConditionTimerTrigger) == 0x1C);

class RERangeI
{
public:
	int32_t min; //0x0000
	int32_t max; //0x0004
}; //Size: 0x0008
static_assert(sizeof(RERangeI) == 0x8);

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
static_assert(sizeof(RopewayEnemyController) == 0x2E8);

class RopewayEnemyContextController : public REBehavior
{
public:
	char pad_0048[12]; //0x0048
	int32_t initialKindId; //0x0054
	char pad_0058[16]; //0x0058
}; //Size: 0x0068
static_assert(sizeof(RopewayEnemyContextController) == 0x68);

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
static_assert(sizeof(RopewayEnemyEmCommonContext) == 0xA8);

class SystemGuid
{
public:
	char pad_0000[16]; //0x0000
}; //Size: 0x0010
static_assert(sizeof(SystemGuid) == 0x10);

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
static_assert(sizeof(RopewayEnemyEmSaveData) == 0x80);

class RopewayBitFlag : public REManagedObject
{
public:
	int32_t flag; //0x0010
	char pad_0014[4]; //0x0014
}; //Size: 0x0018
static_assert(sizeof(RopewayBitFlag) == 0x18);

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
static_assert(sizeof(CollisionSystemAsyncCastHandleBase) == 0x80);

class CollisionSystemAsyncCastRayHandle : public CollisionSystemAsyncCastHandleBase
{
public:
}; //Size: 0x0080
static_assert(sizeof(CollisionSystemAsyncCastRayHandle) == 0x80);

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
static_assert(sizeof(RopewayEnemyManager) == 0x180);

class RopewayEnemyLODSettingUserData : public UserData
{
public:
	float distanceLevels[5]; //0x0030
}; //Size: 0x0044
static_assert(sizeof(RopewayEnemyLODSettingUserData) == 0x44);

class RopewayEnemyManagerFrameTimer : public REManagedObject
{
public:
	bool run; //0x0010
	char pad_0011[7]; //0x0011
	uint64_t delayFrame; //0x0018
	uint64_t endFrame; //0x0020
	uint64_t frameCount; //0x0028
}; //Size: 0x0030
static_assert(sizeof(RopewayEnemyManagerFrameTimer) == 0x30);

class RENativeArray // via.array
{
public:
	class RENativeArrayData *data; //0x0000
	int32_t size; //0x0008
	int32_t numAllocated; //0x000C
}; //Size: 0x0010
static_assert(sizeof(RENativeArray) == 0x10);

class RENativeArrayData
{
public:
	char pad_0000[8]; //0x0000
}; //Size: 0x0008
static_assert(sizeof(RENativeArrayData) == 0x8);

class RopewayCameraStatus : public REManagedObject
{
public:
	char pad_0010[384]; //0x0010
}; //Size: 0x0190
static_assert(sizeof(RopewayCameraStatus) == 0x190);

class RopewayCameraInterpolateSettings : public UserData
{
public:
	void *curveParamTable; //0x0030
}; //Size: 0x0038
static_assert(sizeof(RopewayCameraInterpolateSettings) == 0x38);

class CollisionSystemAsyncCastSphereHandle : public CollisionSystemAsyncCastHandleBase
{
public:
	class RESphere *sphere; //0x0080
}; //Size: 0x0088
static_assert(sizeof(CollisionSystemAsyncCastSphereHandle) == 0x88);

class RESphere
{
public:
	char pad_0000[8]; //0x0000
}; //Size: 0x0008
static_assert(sizeof(RESphere) == 0x8);

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
static_assert(sizeof(RERenderCaptureToTexture) == 0x90);

class AsyncCastHandleResultData : public REManagedObject
{
public:
	char pad_0010[32]; //0x0010
	Vector4f startPos; //0x0030
	Vector4f endPos; //0x0040
	char pad_0050[16]; //0x0050
}; //Size: 0x0060
static_assert(sizeof(AsyncCastHandleResultData) == 0x60);

class ValueTriggerInt : public REManagedObject
{
public:
	int32_t current; //0x0010
	int32_t old; //0x0014
	int32_t lastFrame; //0x0018
}; //Size: 0x001C
static_assert(sizeof(ValueTriggerInt) == 0x1C);

class ActionTriggerInt : public ValueTriggerInt
{
public:
	char pad_001C[4]; //0x001C
	class SystemAction *action; //0x0020
}; //Size: 0x0028
static_assert(sizeof(ActionTriggerInt) == 0x28);

class IkControlStatusArray : public REArrayBase
{
public:
	class RopewayIkControlStatus *N00004982[6]; //0x0020
}; //Size: 0x0050
static_assert(sizeof(IkControlStatusArray) == 0x50);

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
static_assert(sizeof(RopewayIkControlStatus) == 0x80);

class RopewayIkWrapper : public REManagedObject
{
public:
	bool setupped; //0x0010
	char pad_0011[7]; //0x0011
	class REMotionIkLeg *N00004B67; //0x0018
	char pad_0020[88]; //0x0020
}; //Size: 0x0078
static_assert(sizeof(RopewayIkWrapper) == 0x78);

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
static_assert(sizeof(REMotionIkLeg) == 0xA90);

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
static_assert(sizeof(RopewayRangeLerpFloat) == 0x30);

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
static_assert(sizeof(RopewayEquipmentManager) == 0x90);

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
static_assert(sizeof(RopewayWeaponBulletUserData) == 0x48);

class RopewayWeaponEquippedPositionUserData : public UserData
{
public:
	char pad_0030[8]; //0x0030
}; //Size: 0x0038
static_assert(sizeof(RopewayWeaponEquippedPositionUserData) == 0x38);

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
static_assert(sizeof(RopewayGameMaster) == 0x78);

class ManagedEnumValuesPtr
{
public:
	char *(*names)[8192]; //0x0000
}; //Size: 0x0008
static_assert(sizeof(ManagedEnumValuesPtr) == 0x8);

class REEnumMap
{
public:
	class REEnumNode *first; //0x0000
	int32_t num; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
static_assert(sizeof(REEnumMap) == 0x10);

class ManagedEnums : public REEnumMap
{
public:
	class RENativeArray enumValues; //0x0010
}; //Size: 0x0020
static_assert(sizeof(ManagedEnums) == 0x20);

class N00006DF9
{
public:
	char pad_0000[16]; //0x0000
	class RENativeArray enumValues; //0x0010
	char pad_0020[304]; //0x0020
}; //Size: 0x0150
static_assert(sizeof(N00006DF9) == 0x150);

class N00006EE3
{
public:
	class REEnumDescriptor *N00006EE4; //0x0000
	class N00006F33 *N00006EE5; //0x0008
	class N00006F56 *N00006EE6; //0x0010
	char pad_0018[8]; //0x0018
}; //Size: 0x0020
static_assert(sizeof(N00006EE3) == 0x20);

class REEnumNode
{
public:
	class REEnumDescriptor *N00007017; //0x0000
	class REEnumDescriptor *N0000702A; //0x0008
	class REEnumDescriptor *N0000702B; //0x0010
	char pad_0018[8]; //0x0018
}; //Size: 0x0020
static_assert(sizeof(REEnumNode) == 0x20);

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
static_assert(sizeof(REEnumData) == 0x38);

class REEnumPair
{
public:
	uint32_t first; //0x0000
	char pad_0004[4]; //0x0004
	class REEnumData second; //0x0008
}; //Size: 0x0040
static_assert(sizeof(REEnumPair) == 0x40);

class REEnumDescriptor : public REEnumNode
{
public:
	class REEnumPair data; //0x0020
	char pad_0060[332]; //0x0060
}; //Size: 0x01AC
static_assert(sizeof(REEnumDescriptor) == 0x1AC);

class N00006F33
{
public:
	char pad_0000[8]; //0x0000
}; //Size: 0x0008
static_assert(sizeof(N00006F33) == 0x8);

class N00006F56
{
public:
	char pad_0000[8]; //0x0000
}; //Size: 0x0008
static_assert(sizeof(N00006F56) == 0x8);

class REEnumValueNode
{
public:
	char *name; //0x0000
	int64_t value; //0x0008
	class REEnumValueNode *next; //0x0010
}; //Size: 0x0018
static_assert(sizeof(REEnumValueNode) == 0x18);

class StdMap
{
public:
	class StdMapNode *_MyHead; //0x0000
	int32_t _MySize; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
static_assert(sizeof(StdMap) == 0x10);

class StdMapData
{
public:
	char pad_0000[120]; //0x0000
}; //Size: 0x0078
static_assert(sizeof(StdMapData) == 0x78);

class StdMapNode
{
public:
	class StdMapNode *N00008173; //0x0000
	class StdMapNode *N00008174; //0x0008
	class StdMapNode *N00008175; //0x0010
	char pad_0018[8]; //0x0018
	class StdMapData data; //0x0020
}; //Size: 0x0098
static_assert(sizeof(StdMapNode) == 0x98);

class TypeList // RENativeArray
{
public:
	class REType *(*data)[50000]; //0x0000
	int32_t size; //0x0008
	int32_t numAllocated; //0x000C
	char pad_0010[120]; //0x0010
}; //Size: 0x0088
static_assert(sizeof(TypeList) == 0x88);

class MethodParamInfo
{
public:
	char pad_0000[8]; //0x0000
	char *paramName; //0x0008
	uint32_t paramTypeFlag; //0x0010 AND 1f
	uint32_t typeIndex; //0x0014
	char *typeName; //0x0018
}; //Size: 0x0020
static_assert(sizeof(MethodParamInfo) == 0x20);

class RopewayScriptEffectManager : public REBehavior
{
public:
	char pad_0048[9]; //0x0048
	bool isDisableAllEffect; //0x0051
	char pad_0052[182]; //0x0052
}; //Size: 0x0108
static_assert(sizeof(RopewayScriptEffectManager) == 0x108);

class RERenderDepthOfField : public REComponent
{
public:
	char pad_0030[24]; //0x0030
	bool enabled; //0x0048
	char pad_0049[15]; //0x0049
}; //Size: 0x0058
static_assert(sizeof(RERenderDepthOfField) == 0x58);

class RopewayPostEffectControllerBase : public REBehavior
{
public:
	char pad_0048[32]; //0x0048
	class REComponent *effectComponent; //0x0068
	class RopewayPostEffectFilterParamBase *param1; //0x0070
	class RopewayPostEffectFilterParamBase *param2; //0x0078
	class REGameObject *cameraObject; //0x0080
	char pad_0088[8]; //0x0088
	class RopewayPostEffectCascade *cascade; //0x0090
	class REGameObject *cameraObject2; //0x0098
	char pad_00A0[8]; //0x00A0
	class RopewayPostEffectFilterParamBase *param3; //0x00A8
	class RopewayFilterSettingBase *filterSetting; //0x00B0
	class RopewayPostEffectFilterParamBase *param4; //0x00B8
	char pad_00C0[24]; //0x00C0
}; //Size: 0x00D8
static_assert(sizeof(RopewayPostEffectControllerBase) == 0xD8);

class RopewayFilterSettingBase : public REBehavior
{
public:
	char pad_0048[16]; //0x0048
	class RopewayPostEffectFilterParamBase *param; //0x0058
	float interpolationSpan; //0x0060
	int32_t layerApplyType; //0x0064 app::ropeway::posteffect::setting::FilterSettingRoot::ApplyType
	class RopewayPostEffectFilterParamBase *currentParam; //0x0068
	class RopewayPostEffectFilterParamBase *param1; //0x0070
	class RopewayPostEffectFilterParamBase *param2; //0x0078
	char pad_0080[8]; //0x0080
}; //Size: 0x0088
static_assert(sizeof(RopewayFilterSettingBase) == 0x88);

class RopewayPostEffectFilterParamBase : public REManagedObject
{
public:
	char pad_0010[32]; //0x0010
}; //Size: 0x0030
static_assert(sizeof(RopewayPostEffectFilterParamBase) == 0x30);

class RopewayPostEffectCascade : public REBehavior
{
public:
	char pad_0048[48]; //0x0048
}; //Size: 0x0078
static_assert(sizeof(RopewayPostEffectCascade) == 0x78);

class RopewayFilterParamAccessable : public RopewayPostEffectFilterParamBase
{
public:
	char pad_0030[16]; //0x0030
	bool timelineOverwrite; //0x0040
	char pad_0041[11]; //0x0041
	float timelineBlendRate; //0x004C
}; //Size: 0x0050
static_assert(sizeof(RopewayFilterParamAccessable) == 0x50);

class RopewayPostEffectToneMapping : public RopewayFilterParamAccessable
{
public:
	bool enabled; //0x0050
	char pad_0051[3]; //0x0051
	float ev; //0x0054
	float maxWhitePoint; //0x0058
	float minWhitePoint; //0x005C
	float whiteRange; //0x0060
	float tonemapRange; //0x0064
	float preTonemapRange; //0x0068
	float brightAdaptationRate; //0x006C
	float darkAdaptationRate; //0x0070
	int32_t vignetting; //0x0074 via::render::ToneMapping::Vignetting
	int32_t autoExposure; //0x0078 via::render::ToneMapping::AutoExposure
	float kerareBeginAngle; //0x007C kerare = vignetting in japanese
	float linearSectionBegin; //0x0080
	float linearSectionLength; //0x0084
	float sdrToe; //0x0088
	float hdrToe; //0x008C
	float kerareEndAngle; //0x0090
	char pad_0094[4]; //0x0094
	void *textureResource; //0x0098
	char pad_00A0[8]; //0x00A0
	int32_t temporalAAAlgorithm; //0x00A8 via::render::ToneMapping::TemporalAAAlgorithm
	int32_t temporalAA; //0x00AC via::render::ToneMapping::TemporalAA
	bool echoEnabled; //0x00B0
	char pad_00B1[3]; //0x00B1
	float antiAliasingBlendRate; //0x00B4
	float temporalJitterScale; //0x00B8
	float sharpness; //0x00BC
}; //Size: 0x00C0
static_assert(sizeof(RopewayPostEffectToneMapping) == 0xC0);

class REInstantiationDescriptor
{
public:
	class REObjectInfo *type; //0x0000
	uint32_t N00008AC9; //0x0008
	uint32_t N00008B38; //0x000C
	uint32_t size; //0x0010
	uint32_t N00008B3A; //0x0014
	char pad_0018[40]; //0x0018
}; //Size: 0x0040
static_assert(sizeof(REInstantiationDescriptor) == 0x40);

class MethodParams
{
public:
	char pad_0000[48]; //0x0000
	void ***in_data; //0x0030 can point to data
	void **out_data; //0x0038 can be whatever, can be a dword, can point to data
	void *object_ptr; //0x0040 aka "this" pointer
}; //Size: 0x0048
static_assert(sizeof(MethodParams) == 0x48);

class REThreadContext
{
public:
	char pad_0000[80]; //0x0000
	class UnkThreadContextObject *unkPtr; //0x0050
	char pad_0058[32]; //0x0058
	int32_t referenceCount; //0x0078
	char pad_007C[4]; //0x007C
}; //Size: 0x0080
static_assert(sizeof(REThreadContext) == 0x80);

class UnkThreadContextObject
{
public:
	char pad_0000[24]; //0x0000
	void *unkPtr; //0x0018
	char pad_0020[240]; //0x0020
}; //Size: 0x0110
static_assert(sizeof(UnkThreadContextObject) == 0x110);

class N0000256E
{
public:
	char pad_0000[264]; //0x0000
}; //Size: 0x0108
static_assert(sizeof(N0000256E) == 0x108);

class TrialManager : public REManagedObject
{
public:
	char pad_0010[24]; //0x0010
	class N00002CDC *N00002C19; //0x0028
	char pad_0030[56]; //0x0030
	class N00002C87 *N00002C24; //0x0068
	char pad_0070[216]; //0x0070
}; //Size: 0x0148
static_assert(sizeof(TrialManager) == 0x148);

class N00002C87 : public REManagedObject
{
public:
	char pad_0010[64]; //0x0010
}; //Size: 0x0050
static_assert(sizeof(N00002C87) == 0x50);

class N00002CDC : public REManagedObject
{
public:
	char pad_0010[112]; //0x0010
}; //Size: 0x0080
static_assert(sizeof(N00002CDC) == 0x80);

class N00003CA1 : public REManagedObject
{
public:
	char pad_0010[16]; //0x0010
}; //Size: 0x0020
static_assert(sizeof(N00003CA1) == 0x20);

class N00003CB3 : public REManagedObject
{
public:
	char pad_0010[360]; //0x0010
	class N00003D23 *N00003CE5; //0x0178
	char pad_0180[200]; //0x0180
}; //Size: 0x0248
static_assert(sizeof(N00003CB3) == 0x248);

class N00003D23 : public REManagedObject
{
public:
	char pad_0010[16]; //0x0010
	int32_t N00003D2A; //0x0020
	char pad_0024[4]; //0x0024
	uint32_t N00003D2B; //0x0028
	char pad_002C[4]; //0x002C
}; //Size: 0x0030
static_assert(sizeof(N00003D23) == 0x30);

class N00003D34 : public REManagedObject
{
public:
	char pad_0010[8]; //0x0010
	int32_t N00003D37; //0x0018
	char pad_001C[4]; //0x001C
	bool enabled; //0x0020
	char pad_0021[7]; //0x0021
}; //Size: 0x0028
static_assert(sizeof(N00003D34) == 0x28);

class CountdownTimerController : public REManagedObject
{
public:
	float N00003D4A; //0x0010
	float N00003D55; //0x0014
	float N00003D4B; //0x0018
	float N00003D57; //0x001C
	char pad_0020[8]; //0x0020
}; //Size: 0x0028
static_assert(sizeof(CountdownTimerController) == 0x28);

class SceneActivateManager : public REBehavior
{
public:
	char pad_0048[72]; //0x0048
}; //Size: 0x0090
static_assert(sizeof(SceneActivateManager) == 0x90);

class RenderSwingWind : public REComponent
{
public:
	bool Enabled; //0x0030
	char pad_0031[7]; //0x0031
	class N00009C0D *WindParameter; //0x0038
}; //Size: 0x0040
static_assert(sizeof(RenderSwingWind) == 0x40);

class N00009C0D
{
public:
	char pad_0000[264]; //0x0000
}; //Size: 0x0108
static_assert(sizeof(N00009C0D) == 0x108);

class N00009D2E
{
public:
	char pad_0000[112]; //0x0000
}; //Size: 0x0070
static_assert(sizeof(N00009D2E) == 0x70);

class REArraySystemInt32 : public REArrayBase
{
public:
	class N00009D2E N00009D02[256]; //0x0020
}; //Size: 0x7020
static_assert(sizeof(REArraySystemInt32) == 0x7020);

class DotNetGenericHashSet : public REManagedObject
{
public:
	char pad_0010[16]; //0x0010
	class REManagedObject *equalityComparer; //0x0020
}; //Size: 0x0028
static_assert(sizeof(DotNetGenericHashSet) == 0x28);

class AppPropsManager : public REBehavior
{
public:
	char pad_0048[16]; //0x0048
	class REGameObject *player; //0x0058
	class RECamera *camera; //0x0060
	class AppFlotageProcess *flotageProcess; //0x0068
	class AppSwingRopeProcess *swingRopeProcess; //0x0070
	char pad_0078[8]; //0x0078
}; //Size: 0x0080
static_assert(sizeof(AppPropsManager) == 0x80);

class AppObjectApp : public REManagedObject
{
public:
}; //Size: 0x0010
static_assert(sizeof(AppObjectApp) == 0x10);

class AppPropsManagerToyProcess : public AppObjectApp
{
public:
	bool isDebugDraw; //0x0010
	char pad_0011[15]; //0x0011
	class DotNetGenericList *behaviorList; //0x0020
}; //Size: 0x0028
static_assert(sizeof(AppPropsManagerToyProcess) == 0x28);

class AppFlotageProcess : public AppPropsManagerToyProcess
{
public:
	int32_t flotageId; //0x0028
	int32_t turnCounter; //0x002C
	float maxDistance; //0x0030
	float N0000A33D; //0x0034
}; //Size: 0x0038
static_assert(sizeof(AppFlotageProcess) == 0x38);

class AppSwingRopeProcess : public AppPropsManagerToyProcess
{
public:
}; //Size: 0x0028
static_assert(sizeof(AppSwingRopeProcess) == 0x28);

class AppBehaviorApp : public REBehavior
{
public:
	char pad_0048[8]; //0x0048
}; //Size: 0x0050
static_assert(sizeof(AppBehaviorApp) == 0x50);

class AppPlayerHandLight : public AppBehaviorApp
{
public:
	char pad_0050[1]; //0x0050
	bool IsContinuousOn; //0x0051
	char pad_0052[6]; //0x0052
	class AppHandLightPowerController *handLightPowerController; //0x0058
	char pad_0060[16]; //0x0060
	void *nightVision; //0x0070
	char pad_0078[8]; //0x0078
	void *playerStatus; //0x0080
	void *wwise; //0x0088
}; //Size: 0x0090
static_assert(sizeof(AppPlayerHandLight) == 0x90);

class AppCharacterUpdater : public AppBehaviorApp
{
public:
	void *referenceContainer; //0x0050
	void *agentsPriorityContainer; //0x0058
}; //Size: 0x0060
static_assert(sizeof(AppCharacterUpdater) == 0x60);

class AppPlayerUpdater : public AppCharacterUpdater
{
public:
	void *playerConfigure; //0x0060
	class AppPlayerReferenceContainer *playerReferenceContainer; //0x0068
	char pad_0070[8]; //0x0070
}; //Size: 0x0078
static_assert(sizeof(AppPlayerUpdater) == 0x78);

class N0000B498 : public REManagedObject
{
public:
}; //Size: 0x0010
static_assert(sizeof(N0000B498) == 0x10);

class AppCharacterReferenceContainer : public N0000B498
{
public:
	void *N0000B49D; //0x0010
	void *N0000B49E; //0x0018
	void *N0000B49F; //0x0020
	void *N0000B4A0; //0x0028
	void *N0000B4A1; //0x0030
	void *N0000B4A2; //0x0038
	void *N0000B4A3; //0x0040
	class AppInputInputUpdater *inputUpdater; //0x0048
	char pad_0050[200]; //0x0050
}; //Size: 0x0118
static_assert(sizeof(AppCharacterReferenceContainer) == 0x118);

class AppPlayerReferenceContainer : public AppCharacterReferenceContainer
{
public:
	char pad_0118[232]; //0x0118
}; //Size: 0x0200
static_assert(sizeof(AppPlayerReferenceContainer) == 0x200);

class AppDemiAgent : public AppObjectApp
{
public:
	class REGameObject *gameObject; //0x0010
}; //Size: 0x0018
static_assert(sizeof(AppDemiAgent) == 0x18);

class AppAgent : public AppDemiAgent
{
public:
	class AppAgentCostWatch *costWatch1; //0x0018
	class AppAgentCostWatch *costWatch2; //0x0020
}; //Size: 0x0028
static_assert(sizeof(AppAgent) == 0x28);

class AppInputInputUpdater : public AppAgent
{
public:
	char pad_0028[8]; //0x0028
	class DotNetGenericDictionary *inputDictionary; //0x0030
	char pad_0038[72]; //0x0038
}; //Size: 0x0080
static_assert(sizeof(AppInputInputUpdater) == 0x80);

class AppAgentCostWatch : public REManagedObject
{
public:
	float N0000B612; //0x0010
	bool isWait; //0x0014
	char pad_0015[3]; //0x0015
	class SystemDiagnosticsStopwatch *stopwatch; //0x0018
	char pad_0020[4]; //0x0020
	float averageCost; //0x0024
	float minUpdateCost; //0x0028
	float maxUpdateCost; //0x002C
	char pad_0030[8]; //0x0030
}; //Size: 0x0038
static_assert(sizeof(AppAgentCostWatch) == 0x38);

class SystemDiagnosticsStopwatch : public REManagedObject
{
public:
	char pad_0010[24]; //0x0010
}; //Size: 0x0028
static_assert(sizeof(SystemDiagnosticsStopwatch) == 0x28);

class AppHandLightPowerController
{
public:
	char pad_0000[120]; //0x0000
	class IESLight *renderIESLight; //0x0078
}; //Size: 0x0080
static_assert(sizeof(AppHandLightPowerController) == 0x80);

class IESLight
{
public:
	char pad_0000[417]; //0x0000
	bool ShadowEnable; //0x01A1
	char pad_01A2[414]; //0x01A2
	float Radius; //0x0340
}; //Size: 0x0344
static_assert(sizeof(IESLight) == 0x344);

class AppGamePadDevice
{
public:
	char pad_0000[560]; //0x0000
	Vector3f N0000BCA8; //0x0230
	char pad_023C[1548]; //0x023C
}; //Size: 0x0848
static_assert(sizeof(AppGamePadDevice) == 0x848);

class ValueTypeDescriptor
{
public:
	char pad_0000[8]; //0x0000
	uint32_t size; //0x0008
	char pad_000C[36]; //0x000C
}; //Size: 0x0030
static_assert(sizeof(ValueTypeDescriptor) == 0x30);

class REValueTypes
{
public:
	class ValueTypeDescriptor types[10000]; //0x0000
}; //Size: 0x75300
static_assert(sizeof(REValueTypes) == 0x75300);

class DictionaryEntryUInt32Ptr
{
public:
	int32_t hashCode; //0x0000
	int32_t next; //0x0004
	uint32_t key; //0x0008
	char pad_000C[4]; //0x000C
	void *value; //0x0010
}; //Size: 0x0018
static_assert(sizeof(DictionaryEntryUInt32Ptr) == 0x18);

class SystemClassDescriptor
{
public:
	char *name_space; //0x0000
	char *class_name; //0x0008
	uint32_t num_methods; //0x0010
	char pad_0014[4]; //0x0014
	class SystemMethodDescriptor (*methods)[256]; //0x0018
}; //Size: 0x0020
static_assert(sizeof(SystemClassDescriptor) == 0x20);

class SystemClasses
{
public:
	class SystemClassDescriptor N0000CB08[256]; //0x0000
}; //Size: 0x2000
static_assert(sizeof(SystemClasses) == 0x2000);

class SystemMethodDescriptor
{
public:
	char *name; //0x0000
	void* func; //0x0008
	char pad_0010[64]; //0x0010
}; //Size: 0x0050
static_assert(sizeof(SystemMethodDescriptor) == 0x50);

class N0000CBBC
{
public:
	class REClassInfo *class_infos[256]; //0x0000
}; //Size: 0x0800
static_assert(sizeof(N0000CBBC) == 0x800);

class AppHIDPadManager : public REBehavior
{
public:
	char pad_0048[16]; //0x0048
	void *userdata; //0x0058
	class AppHIDPad *pad1; //0x0060
	class N0000CD1C *pad2; //0x0068
	void *delegate1; //0x0070
	void *delegate2; //0x0078
	class DotNetGenericList *pads; //0x0080
	class DotNetGenericList *padDevices; //0x0088
}; //Size: 0x0090
static_assert(sizeof(AppHIDPadManager) == 0x90);

class AppHIDPad : public AppObjectApp
{
public:
	void *delegate1; //0x0010
	class HIDGamePadDevice *device; //0x0018
	char pad_0020[16]; //0x0020
	class REArrayBase *array1; //0x0030
	class REArrayBase *array2; //0x0038
	char pad_0040[96]; //0x0040
	class DotNetGenericDictionary *dictionary1; //0x00A0
	char pad_00A8[8]; //0x00A8
}; //Size: 0x00B0
static_assert(sizeof(AppHIDPad) == 0xB0);

class N0000CD1C
{
public:
	char pad_0000[8]; //0x0000
}; //Size: 0x0008
static_assert(sizeof(N0000CD1C) == 0x8);

class AppHIDUser : public REManagedObject
{
public:
	int32_t N0000D337; //0x0010
	int32_t N0000D340; //0x0014
	char pad_0018[4]; //0x0018
}; //Size: 0x001C
static_assert(sizeof(AppHIDUser) == 0x1C);

class AppMouse : public AppHIDUser
{
public:
	float x; //0x001C
	float y; //0x0020
}; //Size: 0x0024
static_assert(sizeof(AppMouse) == 0x24);

class AppKeyboard : public AppHIDUser
{
public:
}; //Size: 0x001C
static_assert(sizeof(AppKeyboard) == 0x1C);

class RETypeDB
{
public:
	uint32_t magic; //0x0000
	uint32_t version; //0x0004
	uint32_t initialized; //0x0008
	uint32_t numTypes; //0x000C
	uint32_t numMethods; //0x0010
	uint32_t numFields; //0x0014
	uint32_t numTypeImpl; //0x0018
	uint32_t numFieldImpl; //0x001C
	uint32_t numMethodImpl; //0x0020
	uint32_t numPropertyImpl; //0x0024
	uint32_t numProperties; //0x0028
	uint32_t numEvents; //0x002C
	uint32_t numParams; //0x0030
	uint32_t numAttributes; //0x0034
	int32_t numInitData; //0x0038
	uint32_t numAttributes2; //0x003C
	uint32_t numInternStrings; //0x0040
	uint32_t numModules; //0x0044
	int32_t devEntry; //0x0048
	int32_t appEntry; //0x004C
	uint32_t numStringPool; //0x0050
	uint32_t numBytePool; //0x0054
	void *modules; //0x0058
	class REClassInfo (*types)[93788]; //0x0060
	class RETypeImpl (*typesImpl)[256]; //0x0068
	class REMethodDefinition (*methods)[703558]; //0x0070
	class REMethodImpl (*methodsImpl)[56756]; //0x0078
	class REField (*fields)[1]; //0x0080
	class REFieldImpl (*fieldsImpl)[1]; //0x0088
	class REProperty (*properties)[256]; //0x0090
	class REPropertyImpl (*propertiesImpl)[1]; //0x0098
	void *events; //0x00A0
	class REParameterDef (*params)[10000]; //0x00A8
	class REAttributeDef (*attributes)[2000]; //0x00B0
	int32_t (*initData)[19890]; //0x00B8
	int32_t (*attributes2)[256]; //0x00C0
	void *stringPool; //0x00C8
	uint8_t (*bytePool)[256]; //0x00D0
	int32_t (*internStrings)[14154]; //0x00D8
}; //Size: 0x00E0
static_assert(sizeof(RETypeDB) == 0xE0);

class RETypeImpl
{
public:
	int32_t nameThing; //0x0000
	int32_t namespaceThing; //0x0004
	int32_t fieldSize; //0x0008
	int32_t staticFieldSize; //0x000C
	uint8_t moduleId; //0x0010
	uint8_t arrayRank; //0x0011
	uint16_t numMemberMethods; //0x0012
	int32_t numMemberFields; //0x0014
	int16_t interfaceID; //0x0018
	uint16_t numNativeVtable; //0x001A
	uint16_t attributesID; //0x001C
	uint16_t numVtable; //0x001E
	uint64_t mark; //0x0020
	uint64_t cycle; //0x0028
}; //Size: 0x0030
static_assert(sizeof(RETypeImpl) == 0x30);

class REMethodDefinition
{
public:
	uint64_t data; //0x0000 bitfield, 0-17 == declaring typeid, 18-37==impl id, 38-63 == parameters id
	void* function; //0x0008
}; //Size: 0x0010
static_assert(sizeof(REMethodDefinition) == 0x10);

class REMethodImpl
{
public:
	uint16_t attributesIndex; //0x0000
	int16_t vtableIndex; //0x0002
	uint16_t flags; //0x0004
	uint16_t implFlags; //0x0006
	uint32_t nameOffset; //0x0008 offset into stringPool
}; //Size: 0x000C
static_assert(sizeof(REMethodImpl) == 0xC);

class REField
{
public:
	uint64_t data; //0x0000 bitfield, 0-17 == declaring typeid, 18-37==impl id, 38-63 == field offset
}; //Size: 0x0008
static_assert(sizeof(REField) == 0x8);

class REFieldImpl
{
public:
	uint16_t attributesIndex; //0x0000
	uint16_t flags; //0x0002
	uint32_t bitfield1; //0x0004 0-17 == field type ID
	uint32_t bitfield2; //0x0008 0-29 == string pool name index, 30-31 == idk
}; //Size: 0x000C
static_assert(sizeof(REFieldImpl) == 0xC);

class REProperty
{
public:
	uint64_t data; //0x0000 0-19 = impl index, 20-41 == getter index, 42-63 == setter index
}; //Size: 0x0008
static_assert(sizeof(REProperty) == 0x8);

class REPropertyImpl
{
public:
	uint16_t flags; //0x0000
	uint16_t attributesIndex; //0x0002
	int32_t nameOffset; //0x0004
}; //Size: 0x0008
static_assert(sizeof(REPropertyImpl) == 0x8);

class REParameterDef
{
public:
	uint16_t attributesIndex; //0x0000
	uint16_t initData; //0x0002
	uint32_t nameAndModifer; //0x0004 0-29 = name index, 30-31 = modifier
	uint32_t typeAndFlags; //0x0008 0-17 = type index, 18-31 = flags
}; //Size: 0x000C
static_assert(sizeof(REParameterDef) == 0xC);

class RENativeArray_SystemRuntimeType
{
public:
	class SystemRuntimeType *(*entries)[93788]; //0x0000
	uint32_t num; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
static_assert(sizeof(RENativeArray_SystemRuntimeType) == 0x10);

class RENativeArray_Static
{
public:
	class N00004CB2 *(*N00002F64)[93788]; //0x0000
	uint64_t num; //0x0008
}; //Size: 0x0010
static_assert(sizeof(RENativeArray_Static) == 0x10);

class N0000306B
{
public:
	char pad_0000[448]; //0x0000
}; //Size: 0x01C0
static_assert(sizeof(N0000306B) == 0x1C0);

class REVM
{
public:
	char pad_0000[13616]; //0x0000
	int64_t perfCounter[8]; //0x3530
	bool N00002F5E; //0x3570
	char pad_3571[3]; //0x3571
	uint32_t N000030CA; //0x3574
	char pad_3578[32]; //0x3578
	class RENativeArray_SystemRuntimeType typeTbl; //0x3598
	class RENativeArray_Static staticTbl; //0x35A8
	char pad_35B8[32]; //0x35B8
	class RETypeDB *typeDb; //0x35D8
	class N00003030 *N00002F73; //0x35E0
	class N0000306B jobs[256]; //0x35E8
	char pad_1F5E8[40]; //0x1F5E8
}; //Size: 0x1F610
static_assert(sizeof(REVM) == 0x1F610);

class N00003030
{
public:
	char pad_0000[136]; //0x0000
}; //Size: 0x0088
static_assert(sizeof(N00003030) == 0x88);

class REVMPtr
{
public:
	class REVM *N000030D2; //0x0000
}; //Size: 0x0008
static_assert(sizeof(REVMPtr) == 0x8);

class SystemRuntimeType : public REManagedObject
{
public:
	class REClassInfo *runtimeTypeHandle; //0x0010
	int32_t kind; //0x0018
	int32_t number; //0x001C
}; //Size: 0x0020
static_assert(sizeof(SystemRuntimeType) == 0x20);

class REAttributeDef
{
public:
	int32_t methodIndex; //0x0000
	int32_t blob; //0x0004
}; //Size: 0x0008
static_assert(sizeof(REAttributeDef) == 0x8);

class REParamList
{
public:
	uint16_t numParams; //0x0000
	uint16_t invokeID; //0x0002
	uint32_t returnType; //0x0004
	uint32_t params[1]; //0x0008
}; //Size: 0x000C
static_assert(sizeof(REParamList) == 0xC);

class ArrayDeserializeSequence
{
public:
	class DeserializeSequence (*deserializers)[256]; //0x0000
	uint32_t num; //0x0008
	uint32_t numAllocated; //0x000C
}; //Size: 0x0010
static_assert(sizeof(ArrayDeserializeSequence) == 0x10);

class RETypeCLR : public REType
{
public:
	class ArrayDeserializeSequence deserializeThing; //0x0060
	class REType *nativeType; //0x0070
	char *name2; //0x0078
}; //Size: 0x0080
static_assert(sizeof(RETypeCLR) == 0x80);

class DeserializeSequence
{
public:
	int32_t data; //0x0000
	uint32_t offset; //0x0004
	class REClassInfo *nativeType; //0x0008
}; //Size: 0x0010
static_assert(sizeof(DeserializeSequence) == 0x10);

class InternStrings
{
public:
	class SystemString *(*N00004B5E)[20000]; //0x0000
}; //Size: 0x0008
static_assert(sizeof(InternStrings) == 0x8);

class N00004CB2
{
public:
	char pad_0000[136]; //0x0000
}; //Size: 0x0088
static_assert(sizeof(N00004CB2) == 0x88);

class REAttribute : public REObject
{
public:
	char pad_0008[8]; //0x0008
	class REAttribute *next; //0x0010
}; //Size: 0x0018
static_assert(sizeof(REAttribute) == 0x18);

class N0000301D
{
public:
	char pad_0000[136]; //0x0000
}; //Size: 0x0088
static_assert(sizeof(N0000301D) == 0x88);