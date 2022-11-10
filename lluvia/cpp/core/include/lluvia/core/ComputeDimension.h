/**
@file       ComputeDimention.h
@brief      ComputeDimention.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_COMPUTE_DIMENSION_H_
#define LLUVIA_CORE_COMPUTE_DIMENSION_H_

#include <array>
#include <tuple>

#include "lluvia/core/enums/enums.h"

namespace ll {

/**
@brief      Supported compute dimensions.
*/
enum class ComputeDimension : ll::enum_t {
    D1 = 1,
    D2 = 2,
    D3 = 3
};

namespace impl {

    constexpr const std::array<std::tuple<const char*, ll::ComputeDimension>, 3> ComputeDimensionStrings { { std::make_tuple("D1", ll::ComputeDimension::D1),
        std::make_tuple("D2", ll::ComputeDimension::D2),
        std::make_tuple("D3", ll::ComputeDimension::D3) } };
}

} // namespace ll

#endif /* LLUVIA_CORE_COMPUTE_DIMENSION_H_ */
