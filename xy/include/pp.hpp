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

#if defined(__GNUC__) && XY_HOST_LINUX

#   if !defined(_SVID_SOURCE)
#       define _SVID_SOURCE 0
#   endif

#   if !defined(_BSD_SOURCE)
#       define _BSD_SOURCE 0
#   endif

#   if !defined(_XOPEN_SOURCE)
#       define _XOPEN_SOURCE 0
#   endif

#   if !defined(_XOPEN_SOURCE_EXTENDED)
#       define _XOPEN_SOURCE_EXTENDED 0
#   endif

#   if !defined(_POSIX_C_SOURCE)
#       define _POSIX_C_SOURCE 0
#   endif

#   if _SVID_SOURCE || _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED || /* Since glibc 2.12: */ _POSIX_C_SOURCE >= 200809L
#      define XY_LINUX_HAS_STRDUP 1
#   else
#      define XY_LINUX_HAS_STRDUP 0
#   endif
#else
#      define XY_LINUX_HAS_STRDUP 0
#endif

#define XY_CAT_(a, b) a ## b
#define XY_CAT(a, b) XY_CAT_(a, b)

/* https://groups.google.com/forum/#!topic/comp.std.c/d-6Mj5Lko_s
 * get number of variadic things */
#define XY_NARG(...) \
         XY_NARG_(__VA_ARGS__,XY_RSEQ_N())
#define XY_NARG_(...) \
         XY_ARG_N(__VA_ARGS__)
#define XY_ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define XY_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

/* represents nothing */
#define XY_NOTHING10 XY_NOTHING9
#define XY_NOTHING9 XY_NOTHING8
#define XY_NOTHING8 XY_NOTHING7
#define XY_NOTHING7 XY_NOTHING6
#define XY_NOTHING6 XY_NOTHING5
#define XY_NOTHING5 XY_NOTHING4
#define XY_NOTHING4 XY_NOTHING3
#define XY_NOTHING3 XY_NOTHING2
#define XY_NOTHING2 XY_NOTHING1
#define XY_NOTHING1
#define XY_NOTHING XY_NOTHING10

#define XY_EMPTY_COMPILATION_UNIT \
    static void bar(void); \
    static void foo(void) { (void) bar; } \
    static void bar(void) { (void) foo; }

#endif /* XY_PP_HPP_ */
