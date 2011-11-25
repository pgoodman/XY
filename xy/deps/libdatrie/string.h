/*
 * string.h
 *
 *  Created on: Nov 24, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_STRING_H_
#define XY_STRING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "xy/include/pp.hpp"



#if XY_HOST_LINUX
#   if !XY_LINUX_HAS_STRDUP
        char *strdup(const char *str);
#   endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* XY_STRING_H_ */
