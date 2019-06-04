/**
@file       Node.h
@brief      Node class and related enums.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NEW_NODE_H_
#define LLUVIA_CORE_NEW_NODE_H_

#include <cstdint>
#include <string>
#include <memory>

#include <vulkan/vulkan.hpp>

namespace ll {

class Object;

namespace impl {

class Interpreter;

} // namespace impl


namespace news {

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

    void setState(const ll::news::NodeState tState);
    ll::news::NodeState getState() const noexcept;

    void addParameter(const std::string& name, uint32_t p);

    virtual ll::news::NodeType getType() const noexcept = 0;

    virtual void link(const std::string& name, const std::shared_ptr<ll::Object>& obj) = 0;

    virtual void record(const vk::CommandBuffer& commandBuffer) const = 0;

protected:
    virtual void onInit() = 0;

    std::shared_ptr<ll::impl::Interpreter> interpreter;

private:
    
    ll::news::NodeState state {ll::news::NodeState::Created};

};

} // namespace news
} // namespace ll

#endif // LLUVIA_CORE_NEW_NODE_H_
