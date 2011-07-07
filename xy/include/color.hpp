/*
 * color.hpp
 *
 *  Created on: Jul 3, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef XY_COLOR_HPP_
#define XY_COLOR_HPP_


#if defined(_MSC_VER)

#define XY_F_DEF ""
#define XY_F_YELLOW ""
#define XY_F_PINK ""
#define XY_F_GREEN ""
#define XY_F_RED ""
#define XY_F_BLUE ""

#define XY_F_BOLD ""
#define XY_F_UNDER ""

#else

#define XY_F_DEF "\033[0m"
#define XY_F_YELLOW "\033[33m"
#define XY_F_PINK "\033[35m"
#define XY_F_GREEN "\033[32m"
#define XY_F_RED "\033[31m"
#define XY_F_BLUE "\033[34m"

#define XY_F_BOLD "\033[1m"
#define XY_F_UNDER "\033[4m"

#endif

#endif /* XY_COLOR_HPP_ */
