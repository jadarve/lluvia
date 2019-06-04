/**
@file       ComputeNode.cpp
@brief      ComputeNode class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/new/ComputeNode.h"

#include "lluvia/core/Object.h"


namespace ll {
namespace news {

ComputeNode::ComputeNode(const ll::news::ComputeNodeDescriptor& tDescriptor) :
    descriptor {tDescriptor} {

}

ComputeNode::~ComputeNode() {

}

ll::news::NodeType ComputeNode::getType() const noexcept {
    return NodeType::Compute;
}

void ComputeNode::onInit() {

    // verify that all inputs have been linked. Throw error if not.
    // populate the interpreter with the input descriptors
    // populate the interpreter with the output descriptors (uninitialized)
    // run the init script
    // collect the initialized output descriptors
    // create output objects
    // create compute pipeline
}

void ComputeNode::link(const std::string& name, const std::shared_ptr<ll::Object>& obj) {

    if (obj == nullptr) {
        // TODO: throw error: null port
    }

    // only the inputs can be linked
    auto pIter = descriptor.portDescriptors.find(name);
    if (pIter != descriptor.portDescriptors.end()) {
        const auto pDesc = pIter->second;

        if (pDesc.direction != ll::news::PortDirection::IN) {
            // TODO: throw error: port must be input
        }

        ports[name] = obj;
    }

    // TODO: throw error: port does not exsists

}

} // namespace news
} // namespace ll
