/**
@file       Node.h
@brief      Node class and related enums.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NODE_H_
#define LLUVIA_CORE_NODE_H_

#include <cstdint>
#include <string>
#include <memory>

#include <vulkan/vulkan.hpp>

namespace ll {

class Object;


enum class NodeState : uint32_t {
    Created = 0,
    Init = 1
};


enum class NodeType : uint32_t {
    Compute = 0,
    Container = 1,
    Custom = 2
};


class Node {

public:
    virtual ~Node() = default;

    void setState(const ll::NodeState tState);
    ll::NodeState getState() const noexcept;

    void addParameter(const std::string& name, uint32_t p);

    virtual ll::NodeType getType() const noexcept = 0;

    virtual void link(const std::string& name, const std::shared_ptr<ll::Object>& obj) = 0;

    virtual void record(const vk::CommandBuffer& commandBuffer) const = 0;

protected:
    // FIXME: what should I provide to the node to initialize?
    virtual void onInit() = 0;

    // std::shared_ptr<ll::impl::Interpreter> interpreter;

private:
    
    ll::NodeState state {ll::NodeState::Created};

};

} // namespace ll

#endif // LLUVIA_CORE_NODE_H_
