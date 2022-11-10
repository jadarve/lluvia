/**
@file       NodeBuilderDescriptor.h
@brief      NodeBuilderDescriptor class.
@copyright  2021, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NODE_NODE_BUILDER_DESCRIPTOR_H_
#define LLUVIA_CORE_NODE_NODE_BUILDER_DESCRIPTOR_H_

#include "lluvia/core/node/Node.h"

#include <string>

namespace ll {

/**
@brief      Describes the attributes of a node builder.
*/
struct NodeBuilderDescriptor {

    NodeBuilderDescriptor() { }
    NodeBuilderDescriptor(ll::NodeType pNodeType,
        const std::string& pName,
        const std::string& pSummary)
        : nodeType { pNodeType }
        , name { pName }
        , summary { pSummary }
    {
    }

    ll::NodeType nodeType;
    std::string name;

    // corresponds to the first line of the builder's docstring
    std::string summary;
};

} // namespace ll

#endif // LLUVIA_CORE_NODE_NODE_BUILDER_DESCRIPTOR_H_
