/**
@file       Node.h
@brief      Node class and related enums.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NODE_H_
#define LLUVIA_CORE_NODE_H_

#include "lluvia/core/impl/enum_utils.h"

#include <cstdint>
#include <string>
#include <memory>

#include <vulkan/vulkan.hpp>

namespace ll {

class Object;


/**
@brief      Class for node state.

@sa ll::Node
*/
enum class NodeState : uint32_t {
    Created = 0,
    Init    = 1
};


/**
@brief      Class for node type.
*/
enum class NodeType : uint32_t {
    Compute   = 0, /**<  value for ll::ComputeNode type. */
    Container = 1  /**<  value for ll::ContainerNode type. */
};


/**
@brief      Class for port direction.
*/
enum class PortDirection : uint32_t {
    In  = 0, /**< The port is an input to this node. */
    Out = 1  /**< The port is an output to this node. */
};


/**
@brief      Port types.

@sa ll::impl::ParameterTypeStrings string values for this enum.
*/
enum class PortType : uint32_t {
    Buffer           = 0,   /**< value for ll::Buffer type. */
    ImageView        = 1,   /**< value for ll::ImageView without pixel sampler.*/
    SampledImageView = 2,   /**< value for ll::ImageView objects coupled with a pixel sampler. */
};


namespace impl {

    /**
     @brief Node state string values used for converting ll::NodeState to std::string and vice-versa.

     @sa ll::NodeState enum values for this array.
     */
    constexpr const std::array<std::tuple<const char*, ll::NodeState>, 2> NodeStateStrings {{
        std::make_tuple("Created" , ll::NodeState::Created),
        std::make_tuple("Init"    , ll::NodeState::Init)
    }};


    /**
     @brief Port direction string values used for converting ll::PortDirection to std::string and vice-versa.

     @sa ll::PortDirection enum values for this array.
     */
    constexpr const std::array<std::tuple<const char*, ll::PortDirection>, 2> PortDirectionStrings {{
        std::make_tuple("In"  , ll::PortDirection::In),
        std::make_tuple("Out" , ll::PortDirection::Out)
    }};

    /**
    @brief Port type string values used for converting ll::PortType to std::string and vice-versa.

    @sa ll::PortType enum values for this array.
    */
    constexpr const std::array<std::tuple<const char*, ll::PortType>, 3> PortTypeStrings {{
        std::make_tuple("Buffer"           , ll::PortType::Buffer),
        std::make_tuple("ImageView"        , ll::PortType::ImageView),
        std::make_tuple("SampledImageView" , ll::PortType::SampledImageView),
    }};

} // namespace impl


/**
@brief      Converts from ll::PortType enum value to string.

@param[in]  value

@tparam     T          function return type. Defaults to std::string.

@return     Returns the corresponding std::string in ll::impl::PortTypeStrings for the enum value.
*/
template<typename T = std::string>
inline T portTypeToString(ll::PortType&& value) noexcept {
    return ll::impl::enumToString<ll::PortType, ll::impl::PortTypeStrings.size(), impl::PortTypeStrings>(std::forward<ll::PortType>(value));
}


/**
@brief      Converts from ll::PortType enum to Vulkan DescriptorType.

@param[in]  param  The parameter.

@return     The corresponding Vulkan descriptor type for \p param.
*/
vk::DescriptorType portTypeToVkDescriptorType(const ll::PortType& param);


/**
@brief      Converts from Vulkan DescriptorType to ll::PortType enum.

@param[in]  vkDescType  The Vulkan description type.

@return     The corresponding ll::PortType.

@throws     std::system_error if there is no associated ll::PortType value for \p vkDescType.
                              Using the values returned by ll::portTypeToVkDescriptorType is guaranteed
                              to not throw exception.
*/
ll::PortType vkDescriptorTypeToPortType(const vk::DescriptorType& vkDescType);


/**
@brief      Converts from a string-like object to ll::PortType enum.

@param[in]  stringValue  string-like parameter. String literals and `std::string` objects are allowed.

@tparam     T            \p stringValue type. \p T must satisfies `std::is_convertible<T, std::string>()`

@return     ll::PortType value corresponding to stringValue
 
@throws std::out_of_range if \p stringValue is not found in ll::impl::ObjectTypeStrings.
*/
template<typename T>
inline ll::PortType stringToPortType(T&& stringValue) {
    return impl::stringToEnum<ll::PortType, T, ll::impl::PortTypeStrings.size(), impl::PortTypeStrings>(std::forward<T>(stringValue));
}


// using PortDescriptor = struct {
struct PortDescriptor {

    PortDescriptor() {}
    PortDescriptor(uint32_t pBinding,
                   const std::string& pName,
                   ll::PortDirection pDirection,
                   ll::PortType pType) :
        binding {pBinding},
        name {pName},
        direction {pDirection},
        type {pType} {}

    /**
    Binding number within shader program.
    */
    uint32_t binding                {0};
    std::string name                {};
    ll::PortDirection direction     {ll::PortDirection::In};
    ll::PortType type               {ll::PortType::Buffer};
};


class Node {

public:
    virtual ~Node() = default;

    void setState(const ll::NodeState tState);
    ll::NodeState getState() const noexcept;

    void init();

    /**
    @brief      Gets the node type.
    
    @return     The type.

    @sa ll::NodeType
    */
    virtual ll::NodeType getType() const noexcept = 0;

    /**
    @brief      Returns the object associated to a port \p name
                
    @param[in]  name  The name of the port.
    
    @return     The port object or nullptr if the port does not exists.
    */
    virtual std::shared_ptr<ll::Object> getPort(const std::string& name) const noexcept = 0;

    /**
    @brief      Binds a ll::Object as port \p index for this node.
    
    @param[in]  name   The port name.
    @param[in]  obj    The object to bind.

    @throws     std::system_error if \p obj cannot be mapped as a port at position \p index.
    */
    virtual void bind(const std::string& name, const std::shared_ptr<ll::Object>& obj) = 0;

    /**
    @brief      Records the operations required to run this node in a Vulkan command buffer.

    This method is called by ll::CommandBuffer objects when they are called as
    
    @param[in]  commandBuffer  The Vulkan command buffer.

    @throws     std::system_error with corresponding error code and message.
    */
    virtual void record(const vk::CommandBuffer& commandBuffer) const = 0;

protected:
    virtual void onInit() = 0;

private:
    ll::NodeState state {ll::NodeState::Created};

};

} // namespace ll

#endif // LLUVIA_CORE_NODE_H_
