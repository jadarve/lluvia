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

// using PortDescriptor = struct {
struct PortDescriptor
{

    PortDescriptor() {}
    PortDescriptor(uint32_t pBinding,
                   const std::string &pName,
                   ll::PortDirection pDirection,
                   ll::PortType pType) : binding{pBinding},
                                         name{pName},
                                         direction{pDirection},
                                         type{pType} {}

    /**
    Binding number within shader program.
    */
    uint32_t binding{0};
    std::string name{};
    ll::PortDirection direction{ll::PortDirection::In};
    ll::PortType type{ll::PortType::Buffer};
};

} // namespace ll

#endif // LLUVIA_CORE_NODE_PORT_DESCRIPTOR_H_
