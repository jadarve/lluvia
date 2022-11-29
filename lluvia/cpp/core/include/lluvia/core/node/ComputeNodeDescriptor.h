/**
@file       ComputeNodeDescriptor.h
@brief      ComputeNodeDescriptor class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NODE_COMPUTE_NODE_DESCRIPTOR_H_
#define LLUVIA_CORE_NODE_COMPUTE_NODE_DESCRIPTOR_H_

#include "lluvia/core/node/Node.h"
#include "lluvia/core/node/Parameter.h"
#include "lluvia/core/node/PushConstants.h"

#include "lluvia/core/types.h"

#include <array>
#include <map>
#include <memory>
#include <string>
#include <tuple>

namespace ll {

class ComputeNode;
class Program;

/**
@brief      Class for describing a compute node node.

Descriptors are used to construct ll::ComputeNode objects.
*/
class ComputeNodeDescriptor {

public:
    ComputeNodeDescriptor()                                        = default;
    ComputeNodeDescriptor(const ComputeNodeDescriptor& descriptor) = default;
    ComputeNodeDescriptor(ComputeNodeDescriptor&& descriptor)      = default;

    ~ComputeNodeDescriptor() = default;

    ComputeNodeDescriptor& operator=(const ComputeNodeDescriptor& descriptor) = default;
    ComputeNodeDescriptor& operator=(ComputeNodeDescriptor&& descriptor)      = default;

    /**
    @brief      Sets the program object.

    @param[in]  tProgram  The program.

    @return     A reference to this object.
    */
    ComputeNodeDescriptor& setProgram(const std::shared_ptr<ll::Program>& tProgram) noexcept;

    /**
    @brief      Sets the program object and function name.

    @param[in]  tProgram       The program.
    @param[in]  tFunctionName  The function name.

    @return     A reference to this object.

    @sa ll::ComputeNodeDescriptor::setProgram       sets the program object.
    @sa ll::ComputeNodeDescriptor::setFunctionName  sets the function name.
    */
    ComputeNodeDescriptor& setProgram(const std::shared_ptr<ll::Program>& tProgram, const std::string& tFunctionName) noexcept;

    /**
    @brief      Sets a parameter.

    @param[in]  name          The name
    @param[in]  defaultValue  The value.

    @return     A reference to this object.
    */
    ComputeNodeDescriptor& setParameter(const std::string& name, const ll::Parameter& value);

    /**
    @brief      Sets the function name within the program object to run in the ll::ComputeNode.

    @param[in]  name  The function name.

    @return     A reference to this object.
    */
    ComputeNodeDescriptor& setFunctionName(const std::string& name) noexcept;

    /**
    @brief      Sets the builder name this descriptor refers to within the Lua interpreter.

    @param[in]  name  The builder name.

    @return     A reference to this object.
    */
    ComputeNodeDescriptor& setBuilderName(const std::string& name) noexcept;

    /**
    @brief      Adds a port to the descriptor.

    @param[in]  port  The port

    @return     A reference to this object.
    */
    ComputeNodeDescriptor& addPort(const ll::PortDescriptor& port);

    /**
    @brief      Adds a list of ports to the descriptor.

    @param[in]  ports  The ports

    @return     A reference to this object.
    */
    ComputeNodeDescriptor& addPorts(const std::initializer_list<ll::PortDescriptor>& ports);

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

    /**
    @brief      Calculates the grid shape provided local and global shapes.

    The calculation is done as:

    @code
        grid.x = ceil(global.x / local.x)
        grid.y = ceil(global.y / local.y)
        grid.z = ceil(global.z / local.z)
    @endcode

    This method does not control the case of dividing by zero.

    @param[in]  globalShape  The global shape.

    @return     A reference to this object.
    */
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
    @brief      Sets the push constants for this compute node.

    @param[in]  constants  A shared pointer to ll::PushConstants. This way, the values of
                the constants can be shared among several nodes.

    @return     A reference to this object
    */
    ComputeNodeDescriptor& setPushConstants(const ll::PushConstants& constants) noexcept;

    /**
    @brief      Gets the program associated to this compute node.

    @return     The program.
    */
    std::shared_ptr<ll::Program> getProgram() const noexcept;

    /**
    @brief      Gets a port descriptor given its name

    @param[in]  name  The name

    @return     The port descriptor.

    @throws     std::system_error With error code ll::ErrorCode::KeyNotFound if name is not
                                  in the ports table.
    */
    const ll::PortDescriptor& getPort(const std::string& name) const;

    /**
    @brief      Gets a parameter.

    @param[in]  name  The parameter name

    @return     The parameter.

    @throws     std::system_error With error code ll::ErrorCode::KeyNotFound if name is not
                                  in the parameters table.
    */
    const ll::Parameter& getParameter(const std::string& name) const;

    /**
    @brief      Gets the function name within the ll::Program object used by this node.

    @return     The function name.
    */
    const std::string& getFunctionName() const noexcept;

    /**
    @brief      Gets the builder name within the Lua interpreter.

    @return     The builder name.
    */
    const std::string& getBuilderName() const noexcept;

    /**
    @brief      Gets the grid shape.

    @return     The grid shape.
    */
    ll::vec3ui getGridShape() const noexcept;

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

    const ll::PushConstants& getPushConstants() const noexcept;

    std::vector<vk::DescriptorSetLayoutBinding> getParameterBindings() const;

private:
    std::shared_ptr<ll::Program> m_program;
    std::string                  m_functionName;
    std::string                  m_builderName;

    // local and global work group
    ll::vec3ui m_localShape {1, 1, 1};
    ll::vec3ui m_gridShape {1, 1, 1};

    std::map<std::string, ll::PortDescriptor> m_ports;
    std::map<std::string, ll::Parameter>      m_parameters;

    ll::PushConstants m_pushConstants;
};

} // namespace ll

#endif /* LLUVIA_CORE_NODE_COMPUTE_NODE_DESCRIPTOR_H_ */
