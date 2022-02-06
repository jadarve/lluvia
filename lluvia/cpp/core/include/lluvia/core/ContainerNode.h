/**
@file       ComputeNode.h
@brief      ComputeNode class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_CONTAINER_NODE_H_
#define LLUVIA_CORE_CONTAINER_NODE_H_

#include "lluvia/core/node/Node.h"
#include "lluvia/core/ContainerNodeDescriptor.h"

#include <map>
#include <memory>
#include <string>
#include <vector>


namespace ll {

class CommandBuffer;
class Interpreter;


class ContainerNode : public Node, public std::enable_shared_from_this<ll::ContainerNode> {

public:
    ContainerNode(const std::weak_ptr<ll::Interpreter>& interpreter);
    ContainerNode(const std::weak_ptr<ll::Interpreter>& interpreter,
                  const ll::ContainerNodeDescriptor& descriptor);

    ContainerNode(const ll::ContainerNode&)                  = delete;
    ContainerNode(ll::ContainerNode&&)                       = delete;

    virtual ~ContainerNode()                                 = default;

    ll::ContainerNode& operator = (const ll::ContainerNode&) = delete;
    ll::ContainerNode& operator = (ll::ContainerNode&&)      = delete;


    ll::NodeType getType() const noexcept override;

    const ll::ContainerNodeDescriptor& getDescriptor() const noexcept;

    std::shared_ptr<ll::Object> getPort(const std::string& name) const override;

    void bind(const std::string& name, const std::shared_ptr<ll::Object>& obj) override;

    void bindNode(const std::string& name, const std::shared_ptr<ll::Node>& node);

    std::shared_ptr<ll::Node> getNode(const std::string& name) const;

    void record(ll::CommandBuffer& commandBuffer) const override;

    void setParameter(const std::string &name, const ll::Parameter &value) override;

    const ll::Parameter &getParameter(const std::string &name) const override;

protected:
    void onInit() override;

    ll::ContainerNodeDescriptor                        m_descriptor;

    std::map<std::string, std::shared_ptr<ll::Object>> m_objects;
    std::map<std::string, std::shared_ptr<ll::Node>>   m_nodes;

    std::weak_ptr<ll::Interpreter>                     m_interpreter;
};

} // namespace ll

#endif // LLUVIA_CORE_CONTAINER_NODE_H_
