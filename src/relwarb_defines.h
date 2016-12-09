#ifndef RELWARB_DEFINES_H
#define RELWARB_DEFINES_H

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
# if defined(LINUX)
#  define Assert(x) if (!(x)) { __builtin_trap(); }
# else
#  define Assert(x) if (!(x)) { *(int*)0 = 0; }
# endif
#else
# define Assert(x)
#endif

#if defined(STRINGIFY)
#undef STRINGIFY
#endif

#define STRINGIFY(x) #x

#endif // RELWARB_DEFINES_H
