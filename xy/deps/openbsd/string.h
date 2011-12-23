
#ifndef XY_OPENBSD_STRING_H_
#define XY_OPENBSD_STRING_H_

#include "xy/include/pp.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

/* define the linkage for strdup */
#if XY_HOST_LINUX && !XY_LINUX_HAS_STRDUP
#   define XY_STRDUP_LINKAGE
#else
#   define XY_STRDUP_LINKAGE extern
#endif

/* define the linkage for strcasecmp */
#if XY_HOST_LINUX && !defined(HAVE_STRCASECMP)
#   define XY_STRCASECMP_LINKAGE
#else
#   define XY_STRCASECMP_LINKAGE extern
#endif

XY_STRDUP_LINKAGE char *strdup(const char *str) XY_EXCEPTION_SPEC;
XY_STRCASECMP_LINKAGE int strcasecmp(const char *s1, const char *s2) XY_EXCEPTION_SPEC;
XY_STRCASECMP_LINKAGE int strncasecmp(const char *s1, const char *s2, size_t n) XY_EXCEPTION_SPEC;

#if XY_HOST_LINUX
size_t strlcat(char *dst, const char *src, size_t siz) XY_EXCEPTION_SPEC;
size_t strlcpy(char *dst, const char *src, size_t siz) XY_EXCEPTION_SPEC;
#endif

#ifdef __cplusplus
}
#endif

#endif
