/**
@file       PortDescriptor.h
@brief      PortDescriptor class.
@copyright  2022, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NODE_PORT_DESCRIPTOR_H_
#define LLUVIA_CORE_NODE_PORT_DESCRIPTOR_H_

#include "lluvia/core/node/PortDirection.h"
#include "lluvia/core/node/PortType.h"

#include <string>
#include <cstdint>

namespace ll {

class PortDescriptor {

public:
    PortDescriptor()                                  = default;
    PortDescriptor(const PortDescriptor& descriptor)  = default;
    PortDescriptor(PortDescriptor&& descriptor)       = default;

    ~PortDescriptor()                                 = default;

    PortDescriptor(uint32_t binding,
                   const std::string &name,
                   ll::PortDirection direction,
                   ll::PortType portType);

    PortDescriptor& operator=(const PortDescriptor& descriptor) = default;
    PortDescriptor& operator=(PortDescriptor&& descriptor) = default;

    uint32_t getBinding() const noexcept;
    const std::string& getName() const noexcept;
    ll::PortDirection getDirection() const noexcept;
    ll::PortType getPortType() const noexcept;

private:
    /**
    Binding number within shader program.
    */
    uint32_t          m_binding   {0};
    std::string       m_name      {};
    ll::PortDirection m_direction {ll::PortDirection::In};
    ll::PortType      m_portType  {ll::PortType::Buffer};
};

} // namespace ll

#endif // LLUVIA_CORE_NODE_PORT_DESCRIPTOR_H_
