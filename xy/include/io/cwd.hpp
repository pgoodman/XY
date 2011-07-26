/*
 * cwd.hpp
 *
 *  Created on: Jul 23, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef CWD_HPP_
#define CWD_HPP_

#ifdef WINDOWS
    #include <direct.h>
    #define get_cwd_impl _getcwd
#else
    #include <unistd.h>
    #define get_cwd_impl getcwd
 #endif

namespace xy { namespace io {

    /// get the current working directory of the compiler
    const char *get_cwd(void) throw();

}}

#endif /* CWD_HPP_ */
