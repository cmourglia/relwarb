#ifndef RELWRAB_DEFINES_H
#define RELWRAB_DEFINES_H

#include <stdint.h>

#define global_variable static
#define internal static

typedef signed char int8;
typedef int16_t int16;
typedef int32_t int32;

typedef unsigned char uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef int32_t bool32;

typedef float real32;
typedef double real64;

#if defined(_DEBUG)
#define Assert(x) if (!(x)) { *(int*)0 = 0; }
#else
define Assert(x)
#endif

#if defined(STRINGIFY)
#undef STRINGIFY
#endif

#define STRINGIFY(x) #x

#endif // RELWRAB_DEFINES_H
