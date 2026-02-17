#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#if defined(__SIZEOF_INT128__)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

typedef __int128 Int128;
typedef unsigned __int128 UInt128;

#pragma GCC diagnostic pop

#else

// FIXME: get fallback
#error "__int128 is not supported by this compiler. Please provide a fallback implementation."

#endif

#ifdef __cplusplus
}
#endif
