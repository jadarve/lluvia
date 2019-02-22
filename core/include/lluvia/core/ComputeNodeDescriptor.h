/**
@file       ComputeNodeDescriptor.h
@brief      ComputeNodeDescriptor class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_COMPUTE_DESCRIPTOR_NODE_H_
#define LLUVIA_CORE_COMPUTE_DESCRIPTOR_NODE_H_

#include "lluvia/core/impl/enum_utils.h"
#include "lluvia/core/types.h"

#include <array>
#include <memory>
#include <string>
#include <tuple>

#include <vulkan/vulkan.hpp>


namespace ll {

class ComputeNode;
class Program;


/**
@brief      Compute node parameter types.


@sa ll::impl::ParameterTypeStrings string values for this enum.
*/
enum class ParameterType : uint32_t {
    Buffer           = 0,   /**< value for ll::Buffer parameter type. */
    ImageView        = 1,   /**< value for ll::ImageView without pixel sampler.*/
    SampledImageView = 2,   /**< value for ll::ImageView objects coupled with a pixel sampler. */
};


namespace impl {

    /**
    @brief Parameter type string values used for converting ll::ParameterType to std::string and vice-versa.

    @sa ll::ParameterType enum values for this array.
    */
    constexpr const std::array<std::tuple<const char*, ll::ParameterType>, 3> ParameterTypeStrings {{
        std::make_tuple("Buffer"           , ll::ParameterType::Buffer),
        std::make_tuple("ImageView"        , ll::ParameterType::ImageView),
        std::make_tuple("SampledImageView" , ll::ParameterType::SampledImageView),
    }};

} // namespace impl


/**
@brief      Converts from ll::ParameterType enum value to string.

@param[in]  value

@tparam     T          function return type. Defaults to std::string.

@return     Returns the corresponding std::string in ll::impl::ParameterTypeStrings for the enum value.
*/
template<typename T = std::string>
inline T parameterTypeToString(ll::ParameterType&& value) noexcept {
    return ll::impl::enumToString<ll::ParameterType, ll::impl::ParameterTypeStrings.size(), impl::ParameterTypeStrings>(std::forward<ll::ParameterType>(value));
}


/**
@brief      Converts from ll::ParamterType enum to Vulkan DescriptorType.

@param[in]  param  The parameter.

@return     The corresponding Vulkan descriptor type for \p param.
*/
vk::DescriptorType parameterTypeToVkDescriptorType(const ll::ParameterType& param);


/**
@brief      Converts from Vulkan DescriptorType to ll::ParameterType enum.

@param[in]  vkDescType  The Vulkan description type.

@return     The corresponding ll::ParameterType.

@throws     std::system_error if there is no associated ll::ParameterType value for \p vkDescType.
                              Using the values returned by ll::parameterTypeToVkDescriptorType is guaranteed
                              to not throw exception.
*/
ll::ParameterType vkDescriptorTypeToParameterType(const vk::DescriptorType& vkDescType);


/**
@brief      Converts from a string-like object to ll::ParameterType enum.

@param[in]  stringValue  string-like parameter. String literals and `std::string` objects are allowed.

@tparam     T            \p stringValue type. \p T must satisfies `std::is_convertible<T, std::string>()`

@return     ll::ParameterType value corresponding to stringValue
 
@throws std::out_of_range if \p stringValue is not found in ll::impl::ObjectTypeStrings.
*/
template<typename T>
inline ll::ParameterType stringToParameterType(T&& stringValue) {
    return impl::stringToEnum<ll::ParameterType, T, ll::impl::ParameterTypeStrings.size(), impl::ParameterTypeStrings>(std::forward<T>(stringValue));
}


/**
@brief      Class for describing a compute node node.

Descriptors are used to construct ll::ComputeNode objects.
*/
class ComputeNodeDescriptor {

public:
    ComputeNodeDescriptor()                                                     = default;
    ComputeNodeDescriptor(const ComputeNodeDescriptor& descriptor)              = default;
    ComputeNodeDescriptor(ComputeNodeDescriptor&& descriptor)                   = default;

