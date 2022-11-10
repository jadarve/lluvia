/**
@file       NodeState.h
@brief      NodeState enum.
@copyright  2022, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NODE_NODE_STATE_H_
#define LLUVIA_CORE_NODE_NODE_STATE_H_

#include "lluvia/core/enums/enums.h"
#include "lluvia/core/vulkan/vulkan.hpp"

namespace ll {

/**
@brief      Class for node state.

@sa ll::Node
*/
enum class NodeState : ll::enum_t {
    Created = 0,
    Init = 1
};

namespace impl {

    /**
     @brief Node state string values used for converting ll::NodeState to std::string and vice-versa.

     @sa ll::NodeState enum values for this array.
     */
    constexpr const std::array<std::tuple<const char*, ll::NodeState>, 2> NodeStateStrings { { std::make_tuple("Created", ll::NodeState::Created),
        std::make_tuple("Init", ll::NodeState::Init) } };

} // namespace impl

} // namespace ll

#endif // LLUVIA_CORE_NODE_NODE_STATE_H_
