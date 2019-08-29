/**
@file       ComputeNode.h
@brief      ComputeNode class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_CONTAINER_NODE_H_
#define LLUVIA_CORE_CONTAINER_NODE_H_

#include "lluvia/core/Node.h"
#include "lluvia/core/ContainerNodeDescriptor.h"

#include <map>
#include <memory>
#include <string>
#include <vector>


namespace ll {

class CommandBuffer;
class Session;


class ContainerNode : public Node, public std::enable_shared_from_this<ll::ContainerNode> {

public:
    ContainerNode(const std::shared_ptr<const ll::Session>& session);
    ContainerNode(const std::shared_ptr<const ll::Session>& session,
                  const ll::ContainerNodeDescriptor& descriptor);

    ContainerNode(const ll::ContainerNode&)                  = delete;
    ContainerNode(ll::ContainerNode&&)                       = delete;

    virtual ~ContainerNode()                                 = default;

    ll::ContainerNode& operator = (const ll::ContainerNode&) = delete;
    ll::ContainerNode& operator = (ll::ContainerNode&&)      = delete;


    ll::NodeType getType() const noexcept override {
        return ll::NodeType::Container;
    }

    const ll::ContainerNodeDescriptor& getDescriptor() const noexcept;

    std::shared_ptr<ll::Object> getPort(const std::string& name) const noexcept override;

    void bind(const std::string& name, const std::shared_ptr<ll::Object>& obj) override;

    void bindNode(const std::string& name, const std::shared_ptr<ll::Node>& node);

    std::shared_ptr<ll::Node> getNode(const std::string& name) const;

    void record(ll::CommandBuffer& commandBuffer) const override;

protected:
    void onInit() override;

    ll::ContainerNodeDescriptor                        m_descriptor;

    std::map<std::string, std::shared_ptr<ll::Object>> m_objects;
    std::map<std::string, std::shared_ptr<ll::Node>>   m_nodes;

    // Shared pointer to the session this node was created from
    // This will keep the session alive until this or any other node is deleted.
    std::shared_ptr<const ll::Session>                 m_session;
};

} // namespace ll

#endif // LLUVIA_CORE_CONTAINER_NODE_H_