    ~ComputeNodeDescriptor()                                                    = default;
    
    ComputeNodeDescriptor& operator = (const ComputeNodeDescriptor& descriptor) = default;
    ComputeNodeDescriptor& operator = (ComputeNodeDescriptor&& descriptor)      = default;


    /**
    @brief      Sets the program object.
    
    @param[in]  program  The program.
    
    @return     A reference to this object.
    */
    ComputeNodeDescriptor& setProgram(const std::shared_ptr<ll::Program>& program);


    /**
    @brief      Sets the function name within the program object to run in the ll::ComputeNode.
    
    @param[in]  name  The function name.
    
    @return     A reference to this object.
    */
    ComputeNodeDescriptor& setFunctionName(const std::string& name);


    /**
    @brief      Adds a parameter to the descriptor.

    Parameters are stored in a vector. Each call to this function
    adds \p param at the end of such vector.
    
    @param[in]  param  The parameter.
    
    @return     A reference to this object.
    */
    ComputeNodeDescriptor& addParameter(const ll::ParameterType param);


    /**
    @brief      Gets the parameter count.
    
    @return     The parameter count.
    */
    size_t getParameterCount() const noexcept;


    /**
    @brief      Gets the parameter type at index \p i.
    
    @param[in]  i     index. It must be greater than 0 and less than ll::ComputeNodeDescriptor::getParameterCount.
    
    @return     The parameter type at \p i.

    @throws     std::out_of_range if \p i is not between correct range.
    */
    ll::ParameterType getParameterTypeAt(const size_t& i) const;


    /**
    @brief      Sets the grid size in the X axis.

    The grid size defines the number of local groups to be run
    during the execution of a compute node shader program.

    Parameter \p x corresponds to the `groupCountX` parameter in
    vkCmdDispatch. See @VULKAN_DOC#vkCmdDispatch
    for more information.
    
    @param[in]  x   The grid size. It must be greater than zero.
    
    @return     A reference to this object.
    */
    ComputeNodeDescriptor& setGridX(const uint32_t x) noexcept;


    /**
    @brief      Sets the grid size in the Y axis.

    The grid size defines the number of local groups to be run
    during the execution of a compute node shader program.

    Parameter \p y corresponds to the `groupCountY` parameter in
    vkCmdDispatch. See @VULKAN_DOC#vkCmdDispatch
    for more information.
    
    @param[in]  y   The grid size. It must be greater than zero.
    
    @return     A reference to this object.
    */
    ComputeNodeDescriptor& setGridY(const uint32_t y) noexcept;


    /**
    @brief      Sets the grid size in the Z axis.

    The grid size defines the number of local groups to be run
    during the execution of a compute node shader program.

    Parameter \p z corresponds to the `groupCountZ` parameter in
    vkCmdDispatch. See @VULKAN_DOC#vkCmdDispatch
    for more information.
    
    @param[in]  z   The grid size. It must be greater than zero.
    
    @return     A reference to this object.
    */
    ComputeNodeDescriptor& setGridZ(const uint32_t z) noexcept;

    /**
    @brief      Sets the grid shape.

    The grid size defines the number of local groups to be run
    during the execution of a compute node shader program.

    Parameter \p z corresponds to the `groupCountZ` parameter in
    vkCmdDispatch. See @VULKAN_DOC#vkCmdDispatch
    for more information.
    
    @param[in]  shape  The grid shape. Each XYZ component must be greater than zero.
    
    @return     A reference to this object.
    */
    ComputeNodeDescriptor& setGridShape(const ll::vec3ui& shape) noexcept;


    ComputeNodeDescriptor& configureGridShape(const ll::vec3ui& globalShape) noexcept;


