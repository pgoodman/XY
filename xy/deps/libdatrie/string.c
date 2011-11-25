/*
 * string.c
 *
 *  Created on: Nov 24, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_STRING_C_
#define XY_STRING_C_

#ifdef __cplusplus
extern "C" {
#endif

#include "xy/deps/libdatrie/string.h"

/* hack to allow "empty" translation unit */
static void useless2(void);
static void useless1(void) {
    (void) useless2;
}
static void useless2(void) {
    (void) useless1;
}

#if XY_HOST_LINUX
#   if !XY_LINUX_HAS_STRDUP
#       include "xy/deps/openbsd/strdup.c"
#   endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* XY_STRING_C_ */
