/**
@file       PortDescriptor.h
@brief      PortDescriptor class.
@copyright  2022, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NODE_PORT_DESCRIPTOR_H_
#define LLUVIA_CORE_NODE_PORT_DESCRIPTOR_H_

#include "lluvia/core/image/ImageDescriptor.h"
#include "lluvia/core/node/PortDirection.h"
#include "lluvia/core/node/PortType.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ll {

class Object;
class Buffer;
class ImageView;

class PortDescriptor {

public:
    PortDescriptor() = default;
    PortDescriptor(const PortDescriptor& descriptor) = default;
    PortDescriptor(PortDescriptor&& descriptor) = default;

    ~PortDescriptor() = default;

    PortDescriptor(uint32_t binding,
        const std::string& name,
        ll::PortDirection direction,
        ll::PortType portType);

    PortDescriptor& operator=(const PortDescriptor& descriptor) = default;
    PortDescriptor& operator=(PortDescriptor&& descriptor) = default;

    uint32_t getBinding() const noexcept;
    const std::string& getName() const noexcept;
    ll::PortDirection getDirection() const noexcept;
    ll::PortType getPortType() const noexcept;

    // optional checks
    PortDescriptor& checkImageChannelCountIs(ll::ChannelCount channelCount) noexcept;
    PortDescriptor& checkImageChannelTypeIs(ll::ChannelType channelType) noexcept;
    PortDescriptor& checkImageChannelTypeIsAnyOf(std::vector<ll::ChannelType> channelTypes) noexcept;
    PortDescriptor& checkImageViewNormalizedCoordinatesIs(bool normalizedCoordinates) noexcept;
    /**
     * @brief Checks whether or not a given Object is a valid port.
     *
     * @param port The port to validate
     * @return (true, "") if the port is valid or (false, "some message") if the
     * port is not valid under the spec of this descriptor.
     */
    std::pair<bool, std::string> isValid(const std::shared_ptr<ll::Object>& port) const noexcept;

private:
    // at the moment there is nothing to validate for a Buffer type port
    std::pair<bool, std::string> validateBuffer(const std::shared_ptr<ll::Buffer>& port) const noexcept;
    std::pair<bool, std::string> validateImageView(const std::shared_ptr<ll::ImageView>& port) const noexcept;

    std::string toString() const noexcept;

    /**
    Binding number within shader program.
    */
    uint32_t m_binding { 0 };
    std::string m_name {};
    ll::PortDirection m_direction { ll::PortDirection::In };
    ll::PortType m_portType { ll::PortType::Buffer };

    // optional checks
    std::optional<ll::ChannelCount> m_checkImageChannelCount {};
    std::optional<std::vector<ll::ChannelType>> m_checkImageChannelType {};
    std::optional<bool> m_checkImageViewNormalizedCoordinates {};
};

} // namespace ll

#endif // LLUVIA_CORE_NODE_PORT_DESCRIPTOR_H_
