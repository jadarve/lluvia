/**
@file       types.h
@brief      Common types
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_TYPES_H_
#define LLUVIA_CORE_TYPES_H_

#include "lluvia/core/error.h"
#include <cstdint>
#include <stdexcept>

namespace ll {

/**
@brief      3-vector

@tparam     T     component type.
*/
template <typename T>
struct vec3 {
    T x { 0 };
    T y { 0 };
    T z { 0 };

    vec3() { }

    vec3(const T& value)
        : x { value }
        , y { value }
        , z { value }
    {
    }

    vec3(const T& x_, const T& y_, const T& z_)
        : x { x_ }
        , y { y_ }
        , z { z_ }
    {
    }
};

/**
uint32_t 3-vector.
*/
using vec3ui = vec3<uint32_t>;

} // namespace ll

#endif /* LLUVIA_CORE_TYPES_H_ */
