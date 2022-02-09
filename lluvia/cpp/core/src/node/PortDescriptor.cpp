/**
@file       PortDescriptor.cpp
@brief      PortDescriptor class.
@copyright  2022, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/node/PortDescriptor.h"


namespace ll {

PortDescriptor::PortDescriptor(uint32_t binding,
                               const std::string& name,
                               ll::PortDirection direction,
                               ll::PortType portType) : 
    m_binding   {binding},
    m_name      {name},
    m_direction {direction},
    m_portType  {portType} {

}

uint32_t PortDescriptor::getBinding() const noexcept {
    return m_binding;
}

const std::string& PortDescriptor::getName() const noexcept {
    return m_name;
}

ll::PortDirection PortDescriptor::getDirection() const noexcept {
    return m_direction;
}

ll::PortType PortDescriptor::getPortType() const noexcept {
    return m_portType;
}

} // namespace ll
