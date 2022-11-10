/**
@file       Node.h
@brief      Node class and related enums.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NODE_NODE_H_
#define LLUVIA_CORE_NODE_NODE_H_

#include <cstdint>
#include <memory>
#include <string>

#include "lluvia/core/enums/enums.h"
#include "lluvia/core/vulkan/vulkan.hpp"

#include "lluvia/core/node/NodeState.h"
#include "lluvia/core/node/NodeType.h"
#include "lluvia/core/node/PortDescriptor.h"
#include "lluvia/core/node/PortDirection.h"
#include "lluvia/core/node/PortType.h"

namespace ll {

class Object;
class CommandBuffer;
class Parameter;

class Node {

public:
    virtual ~Node() = default;

    void          setState(const ll::NodeState tState);
    ll::NodeState getState() const noexcept;

    void init();

    /**
    @brief      Gets the node type.

    @return     The type.

    @sa ll::NodeType
    */
    virtual ll::NodeType getType() const noexcept = 0;

    /**
    @brief      Returns whether or not a port exists with a given name.

    @param[in]  name  The name of the port.

    @return     true if the port exists, false otherwise.
    */
    virtual bool hasPort(const std::string& name) const noexcept = 0;

    /**
    @brief      Gets a port descriptor given its name

    @param[in]  name  The name

    @return     The port descriptor.

    @throws     std::system_error With error code ll::ErrorCode::KeyNotFound if name is not
                                  in the ports table.
    */
    virtual std::shared_ptr<ll::Object> getPort(const std::string& name) const = 0;

    /**
    @brief      Sets a parameter.

    @param[in]  name          The name
    @param[in]  defaultValue  The value.

    @return     A reference to this object.
    */
    virtual void setParameter(const std::string& name, const ll::Parameter& value) = 0;

    /**
    @brief      Gets a parameter.

    @param[in]  name  The parameter name

    @return     The parameter.

    @throws     std::system_error With error code ll::ErrorCode::KeyNotFound if name is not
                                  in the parameters table.
    */
    virtual const ll::Parameter& getParameter(const std::string& name) const = 0;

    /**
    @brief      Binds a ll::Object as port \p index for this node.

    @param[in]  name   The port name.
    @param[in]  obj    The object to bind.

    @throws     std::system_error if \p obj cannot be mapped as a port at position \p index.
    */
    virtual void bind(const std::string& name, const std::shared_ptr<ll::Object>& obj) = 0;

    /**
    @brief      Records the operations required to run this node in a ll::CommandBuffer.

    @param[in]  commandBuffer  The command buffer.

    @throws     std::system_error with corresponding error code and message.
    */
    virtual void record(ll::CommandBuffer& commandBuffer) const = 0;

protected:
    virtual void onInit() = 0;

private:
    ll::NodeState m_state {ll::NodeState::Created};
};

} // namespace ll

#endif // LLUVIA_CORE_NODE_NODE_H_
