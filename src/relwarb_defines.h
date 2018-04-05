#ifndef RELWARB_DEFINES_H
#define RELWARB_DEFINES_H

#include <stdint.h>
#include <limits>

#if defined(_WIN32) || defined(_WIN64)
#define OS_WINDOWS
#elif defined(__linux__) || defined(__CYGWIN__)
#define OS_LINUX
#elif defined(__APPLE__) || defined(__MACH__)
#define OS_MACOS
#endif

#if defined(_DEBUG)
#define RELWARB_DEBUG
#endif

#if defined(OS_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#pragma warning(push)
#pragma warning(disable : 4100) // Parameters not used
#pragma warning(disable : 4189) // Variable initialized but not used
#pragma warning(disable : 4201) // Nameless struct / unions
#pragma warning(disable : 4244) // Conversion from double to float
#pragma warning(disable : 4505) // Function not used
#pragma warning(disable : 4305) // Truncation from double to float

#endif

#define global_variable static
#define internal static
#define local_persist static

typedef signed char int8;
typedef int16_t int16;
typedef int32_t int32;

typedef unsigned char uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef int32_t bool32;

typedef float real32;
typedef double real64;

constexpr real32 Epsilon32 = std::numeric_limits<real32>::epsilon();

#if defined(RELWARB_DEBUG)
# if defined(OS_LINUX)
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

using ComponentID = int32;
using EntityID    = uint32;
#endif // RELWARB_DEFINES_H
