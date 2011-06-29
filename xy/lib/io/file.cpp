/*
 * file.cpp
 *
 *  Created on: Jun 15, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#include "xy/include/io/file.hpp"

#include <unistd.h>

namespace xy { namespace io {

    file<read_tag>::file(void) throw() : fd{INVALID_FILE_DESCRIPTOR} { }
    file<read_tag>::file(int _fd) throw() : fd{_fd} { }
    file<read_tag>::~file(void) throw() {
        if(INVALID_FILE_DESCRIPTOR != fd) {
            close(fd);
            fd = INVALID_FILE_DESCRIPTOR;
        }
    }
    const char *file<read_tag>::name(void) const throw() {
        return file_name;
    }
}}
