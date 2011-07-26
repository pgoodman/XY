/*      $OpenBSD: realpath.c,v 1.13 2005/08/08 08:05:37 espie Exp $ */
/*
 * Copyright (c) 2003 Constantin S. Svintsoff <kostik@iclub.nsu.ru>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The names of the authors may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* OPENBSD ORIGINAL: lib/libc/stdlib/realpath.c */

#if !defined(HAVE_REALPATH) || defined(BROKEN_REALPATH)

#include "xy/include/io/real_path.hpp"
#include "xy/include/io/cwd.hpp"

#include <sys/stat.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

namespace xy { namespace io {

    /*
     * char *realpath(const char *path, char resolved[REAL_PATH_MAX_LEN]);
     *
     * Find the real name of path, by removing all ".", ".." and symlink
     * components.  Returns (resolved) on success, or (nullptr) on failure,
     * in which case the path which caused trouble is left in (resolved).
     */
    char *
    get_real_path(const char *path, char (&resolved)[REAL_PATH_MAX_LEN])
    {
        struct stat sb;
        char *p, *q, *s;
        size_t left_len, resolved_len;
        unsigned symlinks;
        int serrno, slen;
        char left[REAL_PATH_MAX_LEN], next_token[REAL_PATH_MAX_LEN], symlink[REAL_PATH_MAX_LEN];

        serrno = errno;
        symlinks = 0;
        if (path[0] == '/') {
            resolved[0] = '/';
            resolved[1] = '\0';
            if (path[1] == '\0')
                return (resolved);
            resolved_len = 1;
            left_len = strlcpy(left, path + 1, sizeof(left));
        } else {
            if (get_cwd_impl(resolved, REAL_PATH_MAX_LEN) == nullptr) {
                strlcpy(resolved, ".", REAL_PATH_MAX_LEN);
                return (nullptr);
            }
            resolved_len = strlen(resolved);
            left_len = strlcpy(left, path, sizeof(left));
        }
        if (left_len >= sizeof(left) || resolved_len >= REAL_PATH_MAX_LEN) {
            errno = ENAMETOOLONG;
            return (nullptr);
        }

        /*
         * Iterate over path components in `left'.
         */
        while (left_len != 0) {
            /*
             * Extract the next path component and adjust `left'
             * and its length.
             */
            p = strchr(left, '/');
            s = p ? p : left + left_len;
            if (static_cast<size_t>(s - left) >= sizeof(next_token)) {
                errno = ENAMETOOLONG;
                return (nullptr);
            }
            memcpy(next_token, left, s - left);
            next_token[s - left] = '\0';
            left_len -= s - left;
            if (p != nullptr)
                memmove(left, s + 1, left_len + 1);
            if (resolved[resolved_len - 1] != '/') {
                if (resolved_len + 1 >= REAL_PATH_MAX_LEN) {
                    errno = ENAMETOOLONG;
                    return (nullptr);
                }
                resolved[resolved_len++] = '/';
                resolved[resolved_len] = '\0';
            }
            if (next_token[0] == '\0')
                continue;
            else if (strcmp(next_token, ".") == 0)
                continue;
            else if (strcmp(next_token, "..") == 0) {
                /*
                 * Strip the last path component except when we have
                 * single "/"
                 */
                if (resolved_len > 1) {
                    resolved[resolved_len - 1] = '\0';
                    q = strrchr(resolved, '/') + 1;
                    *q = '\0';
                    resolved_len = q - resolved;
                }
                continue;
            }

            /*
             * Append the next path component and lstat() it. If
             * lstat() fails we still can return successfully if
             * there are no more path components left.
             */
            resolved_len = strlcat(resolved, next_token, REAL_PATH_MAX_LEN);
            if (resolved_len >= REAL_PATH_MAX_LEN) {
                errno = ENAMETOOLONG;
                return (nullptr);
            }
            if (lstat(resolved, &sb) != 0) {
                if (errno == ENOENT && p == nullptr) {
                    errno = serrno;
                    return (resolved);
                }
                return (nullptr);
            }
            if (S_ISLNK(sb.st_mode)) {
                if (symlinks++ > MAXSYMLINKS) {
                    errno = ELOOP;
                    return (nullptr);
                }
                slen = readlink(resolved, symlink, sizeof(symlink) - 1);
                if (slen < 0)
                    return (nullptr);
                symlink[slen] = '\0';
                if (symlink[0] == '/') {
                    resolved[1] = 0;
                    resolved_len = 1;
                } else if (resolved_len > 1) {
                    /* Strip the last path component. */
                    resolved[resolved_len - 1] = '\0';
                    q = strrchr(resolved, '/') + 1;
                    *q = '\0';
                    resolved_len = q - resolved;
                }

                /*
                 * If there are any path components left, then
                 * append them to symlink. The result is placed
                 * in `left'.
                 */
                if (p != nullptr) {
                    if (symlink[slen - 1] != '/') {
                        if (static_cast<size_t>(slen + 1) >= sizeof(symlink)) {
                            errno = ENAMETOOLONG;
                            return (nullptr);
                        }
                        symlink[slen] = '/';
                        symlink[slen + 1] = 0;
                    }
                    left_len = strlcat(symlink, left, sizeof(left));
                    if (left_len >= sizeof(left)) {
                        errno = ENAMETOOLONG;
                        return (nullptr);
                    }
                }
                left_len = strlcpy(left, symlink, sizeof(left));
            }
        }

        /*
         * Remove trailing slash except when the resolved pathname
         * is a single "/".
         */
        if (resolved_len > 1 && resolved[resolved_len - 1] == '/')
            resolved[resolved_len - 1] = '\0';
        return (resolved);
    }

}}
#endif /* !defined(HAVE_REALPATH) || defined(BROKEN_REALPATH) */