    /**
    @brief      Sets the local group size in the X axis.
    
    @param[in]  x     The local group size. It must be greater than zero.
    
    @return     A reference to this object.
    */
    ComputeNodeDescriptor& setLocalX(const uint32_t x) noexcept;


    /**
    @brief      Sets the local group size in the Y axis.
    
    @param[in]  y     The local group size. It must be greater than zero.
    
    @return     A reference to this object.
    */
    ComputeNodeDescriptor& setLocalY(const uint32_t y) noexcept;


    /**
    @brief      Sets the local group size in the Z axis.
    
    @param[in]  z     The local group size. It must be greater than zero.
    
    @return     A reference to this object.
    */
    ComputeNodeDescriptor& setLocalZ(const uint32_t z) noexcept;


    /**
    @brief      Sets the local group shape.
    
    @param[in]  shape  The shape. Each XYZ component must be greater than zero.
    
    @return     A reference to this object.
    */
    ComputeNodeDescriptor& setLocalShape(const ll::vec3ui& shape) noexcept;

    /**
    @brief      Gets the Vulkan descriptor pool sizes needed for this compute node.

    The returned vector contains the non-zero pool sizes required for the parameters
    of this compute node.
    
    @return     The descriptor pool sizes.
    */
    std::vector<vk::DescriptorPoolSize> getDescriptorPoolSizes() const noexcept;


    /**
    @brief      Gets the program associated to this compute node.
    
    @return     The program.
    */
    std::shared_ptr<ll::Program> getProgram() const noexcept;


    /**
    @brief      Gets the function name within the ll::Program object used by this node.
    
    @return     The function name.
    */
    std::string getFunctionName() const noexcept;
    

    /**
    @brief      Gets the Vulkan parameter bindings associated to this compute node.

    See @VULKAN_DOC#VkDescriptorSetLayoutBinding for more information.
    
    @return     The parameter bindings.
    */
    std::vector<vk::DescriptorSetLayoutBinding> getParameterBindings() const noexcept;

    /**
    @brief      Gets the grid shape.
    
    @return     The grid shape.
    */
    ll::vec3ui getGridShape()  const noexcept;

    /**
    @brief      Gets the local group shape.
    
    @return     The local group shape.
    */
    ll::vec3ui getLocalShape() const noexcept;

    uint32_t getGridX() const noexcept;
    uint32_t getGridY() const noexcept;
    uint32_t getGridZ() const noexcept;

    uint32_t getLocalX() const noexcept;
    uint32_t getLocalY() const noexcept;
    uint32_t getLocalZ() const noexcept;


    /**
    @brief      Returns the number of storage buffers used by the compute node.

    This is the number of times ll::ComputeNodeDescriptor::addParameter is called with ll::ParameterType::Buffer.
    
    @return     The storage buffer count.
    */
    uint32_t getStorageBufferCount()        const noexcept;


    /**
    @brief      Returns the number of storage images used by the compute node.

    This is the number of times ll::ComputeNodeDescriptor::addParameter is called with ll::ParameterType::ImageView.
    
    @return     The storage image count.
    */
    uint32_t getStorageImageViewCount()           const noexcept;


    /**
    @brief      Returns the number of storage images used by the compute node.

    This is the number of times ll::ComputeNodeDescriptor::addParameter is called with ll::ParameterType::SampledImageView.
    
    @return     The sampled image count.
    */
    uint32_t getSampledImageViewCount() const noexcept;


private:
    uint32_t countDescriptorType(const vk::DescriptorType type) const noexcept;

    std::shared_ptr<ll::Program>                program;
    std::string                                 functionName;
    std::vector<vk::DescriptorSetLayoutBinding> parameterBindings;

    // local and global work group
    ll::vec3ui localShape {1, 1, 1};
    ll::vec3ui gridShape  {1, 1, 1};


friend class ComputeNode;
};


} // namespace ll

#endif /* LLUVIA_CORE_COMPUTE_DESCRIPTOR_NODE_H_ */
