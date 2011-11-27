/*
 * stdlib.h
 *
 *  Created on: Nov 27, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_OPENBSD_STDLIB_H_
#define XY_OPENBSD_STDLIB_H_

#include "xy/include/pp.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <sys/param.h>
#include <sys/stat.h>

#if !defined(HAVE_REALPATH) || defined(BROKEN_REALPATH)
#   define XY_REALPATH_LINKAGE
#else
#   define XY_REALPATH_LINKAGE extern
#endif

extern int lstat(const char *path, struct stat *buf);
extern ssize_t readlink(const char *path, char *buf, size_t bufsiz);

XY_REALPATH_LINKAGE char *realpath(const char *pathname, char *resolved_path);

#ifdef __cplusplus
}
#endif

#endif /* XY_STDLIB_H_ */
