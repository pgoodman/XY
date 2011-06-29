/*
 * array.hpp
 *
 *  Created on: Jun 19, 2011
 *      Author: Peter Goodman
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef XY_ARRAY_HPP_
#define XY_ARRAY_HPP_

namespace xy { namespace array {

    /// type-aware and safe slice of an array
    template <
        const size_t START,
        const size_t SLICE_LENGTH,
        typename T,
        const size_t GIVEN_LENGTH
    >
    inline constexpr auto slice(T (&arr)[GIVEN_LENGTH]) throw() -> T (&)[SLICE_LENGTH] {
        static_assert(
            GIVEN_LENGTH >= SLICE_LENGTH,
            "The requested array slice is contains more slots than the "
            "original array."
        );
        static_assert(
            START < GIVEN_LENGTH,
            "Out-of-bounds array slice. The start index of the slice is "
            "beyond the end of the array."
        );
        static_assert(
            SLICE_LENGTH > 0,
            "Zero length array slices are not allowed."
        );
        static_assert(
            (START + SLICE_LENGTH) <= GIVEN_LENGTH,
            "Out-of-bounds array slice. The slice extends beyond the end of "
            "the original array."
        );
        return reinterpret_cast<T (&)[SLICE_LENGTH]>(*(arr + START));
    }

    /// length of an array
    template <typename T, const size_t LENGTH>
    inline constexpr auto length(T (&)[LENGTH]) throw() -> size_t {
        return LENGTH;
    }

    /// size of an array
    template <typename T, const size_t LENGTH>
    inline constexpr auto size(T (&)[LENGTH]) throw() -> size_t {
        return sizeof(T) * LENGTH;
    }

}}

#endif /* XY_ARRAY_HPP_ */
