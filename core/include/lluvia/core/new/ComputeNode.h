/**
@file       ComputeNode.h
@brief      ComputeNode class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NEW_COMPUTE_NODE_H_
#define LLUVIA_CORE_NEW_COMPUTE_NODE_H_

#include "lluvia/core/ComputeNodeDescriptor.h"
#include "lluvia/core/new/ComputeNodeDescriptor.h"
#include "lluvia/core/new/Node.h"

#include <cstdint>
#include <memory>
#include <string>
#include <map>

#include <vulkan/vulkan.hpp>

namespace ll {

class Object;

namespace news {

/**
@brief      Class representing compute nodes.
*/
class ComputeNode : public news::Node {

public:
    ComputeNode()                                  = delete;
    ComputeNode(const ComputeNode& node)           = delete;
    ComputeNode(ComputeNode&& node)                = delete;
    ComputeNode(const ll::news::ComputeNodeDescriptor& tDescriptor);

    virtual ~ComputeNode();

    ComputeNode& operator = (const ComputeNode& node) = delete;
    ComputeNode& operator = (ComputeNode&& node)      = delete;

    ll::news::NodeType getType() const noexcept override;

    void link(const std::string& name, const std::shared_ptr<ll::Object>& obj) override;


protected:
    void onInit() override;

private:
    ll::news::ComputeNodeDescriptor descriptor;
    std::map<const std::string, std::shared_ptr<ll::Object>> ports;
};

} // namespace news
} // namespace ll

#endif // LLUVIA_CORE_NEW_COMPUTE_NODE_H_
