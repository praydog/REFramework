#pragma once

extern "C" bool __declspec(dllexport) __stdcall SetupDirectX(void* item, int graphicsAPI = 0);

extern "C" bool __declspec(dllexport) __stdcall SetupGraphicDevice(void* tex, int graphicsAPI = 0);

extern "C" __declspec(dllexport) void* __stdcall InitUpscaleFeature(int id, int upscaleMethod, int qualityLevel, int displaySizeX, int displaySizeY, bool isContentHDR, bool depthInverted, bool YAxisInverted, bool motionVetorsJittered, bool enableSharpening, bool enableAutoExposure, int format = 10);

extern "C" void __declspec(dllexport) __stdcall EvaluateUpscale(int id, void* color, void* motionVector, void* depth, void* destination, float sharpness, float jitterOffsetX, float jitterOffsetY,
	bool reset, float nearPlane, float farPlane, float verticalFOV);

extern "C" void __declspec(dllexport) __stdcall EvaluateUpscaleDX12(int id, bool execute, void* color, void* motionVector, void* depth, void* destination, float sharpness, float jitterOffsetX, float jitterOffsetY,
	bool reset, float nearPlane, float farPlane, float verticalFOV);

extern "C" void __declspec(dllexport) __stdcall SetMotionScaleX(int id = 1, float motionScaleX = 1);

extern "C" void __declspec(dllexport) __stdcall SetMotionScaleY(int id = 1, float motionScaleX = 1);

extern "C" int __declspec(dllexport) __stdcall GetRenderWidth(int id = 1);

extern "C" int __declspec(dllexport) __stdcall GetRenderHeight(int id = 1);

extern "C" float __declspec(dllexport) __stdcall GetOptimalSharpness(int id = 1);

extern "C" float __declspec(dllexport) __stdcall GetOptimalMipmapBias(int id = 1);

extern "C" void __declspec(dllexport) __stdcall SetDebug(bool debug = true);

extern "C" void __declspec(dllexport) __stdcall ReleaseUpscaleFeature(int id);

extern "C" int __declspec(dllexport) __stdcall GetJitterPhaseCount(int id);

extern "C" int __declspec(dllexport) __stdcall GetJitterOffset(float* outX, float* outY, int index, int phaseCount);

extern "C" void __declspec(dllexport) __stdcall InitCSharpDelegate(void (*Log)(char* message, int iSize));

extern "C" bool __declspec(dllexport) __stdcall IsUpscaleMethodAvailable(int upscaleMethod);

extern "C" __declspec(dllexport) char* __stdcall GetUpscaleMethodName(int upscaleMethod);

