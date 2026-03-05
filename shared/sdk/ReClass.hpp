#pragma once

#include <cstdint>
#include "Math.hpp"

#include "TDBVer.hpp"

#pragma pack(push, r1, 1)
#ifdef REFRAMEWORK_UNIVERSAL
// Monolithic build: RE8 layout at global scope (majority of games use 0x60 REType).
// MHWILDS/RE9 have 0x68 REType — runtime dispatch via RETypeLayouts.hpp accessors.
#include "ReClass_Internal_RE8.hpp"
#else
// Legacy per-game build (when individual game macros are defined)
#ifdef DMC5
#include "ReClass_Internal_DMC5.hpp"
#elif defined(PRAGMATA)
#include "ReClass_Internal_PRAGMATA.hpp"
#elif defined(RE9)
#include "ReClass_Internal_RE9.hpp"
#elif defined(MHWILDS)
#include "ReClass_Internal_MHWILDS.hpp"
#elif defined(MHSTORIES3)
#include "ReClass_Internal_MHSTORIES3.hpp"
#elif defined(DD2)
#include "ReClass_Internal_DD2.hpp"
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
#endif // game macros
#endif // REFRAMEWORK_UNIVERSAL
#pragma pack(pop, r1)

#include "Enums_Internal.hpp"

#include "RETypes.hpp"
#include "REType.hpp"
#include "RETypeLayouts.hpp"
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
