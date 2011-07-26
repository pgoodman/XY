/*
 * cwd.cpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#include <cstdio>
#include <cstring>

#include "xy/include/io/cwd.hpp"
#include "xy/include/array.hpp"

namespace xy { namespace io {

    const char *get_cwd(void) throw() {
        static bool have_cwd(false);
        static char CWD[FILENAME_MAX + 1U]{'\0'};

        static_assert(static_cast<size_t>(FILENAME_MAX + 1U) > FILENAME_MAX,
            "Macro FILENAME_MAX is set to maximum value for size_t and caused "
            "a rollover."
        );

        if(!have_cwd) {
            if(!get_cwd_impl(&(CWD[0]), array::size(CWD))) {
                CWD[0] = '\0';
            }
            CWD[FILENAME_MAX] = '\0';
        }

        return &(CWD[0]);
    }

}}
