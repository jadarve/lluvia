#include "lluvia/core/ContainerNodeDescriptor.h"

#include "lluvia/core/error.h"

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


const ll::PortDescriptor& ContainerNodeDescriptor::getPort(const std::string& name) const {

    auto it = m_ports.find(name);

    ll::throwSystemErrorIf(it == m_ports.cend(), ll::ErrorCode::KeyNotFound, "Port [" + name + "] not found.");
    return it->second;    
}


ContainerNodeDescriptor& ContainerNodeDescriptor::setParameter(const std::string& name, const ll::Parameter& defaultValue) {
    m_parameters[name] = defaultValue;
    return *this;
}


const ll::Parameter& ContainerNodeDescriptor::getParameter(const std::string& name) const {

    auto it = m_parameters.find(name);

    ll::throwSystemErrorIf(it == m_parameters.cend(), ll::ErrorCode::KeyNotFound, "Parameter [" + name + "] not found.");
    return it->second;
}


ContainerNodeDescriptor& ContainerNodeDescriptor::setBuilderName(const std::string& name) noexcept {

    m_builderName = name;
    return *this;
}


const std::string& ContainerNodeDescriptor::getBuilderName() const noexcept {
    return m_builderName;
}

} // namespace ll
