/**
@file       FloatPrecisio.h
@brief      FloatPrecisio enum.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_FLOAT_PRECISION_H_
#define LLUVIA_CORE_FLOAT_PRECISION_H_

#include "lluvia/core/enums/enums.h"

namespace ll {

/**
@brief      Floating point precision.
*/
enum class FloatPrecision : ll::enum_t {
    FP16 = 0,
    FP32 = 1,
    FP64 = 2,
};

namespace impl {

    constexpr const std::array<std::tuple<const char*, ll::FloatPrecision>, 3> FloatPrecisionStrings {{
        std::make_tuple("FP16", ll::FloatPrecision::FP16),
        std::make_tuple("FP32", ll::FloatPrecision::FP32),
        std::make_tuple("FP64", ll::FloatPrecision::FP64),
    }};

} // namespace impl

} // namespace ll

#endif // LLUVIA_CORE_FLOAT_PRECISION_H_
