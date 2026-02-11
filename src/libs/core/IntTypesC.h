#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#if defined(__SIZEOF_INT128__)
typedef __int128 Int128;
typedef unsigned __int128 UInt128;
#else
    // FIXME: get fallback
    #error "__int128 isn't supported by this compiler"
#endif

#ifdef __cplusplus
}
#endif
