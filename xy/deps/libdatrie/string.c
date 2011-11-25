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

#if XY_HOST_LINUX
#   if !XY_LINUX_HAS_STRDUP
#       include "xy/deps/openbsd/strdup.c"
#   endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* XY_STRING_C_ */
