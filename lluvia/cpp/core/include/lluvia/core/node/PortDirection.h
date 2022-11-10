/**
@file       PortDirection.h
@brief      PortDirection enum.
@copyright  2022, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NODE_PORT_DIRECTION_H_
#define LLUVIA_CORE_NODE_PORT_DIRECTION_H_

#include "lluvia/core/enums/enums.h"
#include "lluvia/core/vulkan/vulkan.hpp"

namespace ll {

/**
@brief      Class for port direction.
*/
enum class PortDirection : ll::enum_t {
    In = 0, /**< The port is an input to this node. */
    Out = 1 /**< The port is an output to this node. */
};

namespace impl {

    /**
     @brief Port direction string values used for converting ll::PortDirection to std::string and vice-versa.

     @sa ll::PortDirection enum values for this array.
    */
    constexpr const std::array<std::tuple<const char*, ll::PortDirection>, 2> PortDirectionStrings { { std::make_tuple("In", ll::PortDirection::In),
        std::make_tuple("Out", ll::PortDirection::Out) } };

} // namespace impl

} // namespace ll

#endif // LLUVIA_CORE_NODE_PORT_DIRECTION_H_
