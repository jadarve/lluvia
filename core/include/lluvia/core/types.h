/**
@file       types.h
@brief      Common types
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_TYPES_H_
#define LLUVIA_CORE_TYPES_H_

#include <cstdint>
#include <stdexcept>
#include "lluvia/core/error.h"

namespace ll {

/**
@brief      3-vector

@tparam     T     component type.
*/
template<typename T>
struct vec3 {
    T x;
    T y;
    T z;
};

/**
uint32_t 3-vector.
*/
using vec3ui = vec3<uint32_t>;

} // namespace ll

#endif /* LLUVIA_CORE_TYPES_H_ */
