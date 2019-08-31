/**
@file       ContainerNodeDescriptor.h
@brief      ContainerNodeDescriptor class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_CONTAINER_DESCRIPTOR_NODE_H_
#define LLUVIA_CORE_CONTAINER_DESCRIPTOR_NODE_H_

#include "lluvia/core/impl/enum_utils.h"
#include "lluvia/core/types.h"
#include "lluvia/core/Node.h"
#include "lluvia/core/Parameter.h"

#include <map>
#include <string>


namespace ll {


/**
@brief      Class for describing a container node.

Descriptors are used to construct ll::ContainerNode objects.
*/
class ContainerNodeDescriptor {

public:
    ContainerNodeDescriptor()                                                       = default;
    ContainerNodeDescriptor(const ContainerNodeDescriptor& descriptor)              = default;
    ContainerNodeDescriptor(ContainerNodeDescriptor&& descriptor)                   = default;

    ~ContainerNodeDescriptor()                                                      = default;
    
    ContainerNodeDescriptor& operator = (const ContainerNodeDescriptor& descriptor) = default;
    ContainerNodeDescriptor& operator = (ContainerNodeDescriptor&& descriptor)      = default;


    /**
    @brief      Adds a port to the descriptor.
    
    @param[in]  port  The port
    
    @return     A reference to this object.
    */
    ContainerNodeDescriptor& addPort(const ll::PortDescriptor& port);


    /**
    @brief      Adds a list of ports to the descriptor.
    
    @param[in]  ports  The ports
    
    @return     A reference to this object.
    */
    ContainerNodeDescriptor& addPorts(const std::initializer_list<ll::PortDescriptor>& ports);


    /**
    @brief      Gets a port descriptor given its name
    
    @param[in]  name  The name
    
    @return     The port descriptor.

    @throws     std::system_error With error code ll::ErrorCode::KeyNotFound if name is not
                                  in the ports table.
    */
    const ll::PortDescriptor& getPort(const std::string& name) const;


    /**
    @brief      Adds a parameter.
    
    @param[in]  name          The name
    @param[in]  defaultValue  The value.
    
    @return     A reference to this object.
    */
    ContainerNodeDescriptor& addParameter(const std::string& name, const ll::Parameter& value);


    /**
    @brief      Gets a parameter.
    
    @param[in]  name  The parameter name
    
    @return     The parameter.

    @throws     std::system_error With error code ll::ErrorCode::KeyNotFound if name is not
                                  in the parameters table.
    */
    const ll::Parameter& getParameter(const std::string& name) const;

    /**
    @brief      Sets the builder name this descriptor refers to within the Lua interpreter.
    
    @param[in]  name  The builder name.
    
    @return     A reference to this object.
    */
    ContainerNodeDescriptor& setBuilderName(const std::string& name) noexcept;


    /**
    @brief      Gets the builder name within the Lua interpreter.
    
    @return     The builder name.
    */
    std::string getBuilderName() const noexcept;


private:
    std::string                               m_builderName;
    std::map<std::string, ll::PortDescriptor> m_ports;
    std::map<std::string, ll::Parameter>      m_parameters;
};


} // namespace ll

#endif // LLUVIA_CORE_CONTAINER_DESCRIPTOR_NODE_H_
