/**
@file       ContainerNode.cpp
@brief      ContainerNode class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/ContainerNode.h"

#include "lluvia/core/CommandBuffer.h"
#include "lluvia/core/Session.h"
#include "lluvia/core/Interpreter.h"

namespace ll {


ContainerNode::ContainerNode(const std::shared_ptr<ll::Session>& session) :
    m_session {session} {

}


ContainerNode::ContainerNode(const std::shared_ptr<ll::Session>& session,
              const ll::ContainerNodeDescriptor& descriptor) :
    m_descriptor {descriptor},
    m_session {session} {

}


ll::NodeType ContainerNode::getType() const noexcept {
    return ll::NodeType::Container;
}


const std::shared_ptr<ll::Session>& ContainerNode::getSession() const noexcept {
    return m_session;
}


const ll::ContainerNodeDescriptor& ContainerNode::getDescriptor() const noexcept {
    return m_descriptor;
}


std::shared_ptr<ll::Object> ContainerNode::getPort(const std::string& name) const {

    const auto it = m_objects.find(name);
    ll::throwSystemErrorIf(it == m_objects.cend(), ll::ErrorCode::KeyNotFound, "Port [" + name + "] not found.");

    return it->second;
}


void ContainerNode::bind(const std::string& name, const std::shared_ptr<ll::Object>& obj) {

    // FIXME: if name is in descriptor ports, do the proper checks
    m_objects[name] = obj;
}


void ContainerNode::bindNode(const std::string& name, const std::shared_ptr<ll::Node>& node) {
    m_nodes[name] = node;
}


std::shared_ptr<ll::Node> ContainerNode::getNode(const std::string& name) const {

    std::cout << "ContainerNode::getNode: " << name << std::endl;
    const auto it = m_nodes.find(name);
    return it == m_nodes.cend()? nullptr : it->second;
}


void ContainerNode::record(ll::CommandBuffer& commandBuffer) const {

    ll::throwSystemErrorIf(getState() != ll::NodeState::Init, ll::ErrorCode::InvalidNodeState, "node must be in Init state before calling record()");
    
    const auto builderName = m_descriptor.getBuilderName();
    if (!builderName.empty()) {

        constexpr const auto lua = R"(
            local builderName, node, cmdBuffer = ...
            local builder = ll.getNodeBuilder(builderName)
            builder.onNodeRecord(node, cmdBuffer)
        )";

        auto load = m_session->getInterpreter()->load(lua);
        load(builderName, shared_from_this(), commandBuffer);
    }
}


void ContainerNode::onInit() {

    const auto builderName = m_descriptor.getBuilderName();
    if (!builderName.empty()) {

        constexpr const auto lua = R"(
            local builderName, node = ...
            local builder = ll.getNodeBuilder(builderName)
            builder.onNodeInit(node)
        )";

        auto load = m_session->getInterpreter()->load(lua);
        load(builderName, shared_from_this());
    }
}

} // namespace ll
