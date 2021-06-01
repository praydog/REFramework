#pragma once

#include <cstdint>
#include "Math.hpp"

#pragma pack(push, r1, 1)
#ifdef DMC5
#define TDB_VER 67
#include "ReClass_Internal_DMC5.hpp"
#elif RE8
#define TDB_VER 69
#include "ReClass_Internal_RE8.hpp"
#elif RE3
#define TDB_VER 67
#include "ReClass_Internal_RE3.hpp"
#else
#define TDB_VER 67
#include "ReClass_Internal.hpp"
#endif
#pragma pack(pop, r1)

#include "Enums_Internal.hpp"

#include "RETypes.hpp"
#include "REType.hpp"
#include "RETypeCLR.hpp"
#include "RETypeDefinition.hpp"
#include "REArray.hpp"
#include "REContext.hpp"
#include "REManagedObject.hpp"
#include "REString.hpp"
#include "RETransform.hpp"
#include "REComponent.hpp"
#include "RopewaySweetLightManager.hpp"
#include "REVariableDescriptor.hpp"
#include "REGlobals.hpp"
