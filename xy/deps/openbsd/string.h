
#ifndef XY_OPENBSD_STRING_H_
#define XY_OPENBSD_STRING_H_

#include "xy/include/pp.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

char *strdup(const char *str);

#if XY_HOST_LINUX
size_t strlcat(char *dst, const char *src, size_t siz);
size_t strlcpy(char *dst, const char *src, size_t siz);
#endif

int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);

#ifdef __cplusplus
}
#endif

#endif
