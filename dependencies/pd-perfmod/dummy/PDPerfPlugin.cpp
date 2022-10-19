// dummy implementation of the DLL so I don't need to commit the DLL to the repo
#include "../include/PDPerfPlugin.h"

bool __stdcall SetupDirectX(void* item, int graphicsAPI){return false;};

bool __stdcall SetupGraphicDevice(void* tex, int graphicsAPI){return false;};

void* __stdcall InitUpscaleFeature(int id, int upscaleMethod, int qualityLevel, int displaySizeX, int displaySizeY, bool isContentHDR, bool depthInverted, bool YAxisInverted, bool motionVetorsJittered, bool enableSharpening, bool enableAutoExposure, int format) {return nullptr;};

void __stdcall EvaluateUpscale(int id, void* color, void* motionVector, void* depth, void* destination, float sharpness, float jitterOffsetX, float jitterOffsetY,
	bool reset, float nearPlane, float farPlane, float verticalFOV) {};

void __stdcall SetMotionScaleX(int id, float motionScaleX) {};

void __stdcall SetMotionScaleY(int id, float motionScaleX) {};

int __stdcall GetRenderWidth(int id) { return 0; };

int __stdcall GetRenderHeight(int id) { return 0; };

float __stdcall GetOptimalSharpness(int id) { return 0.0f; };

float __stdcall GetOptimalMipmapBias(int id) { return 0.0f;};

void __stdcall SetDebug(bool debug) {};

void __stdcall ReleaseUpscaleFeature(int id) {};

int __stdcall GetJitterPhaseCount(int id) { return 0; };

int __stdcall GetJitterOffset(float* outX, float* outY, int index, int phaseCount) { return 0; };

void __stdcall InitCSharpDelegate(void (*Log)(char* message, int iSize)) {};

bool  __stdcall IsUpsacleMethodAvailable(int upscaleMethod) { return false; };

char* __stdcall GetUpscaleMethodName(int upscaleMethod) { return ""; };
