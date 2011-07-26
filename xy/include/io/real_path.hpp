/*
 * realpath.hpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef REALPATH_HPP_
#define REALPATH_HPP_

#include <sys/param.h>

namespace xy { namespace io {

    enum {
        REAL_PATH_MAX_LEN = PATH_MAX
    };

    char *get_real_path(const char *path, char (&resolved)[REAL_PATH_MAX_LEN]);

}}

#endif /* REALPATH_HPP_ */
