#pragma once

/* For reference
* I can't use enum as parameter type cuz unity does't work well
* No VULKAN yet
enum GraphicsAPI
{
	D3D11,
	D3D12
};
enum UpscaleType
{
	DLSS,
	FSR2,
	XESS
};
enum PerfQualityLevel
{
	Performance,
	Balanced,
	Quality,
	UltraPerformance // or UltraQuality for XeSS, I don't know why Intel has to be different
};
*/

typedef struct
{
	unsigned int X;
	unsigned int Y;
} Coordinates;

typedef struct
{
	int  id;
	int  upscaleMethod;
	int  qualityLevel;
	int  displaySizeX;
	int  displaySizeY;
	int  format;
	bool isContentHDR;
	bool depthInverted;
	bool YAxisInverted;
	bool motionVetorsJittered;
	bool enableSharpening;
	bool enableAutoExposure;
} InitParams;

struct UpscaleParams
{
	int         id;
	void*       color;
	void*       motionVector;
	void*       depth;
	void*       mask;
	void*       resolvedColor = nullptr;  // Do not use
	void*       destination;
	float       renderSizeX;
	float       renderSizeY;
	float       sharpness;
	float       jitterOffsetX;
	float       jitterOffsetY;
	float       motionScaleX;
	float       motionScaleY;
	bool        reset;
	float       nearPlane;
	float       farPlane;
	float       verticalFOV;
	bool        execute;
	void*       cmdList = nullptr;  // Don not use
	Coordinates colorBase = { 0, 0 };
	Coordinates depthBase = { 0, 0 };
	Coordinates motionBase = { 0, 0 };
};

/* If you are calling EvaluateUpscale more than once per frame, set execute to true only at last call*/

extern "C" __declspec(dllexport) bool __stdcall SetupDirectX(void* item, int graphicsAPI);

extern "C" __declspec(dllexport) void* __stdcall SimpleInit(int id, int upscaleMethod, int qualityLevel, int displaySizeX, int displaySizeY, bool isContentHDR, bool depthInverted, bool YAxisInverted,
	bool motionVetorsJittered, bool enableSharpening, bool enableAutoExposure, int format = 10);

extern "C" __declspec(dllexport) void* __stdcall InitUpscaler(InitParams* params);

extern "C" __declspec(dllexport) void __stdcall SimpleEvaluate(int id, void* color, void* motionVector, void* depth, void* mask, void* destination, int renderSizeX, int renderSizeY, float sharpness,
	float jitterOffsetX, float jitterOffsetY, int motionScaleX, int motionScaleY, bool reset, float nearPlane, float farPlane, float verticalFOV, bool execute = true);

extern "C" __declspec(dllexport) void __stdcall EvaluateUpscaler(UpscaleParams* params);

extern "C" __declspec(dllexport) void __stdcall SetMotionScaleX(int id, float motionScaleX);

extern "C" __declspec(dllexport) void __stdcall SetMotionScaleY(int id, float motionScaleX);

extern "C" __declspec(dllexport) int __stdcall GetRenderWidth(int id);

extern "C" __declspec(dllexport) int __stdcall GetRenderHeight(int id);

extern "C" __declspec(dllexport) float __stdcall GetOptimalSharpness(int id);

extern "C" __declspec(dllexport) float __stdcall GetOptimalMipmapBias(int id);

extern "C" __declspec(dllexport) void __stdcall SetDebug(bool debug = true);

extern "C" __declspec(dllexport) void __stdcall ReleaseUpscaleFeature(int id);

extern "C" __declspec(dllexport) int __stdcall GetJitterPhaseCount(int id);

extern "C" __declspec(dllexport) int __stdcall GetJitterOffset(float* outX, float* outY, int index, int phaseCount);

extern "C" __declspec(dllexport) void __stdcall InitLogDelegate(void (*Log)(char* message, int iSize));

extern "C" __declspec(dllexport) bool __stdcall IsUpscaleMethodAvailable(int upscaleMethod);

extern "C" __declspec(dllexport) char* __stdcall GetUpscaleMethodName(int upscaleMethod);