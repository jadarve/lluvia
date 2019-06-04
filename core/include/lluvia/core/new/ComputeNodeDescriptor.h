/**
@file       ComputeNodeDescriptor.h
@brief      ComputeNodeDescriptor class and related enums.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NEW_COMPUTE_NODE_DESCRIPTOR_H_
#define LLUVIA_CORE_NEW_COMPUTE_NODE_DESCRIPTOR_H_

#include "lluvia/core/ComputeNodeDescriptor.h"

#include <cstdint>
#include <map>
#include <string>

namespace ll {
namespace news {

enum class PortDirection : uint32_t {
    IN  = 0,
    OUT = 1
};

struct PortDescriptor {
    ll::news::PortDirection direction;
    ll::ParameterType type;
};

struct ComputeNodeDescriptor {
    std::map<const std::string, ll::news::PortDescriptor> portDescriptors;
};

// class ComputeNodeDescriptor {

// public:
//     void addPort(const std::string& name, const PortDescriptor& port);

// private:
//     std::map<const std::string, ll::news::PortDescriptor> portDescriptors;

// };

} // namespace news
} // namespace ll

#endif // LLUVIA_CORE_NEW_COMPUTE_NODE_DESCRIPTOR_H_
