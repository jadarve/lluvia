/**
@file       ComputeNode.h
@brief      ComputeNode class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_CONTAINER_NODE_H_
#define LLUVIA_CORE_CONTAINER_NODE_H_

#include "lluvia/core/Node.h"

#include <map>
#include <memory>
#include <string>


namespace ll {

class CommandBuffer;
class Session;


class ContainerNode : public Node {

public:

    ll::NodeType getType() const noexcept override;

    std::shared_ptr<ll::Object> getPort(const std::string& name) const noexcept override = 0;

    void bind(const std::string& name, const std::shared_ptr<ll::Object>& obj) override = 0;

    void record(ll::CommandBuffer& commandBuffer) const override = 0;

protected:
    void onInit() override = 0;

    // Shared pointer to the session this node was created from
    // This will keep the session alive until this or any other node is deleted.
    std::shared_ptr<const ll::Session>            m_session;
};

} // namespace ll

#endif // LLUVIA_CORE_CONTAINER_NODE_H_
