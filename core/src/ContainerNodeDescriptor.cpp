#include "lluvia/core/ContainerNodeDescriptor.h"

namespace ll {

ContainerNodeDescriptor& ContainerNodeDescriptor::addPort(const ll::PortDescriptor& port) {
    
    m_ports[port.name] = port;
    return *this;
}


ContainerNodeDescriptor& ContainerNodeDescriptor::addPorts(const std::initializer_list<ll::PortDescriptor>& ports) {
    
    for(const auto& port : ports) {
        addPort(port);
    }

    return *this;
}


ContainerNodeDescriptor& ContainerNodeDescriptor::setBuilderName(const std::string& name) noexcept {

    m_builderName = name;
    return *this;
}


std::string ContainerNodeDescriptor::getBuilderName() const noexcept {
    return m_builderName;
}

} // namespace ll
