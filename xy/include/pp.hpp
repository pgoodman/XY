/*
 * pp.hpp
 *
 *  Created on: Nov 24, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_PP_HPP_
#define XY_PP_HPP_

#if defined(__HOS_LINUX__) || defined(__linux__) || defined(linux) || defined(__linux)
#   define XY_HOST_LINUX 1
#else
#   define XY_HOST_LINUX 0
#endif

#if _SVID_SOURCE || _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED || /* Since glibc 2.12: */ _POSIX_C_SOURCE >= 200809L
#   define XY_LINUX_HAS_STRDUP 1
#else
#   define XY_LINUX_HAS_STRDUP 0
#endif

#endif /* XY_PP_HPP_ */
