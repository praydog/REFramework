#pragma once

#include <cstdint>
#include "Math.hpp"

#include "TDBVer.hpp"

#pragma pack(push, r1, 1)
#ifdef DMC5
#include "ReClass_Internal_DMC5.hpp"
#elif defined(DD2)
#include "ReClass_Internal_DD2.hpp" // Copy of SF6
#elif defined(SF6)
#include "ReClass_Internal_SF6.hpp"
#elif defined(MHRISE)
#include "ReClass_Internal_RE8.hpp"
#elif defined(RE8)
#include "ReClass_Internal_RE8.hpp"
#elif defined(RE4)
#include "ReClass_Internal_RE4.hpp"
#elif defined(RE3)
#ifdef RE3_TDB67
#include "ReClass_Internal_RE3.hpp"
#else
#include "ReClass_Internal_RE3_TDB70.hpp"
#endif
#elif defined(RE2)
#ifdef RE2_TDB66
#include "ReClass_Internal.hpp"
#else
#include "ReClass_Internal_RE2_TDB70.hpp"
#endif
#elif defined(RE7)
#ifdef RE7_TDB49
#include "ReClass_Internal_RE7.hpp"
#else
#include "ReClass_Internal_RE2_TDB70.hpp"
#endif
#endif
#pragma pack(pop, r1)

#include "Enums_Internal.hpp"

#include "RETypes.hpp"
#include "REType.hpp"
#include "RETypeCLR.hpp"
#include "RETypeDB.hpp"
#include "RETypeDefinition.hpp"
#include "REArray.hpp"
#include "REContext.hpp"
#include "REManagedObject.hpp"
#include "SystemArray.hpp"
#include "REString.hpp"
#include "RETransform.hpp"
#include "REComponent.hpp"
#include "RopewaySweetLightManager.hpp"
#include "REVariableDescriptor.hpp"
#include "REGlobals.hpp"
