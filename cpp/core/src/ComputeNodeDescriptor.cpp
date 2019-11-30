/**
@file       ComputeNodeDescriptor.cpp
@brief      ComputeNodeDescriptor class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/ComputeNodeDescriptor.h"

#include "lluvia/core/error.h"
#include "lluvia/core/utils.h"
#include "lluvia/core/Program.h"

#include <cassert>
#include <exception>
#include <stdexcept>
#include <vector>
#include <iostream>

namespace ll {


ComputeNodeDescriptor& ComputeNodeDescriptor::setProgram(const std::shared_ptr<ll::Program>& tProgram) noexcept {

    this->m_program = tProgram;
    return *this;
}

ComputeNodeDescriptor& ComputeNodeDescriptor::setProgram(const std::shared_ptr<ll::Program>& tProgram, const std::string& tFunctionName) noexcept {
    
    setProgram(tProgram);
    setFunctionName(tFunctionName);
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::addParameter(const std::string& name, const ll::Parameter& value) {

    m_parameters[name] = value;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setFunctionName(const std::string& name) noexcept {

    m_functionName = name;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setBuilderName(const std::string& name) noexcept {
    m_builderName = name;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::addPort(const ll::PortDescriptor& port) {

    m_ports[port.name] = port;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::addPorts(const std::initializer_list<ll::PortDescriptor>& ports) {

    for(const auto& port : ports) {
        addPort(port);
    }

    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setGridX(const uint32_t x) noexcept {
    m_gridShape.x = x;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setGridY(const uint32_t y) noexcept {
    m_gridShape.y = y;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setGridZ(const uint32_t z) noexcept {
    m_gridShape.z = z;
    return *this;
}

ComputeNodeDescriptor& ComputeNodeDescriptor::setGridShape(const ll::vec3ui& shape) noexcept {

    m_gridShape = shape;
    return *this;
}

ComputeNodeDescriptor& ComputeNodeDescriptor::configureGridShape(const ll::vec3ui& globalShape) noexcept {

    m_gridShape = ll::configureGridShape(m_localShape, globalShape);
    return *this;
}

ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalX(const uint32_t x) noexcept {
    m_localShape.x = x;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalY(const uint32_t y) noexcept {
    m_localShape.y = y;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalZ(const uint32_t z) noexcept {
    m_localShape.z = z;
    return *this;
}


ComputeNodeDescriptor& ComputeNodeDescriptor::setLocalShape(const ll::vec3ui& shape) noexcept {
    m_localShape = shape;
    return *this;
}


const std::string& ComputeNodeDescriptor::getFunctionName() const noexcept {
    return m_functionName;
}


const std::string& ComputeNodeDescriptor::getBuilderName() const noexcept {
    return m_builderName;
}


ll::vec3ui ComputeNodeDescriptor::getGridShape() const noexcept {
    return m_gridShape;
}


ll::vec3ui ComputeNodeDescriptor::getLocalShape() const noexcept {
    return m_localShape;
}


uint32_t ComputeNodeDescriptor::getGridX() const noexcept {
    return m_gridShape.x;
}


uint32_t ComputeNodeDescriptor::getGridY() const noexcept {
    return m_gridShape.y;
}


uint32_t ComputeNodeDescriptor::getGridZ() const noexcept {
    return m_gridShape.z;
}


uint32_t ComputeNodeDescriptor::getLocalX() const noexcept {
    return m_localShape.x;
}


uint32_t ComputeNodeDescriptor::getLocalY() const noexcept {
    return m_localShape.y;
}


uint32_t ComputeNodeDescriptor::getLocalZ() const noexcept {
    return m_localShape.z;
}


std::shared_ptr<ll::Program> ComputeNodeDescriptor::getProgram() const noexcept {
    return m_program;
}


const ll::PortDescriptor& ComputeNodeDescriptor::getPort(const std::string& name) const {

    auto it = m_ports.find(name);

    ll::throwSystemErrorIf(it == m_ports.cend(), ll::ErrorCode::KeyNotFound, "Port [" + name + "] not found.");
    return it->second;    
}


const ll::Parameter& ComputeNodeDescriptor::getParameter(const std::string& name) const {

    auto it = m_parameters.find(name);

    ll::throwSystemErrorIf(it == m_parameters.cend(), ll::ErrorCode::KeyNotFound, "Parameter [" + name + "] not found.");
    return it->second;
}


std::vector<vk::DescriptorSetLayoutBinding> ComputeNodeDescriptor::getParameterBindings() const {

    auto bindings = std::vector<vk::DescriptorSetLayoutBinding> {};

    for(const auto it : m_ports) {

        const auto& port = it.second;
        auto binding = vk::DescriptorSetLayoutBinding {}
                        .setBinding(port.binding)
                        .setDescriptorCount(1)
                        .setDescriptorType(ll::portTypeToVkDescriptorType(port.type))
                        .setStageFlags(vk::ShaderStageFlagBits::eCompute)
                        .setPImmutableSamplers(nullptr);
        bindings.push_back(binding);
    }

    return bindings;
}

} // namespace ll
