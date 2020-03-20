#pragma once

#include <cstdint>
#include "Math.hpp"

#pragma pack(push, r1, 1)
#ifdef RE3
#include "ReClass_Internal_RE3.hpp"
#else
#include "ReClass_Internal.hpp"
#endif
#pragma pack(pop, r1)

#include "Enums_Internal.hpp"

#include "RETypes.hpp"
#include "REArray.hpp"
#include "REContext.hpp"
#include "REManagedObject.hpp"
#include "REString.hpp"
#include "RETransform.hpp"
#include "REComponent.hpp"
#include "RopewaySweetLightManager.hpp"
#include "REGlobals.hpp"
