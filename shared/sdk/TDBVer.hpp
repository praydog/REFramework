#pragma once

#ifdef DMC5
#define TDB_VER 67
#elif defined(MHRISE)
#ifdef MHRISE_TDB70
#define TDB_VER 70
#else
#define TDB_VER 71
#endif
#elif defined(RE8)
#define TDB_VER 69
#elif RE3
#ifdef RE3_TDB67
#define TDB_VER 67
#else
#define TDB_VER 70
#endif
#elif RE2
#ifdef RE2_TDB66
#define TDB_VER 66
#else
#define TDB_VER 70
#endif
#elif RE7
#ifdef RE7_TDB49
#define TDB_VER 49
#else
#define TDB_VER 70
#endif
#endif