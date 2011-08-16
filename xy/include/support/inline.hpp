/*
 * inline.hpp
 *
 *  Created on: Aug 15, 2011
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef XY_INLINE_HPP_
#define XY_INLINE_HPP_


#if defined(__INTEL_COMPILER)
#define XY_INLINE __inline
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
#define XY_INLINE __forceinline
#elif defined(__GNUC__) && ((__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 2)))
#define XY_INLINE __attribute__((always_inline))
#elif defined(__clang__)
#define XY_INLINE __attribute__((always_inline))
#else
#define XY_INLINE inline
#endif



#endif /* XY_INLINE_HPP_ */
