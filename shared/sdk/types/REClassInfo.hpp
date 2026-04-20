#pragma once

// REClassInfo is the same runtime structure as RETypeDefinition.
// The name is a historical artifact from before the TDB was fully researched.
//
// Under REFRAMEWORK_UNIVERSAL, this is an empty opaque struct.
// Cast any REClassInfo* to sdk::RETypeDefinition* to access fields.
// All consumer code should use RETypeDefinition methods instead.
#ifdef REFRAMEWORK_UNIVERSAL
class REClassInfo {};
#endif
