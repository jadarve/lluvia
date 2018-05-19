/**
@file       ComputeNode.h
@brief      ComputeNode class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_COMPUTE_NODE_H_
#define LLUVIA_CORE_COMPUTE_NODE_H_

#include "lluvia/core/ComputeNodeDescriptor.h"

#include <cstdint>
#include <memory>
#include <string>

#include <vulkan/vulkan.hpp>

namespace ll {

class Buffer;
class Image;
class ImageView;
class Object;
class Program;
class Visitor;


/**
@brief      Class representing compute nodes.
*/
class ComputeNode {

public:
    ComputeNode()                                     = delete;
    ComputeNode(const ComputeNode& node)              = delete;
    ComputeNode(ComputeNode&& node)                   = delete;


    /**
    @brief      Constructs the object.
    
    @param[in]  device      The Vulkan device where this node will run.
    @param[in]  descriptor  The descriptor. A copy of this descriptor is kept within this object.
                            So this one can be modified after the compute node is constructed.
    */
    ComputeNode(const vk::Device& device, const ll::ComputeNodeDescriptor& descriptor);

    ~ComputeNode();

    ComputeNode& operator = (const ComputeNode& node) = delete;
    ComputeNode& operator = (ComputeNode&& node)      = delete;


    /**
    @brief      Gets the function name within the shader module this node runs.
    
    @return     The function name.
    */
    std::string getFunctionName() const noexcept;


    /**
    @brief      Gets the program object associated to this node.
    
    @return     The program.
    */
    std::shared_ptr<ll::Program> getProgram() const noexcept;


    /**
    @brief      Gets the local group size in X dimension.

    See ll::ComputeNodeDescriptor::getLocalX for more information.
    
    @return     The local group size in X.
    */
    uint32_t getLocalX() const noexcept;


    /**
    @brief      Gets the local group size in Y dimension.

    See ll::ComputeNodeDescriptor::getLocalY for more information.
    
    @return     The local group size in Y.
    */
    uint32_t getLocalY() const noexcept;


    /**
    @brief      Gets the local group size in Z dimension.

    See ll::ComputeNodeDescriptor::getLocalZ for more information.
    
    @return     The local group size in Z.
    */
    uint32_t getLocalZ() const noexcept;


    /**
    @brief      Gets the grid size in X dimension.

    See ll::ComputeNodeDescriptor::getGridX for more information.
    
    @return     The grid size in X.
    */
    uint32_t getGridX() const noexcept;


    /**
    @brief      Gets the grid size in Y dimension.

    See ll::ComputeNodeDescriptor::getGridY for more information.
    
    @return     The grid size in Y.
    */
    uint32_t getGridY() const noexcept;


    /**
    @brief      Gets the grid size in Z dimension.

    See ll::ComputeNodeDescriptor::getGridZ for more information.
    
    @return     The grid size in Z.
    */
    uint32_t getGridZ() const noexcept;


    /**
    @brief      Gets the parameter count for this node.
    
    @return     The parameter count.
    */
    size_t getParameterCount() const noexcept;


    /**
    @brief      Returns the object associated to parameter \p index.
    
    @param[in]  index  The index of this parameter.
                       It must be less than the value returned by ll::ComputeNode::getParameterCount.
    
    @return     The parameter.
    */
    std::shared_ptr<ll::Object> getParameter(size_t index) const noexcept;


    /**
    @brief      Binds a ll::Object as parameter \p index for this node.
    
    @param[in]  index  The parameter index.
                       It must be less than the value returned by ll::ComputeNode::getParameterCount.
    @param[in]  obj    The object to bind.

    @throws     std::system_error if \p obj cannot be mapped as a parameter at position \p index.
    */
    void bind(uint32_t index, const std::shared_ptr<ll::Object>& obj);


    /**
    @brief      Records the operations required to run this compute node in a Vulkan command buffer.

    This method is called by ll::CommandBuffer objects when they are called as
    
    @param[in]  commandBuffer  The command buffer.
    */
    void record(const vk::CommandBuffer& commandBuffer) const;


    /**
    @brief      Accepts a visitor to this compute node.
    
    @param      visitor  The visitor
    */
    void accept(ll::Visitor* visitor);

private:
    void bindBuffer(uint32_t index, const std::shared_ptr<ll::Buffer>& buffer);
    void bindImageView(uint32_t index, const std::shared_ptr<ll::ImageView>& imageView);

    vk::Device                          device;

    vk::DescriptorSetLayout             descriptorSetLayout;
    vk::PipelineShaderStageCreateInfo   stageInfo;

    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes;
    vk::DescriptorPoolCreateInfo        descriptorPoolCreateInfo;

    vk::PipelineLayout                  pipelineLayout;
    vk::Pipeline                        pipeline;

    vk::DescriptorSet                   descriptorSet;
    vk::DescriptorPool                  descriptorPool;

    ll::ComputeNodeDescriptor           descriptor;

    // specialization constants
    // vk::SpecializationInfo specializationInfo;
    // std::vector<vk::SpecializationMapEntry> specializationMapEntries;
    // // uint32_t local_x {1};

    std::vector<std::shared_ptr<ll::Object>> objects;
};


} // namespace ll

#endif /* LLUVIA_CORE_COMPUTE_NODE_H_ */
