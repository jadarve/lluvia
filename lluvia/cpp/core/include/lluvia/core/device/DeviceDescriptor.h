/**
@file       ImageDescriptor.h
@brief      ImageDescriptor class and related enumerations.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_DEVICE_DEVICE_DESCRIPTOR_H_
#define LLUVIA_CORE_DEVICE_DEVICE_DESCRIPTOR_H_

#include <cstdint>
#include <string>

#include "lluvia/core/device/DeviceType.h"

namespace ll {

struct DeviceDescriptor {
    uint32_t       id;
    ll::DeviceType deviceType;
    std::string    name;

    bool operator==(const DeviceDescriptor& other) const
    {
        return id == other.id && deviceType == other.deviceType && name == other.name;
    }
};

} // namespace ll

#endif /* LLUVIA_CORE_DEVICE_DEVICE_DESCRIPTOR_H_ */
