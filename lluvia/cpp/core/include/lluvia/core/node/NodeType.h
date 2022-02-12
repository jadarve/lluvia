/**
@file       NodeType.h
@brief      NodeType enum.
@copyright  2022, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NODE_NODE_TYPE_H_
#define LLUVIA_CORE_NODE_NODE_TYPE_H_

#include "lluvia/core/enums/enums.h"
#include "lluvia/core/vulkan/vulkan.hpp"


namespace ll
{

/**
@brief      Class for node type.
*/
enum class NodeType : ll::enum_t
{
    Compute = 0,  /**<  value for ll::ComputeNode type. */
    Container = 1 /**<  value for ll::ContainerNode type. */
};

namespace impl
{

    /**
     @brief Node type string values used for converting ll::NodeType to std::string and vice-versa.

     @sa ll::NodeType enum values for this array.
    */
    constexpr const std::array<std::tuple<const char *, ll::NodeType>, 2> NodeTypeStrings{{std::make_tuple("Compute", ll::NodeType::Compute),
                                                                                           std::make_tuple("Container", ll::NodeType::Container)}};
} // namespace impl


} // namespace ll

#endif // LLUVIA_CORE_NODE_NODE_TYPE_H_
