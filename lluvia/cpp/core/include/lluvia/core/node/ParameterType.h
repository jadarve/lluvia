/**
@file       ParameterType.h
@brief      ParameterType enum.
@copyright  2022, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NODE_PARAMETER_TYPE_H_
#define LLUVIA_CORE_NODE_PARAMETER_TYPE_H_

#include "lluvia/core/enums/enums.h"

namespace ll {

enum class ParameterType : ll::enum_t {
    Int    = 0,
    Float  = 1,
    String = 2,
};

namespace impl {

    /**
     @brief String values used for converting ll::ParameterType to std::string and vice-versa.

     @sa ll::ParameterType enum values for this array.
     */
    constexpr const std::array<std::tuple<const char*, ll::ParameterType>, 3> ParameterTypeStrings {{std::make_tuple("Int", ll::ParameterType::Int),
        std::make_tuple("Float", ll::ParameterType::Float),
        std::make_tuple("String", ll::ParameterType::String)}};

} // namespace impl
} // namespace ll

#endif // LLUVIA_CORE_NODE_PARAMETER_TYPE_H_
