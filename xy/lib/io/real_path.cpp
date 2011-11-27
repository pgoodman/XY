
#include "xy/include/io/real_path.hpp"
#include "xy/include/cstring.hpp"

#include <sys/param.h>
#include <sys/stat.h>

#ifdef PATH_MAX
#undef PATH_MAX
#endif

#define PATH_MAX xy::io::REAL_PATH_MAX_LEN
extern "C" {
#   include "xy/deps/openbsd/realpath.c"
}

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
        return realpath(path, &(resolved[0]));
    }
}}
