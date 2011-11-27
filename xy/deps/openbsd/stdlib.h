/*
 * stdlib.h
 *
 *  Created on: Nov 27, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_OPENBSD_STDLIB_H_
#define XY_OPENBSD_STDLIB_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

char *realpath(const char *pathname, char *resolved_path);

#ifdef __cplusplus
}
#endif

#endif /* XY_STDLIB_H_ */
