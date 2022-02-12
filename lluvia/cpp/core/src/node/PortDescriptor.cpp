/**
@file       PortDescriptor.cpp
@brief      PortDescriptor class.
@copyright  2022, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/node/PortDescriptor.h"

#include "lluvia/core/Object.h"
#include "lluvia/core/buffer/Buffer.h"
#include "lluvia/core/image/ImageView.h"

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

PortDescriptor& PortDescriptor::checkImageChannelCountIs(ll::ChannelCount channelCount) noexcept {

    m_checkImageChannelCount = std::optional<ll::ChannelCount>{channelCount};
    return *this;
}

PortDescriptor& PortDescriptor::checkImageChannelTypeIs(ll::ChannelType channelType) noexcept {

    m_checkImageChannelType = std::optional<std::vector<ll::ChannelType>> {{channelType}};
    return *this;
}

PortDescriptor& PortDescriptor::checkImageChannelTypeIsAnyOf(std::vector<ll::ChannelType> channelTypes) noexcept {

    m_checkImageChannelType = std::optional<std::vector<ll::ChannelType>>{channelTypes};
    return *this;
}

PortDescriptor& PortDescriptor::checkImageViewNormalizedCoordinatesIs(bool normalizedCoordinates) noexcept {

    m_checkImageViewNormalizedCoordinates = std::optional<bool> {normalizedCoordinates};
    return *this;
}


std::pair<bool, std::string> PortDescriptor::isValid(const std::shared_ptr<ll::Object> &port) const noexcept {

    if (port == nullptr) {
        return std::make_pair(false, "null port");
    }

    switch (port->getType()) {

    case ll::ObjectType::Buffer:
        return validateBuffer();
    
    case ll::ObjectType::ImageView:
        return validateImageView(std::static_pointer_cast<ll::ImageView>(port));

    default:
        return std::make_pair(false, "Unsupported object type: " + ll::objectTypeToString(port->getType()));
    }

    return std::make_pair(true, std::string{});
}

std::pair<bool, std::string> PortDescriptor::validateBuffer() const noexcept {

    if (m_portType != ll::PortType::Buffer) {
        return std::make_pair(false, "Port " + toString() + " cannot receive object of type ll::PortType::Buffer");
    }

    return std::make_pair(true, std::string{});
}

std::pair<bool, std::string> PortDescriptor::validateImageView(const std::shared_ptr<ll::ImageView> &port) const noexcept {

    const auto isSampledImageView = port->getDescriptor().isSampled();

    if (isSampledImageView) {
        // check this port type
        if (m_portType != ll::PortType::SampledImageView) {
            return std::make_pair(false, "Port " + toString() + " cannot receive object of type ll::PortType::SampledImageView");
        }

    } else {

        // check this port type
        if (m_portType != ll::PortType::ImageView) {
            return std::make_pair(false, "Port " + toString() + " cannot receive object of type ll::PortType::ImageView");
        }
    }

    ///////////////////////////////////////////////////////
    // Optional checks
    ///////////////////////////////////////////////////////

    if (m_checkImageChannelCount.has_value() &&
        m_checkImageChannelCount.value() != port->getChannelCount()) {
        
        return std::make_pair(false, "Port " + toString() + " invalid image channel count, expecting: " +
            std::to_string(static_cast<enum_t>(m_checkImageChannelCount.value())) + 
            " got: " + std::to_string(static_cast<enum_t>(port->getChannelCount())));
    }

    if (m_checkImageChannelType.has_value()) {
        const auto& validChannelTypes = m_checkImageChannelType.value();

        if (std::find(validChannelTypes.cbegin(), validChannelTypes.cend(), port->getChannelType()) == validChannelTypes.cend()) {

            auto cTypeString = std::string{};
            for (const auto& cType : validChannelTypes) {
                cTypeString += ll::channelTypeToString(cType) + ", ";
            }

            return std::make_pair(false, "Port " + toString() + " invalid image channel type," + 
                " expecting any of: [" + cTypeString + "]" +
                " got: " + ll::channelTypeToString(port->getChannelType()));
        }
    }

    if (m_checkImageViewNormalizedCoordinates.has_value() &&
        m_checkImageViewNormalizedCoordinates.value() != port->getDescriptor().isNormalizedCoordinates()) {
        
        return std::make_pair(false, "Port " + toString() + " invalid image view normalized coordinates flag," +
            " expecting: " + std::to_string(m_checkImageViewNormalizedCoordinates.value()) +
            " got: " + std::to_string(port->getDescriptor().isNormalizedCoordinates())
            );
    }


    return std::make_pair(true, std::string{});
}

std::string PortDescriptor::toString() const noexcept {
    return "{binding: " + std::to_string(m_binding) 
        + ", name: " + m_name
        + ", portType: ll::PortType::" + ll::portTypeToString(m_portType) + "}";
}

} // namespace ll
