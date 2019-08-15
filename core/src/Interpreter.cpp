/**
@file       Interpreter.cpp
@brief      Interpreter class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Interpreter.h"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/ComputeNodeDescriptor.h"
#include "lluvia/core/Node.h"
#include "lluvia/core/Object.h"
#include "lluvia/core/Program.h"
#include "lluvia/core/types.h"
#include "lluvia/core/ImageDescriptor.h"
#include "lluvia/core/ImageViewDescriptor.h"
#include "lluvia/core/Image.h"


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-lambda-capture"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#pragma clang diagnostic pop

#include <vulkan/vulkan.hpp>

#include <iostream>

namespace ll {

template<typename T, std::size_t N, const std::array<std::tuple<const char*, T>, N>& values>
void registerEnum(sol::table& lib, const std::string& enumName) {

    auto table = lib.create(enumName);
    for (const auto& kv : values) {
        table[std::get<0>(kv)] = std::get<1>(kv);
    }
}

void registerTypes(sol::table& lib) {

    ///////////////////////////////////////////////////////
    // Enums
    ///////////////////////////////////////////////////////
    registerEnum<ll::ChannelCount, ll::impl::ChannelCountStrings.size(), ll::impl::ChannelCountStrings>(lib, "ChannelCount");
    registerEnum<ll::ChannelType, ll::impl::ChannelTypeStrings.size(), ll::impl::ChannelTypeStrings>(lib, "ChannelType");
    registerEnum<ll::ImageAddressMode, ll::impl::ImageAddressModeStrings.size(), ll::impl::ImageAddressModeStrings>(lib, "ImageAddressMode");
    registerEnum<ll::ImageAxis, ll::impl::ImageAxisStrings.size(), ll::impl::ImageAxisStrings>(lib, "ImageAxis");
    registerEnum<ll::ImageFilterMode, ll::impl::ImageFilterModeStrings.size(), ll::impl::ImageFilterModeStrings>(lib, "ImageFilterMode");
    registerEnum<ll::NodeState, ll::impl::NodeStateStrings.size(), ll::impl::NodeStateStrings>(lib, "NodeState");
    registerEnum<ll::ObjectType, ll::impl::ObjectTypeStrings.size(), ll::impl::ObjectTypeStrings>(lib, "ObjectType");
    registerEnum<ll::PortDirection, ll::impl::PortDirectionStrings.size(), ll::impl::PortDirectionStrings>(lib, "PortDirection");
    registerEnum<ll::PortType, ll::impl::PortTypeStrings.size(), ll::impl::PortTypeStrings>(lib, "PortType");
    registerEnum<vk::BufferUsageFlagBits, ll::impl::VkBufferUsageFlagBitsStrings.size(), ll::impl::VkBufferUsageFlagBitsStrings>(lib, "BufferUsageFlagBits");
    registerEnum<vk::ImageLayout, ll::impl::VkImageLayoutStrings.size(), ll::impl::VkImageLayoutStrings>(lib, "ImageLayout");
    registerEnum<vk::ImageUsageFlagBits, ll::impl::VkImageUsageFlagBitsStrings.size(), ll::impl::VkImageUsageFlagBitsStrings>(lib, "ImageUsageFlagBits");

    ///////////////////////////////////////////////////////
    // Types
    ///////////////////////////////////////////////////////
    lib.new_usertype<ll::vec3ui>("vec3ui",
        sol::constructors<ll::vec3ui(), ll::vec3ui(const uint32_t&), ll::vec3ui(const uint32_t&, const uint32_t&, const uint32_t&)>(),
        "x", &ll::vec3ui::x,
        "y", &ll::vec3ui::y,
        "z", &ll::vec3ui::z);


    ///////////////////////////////////////////////////////
    // Objects
    ///////////////////////////////////////////////////////
    lib.new_usertype<ll::Object>("Object"
        );

    lib.new_usertype<ll::Buffer>("Buffer",
        "size", sol::property(&ll::Buffer::getSize),
        "isMappable", sol::property(&ll::Buffer::isMappable)
        );

    // TODO: usageFlags, tiling
    lib.new_usertype<ll::ImageDescriptor>("ImageDescriptor",
        sol::constructors<ll::ImageDescriptor()>(),
        "channelType", sol::property(&ll::ImageDescriptor::getChannelType, &ll::ImageDescriptor::setChannelType),
        "channelCount", sol::property(&ll::ImageDescriptor::getChannelCount<ll::ChannelCount>, &ll::ImageDescriptor::setChannelCount),
        "width", sol::property(&ll::ImageDescriptor::getWidth, &ll::ImageDescriptor::setWidth),
        "height", sol::property(&ll::ImageDescriptor::getHeight, &ll::ImageDescriptor::setHeight),
        "depth", sol::property(&ll::ImageDescriptor::getDepth, &ll::ImageDescriptor::setDepth),
        "shape", sol::property(&ll::ImageDescriptor::getShape, &ll::ImageDescriptor::setShape)
        );

    // TODO: layout, usageFlags, createImageView
    lib.new_usertype<ll::Image>("Image",
        sol::constructors<>(),
        "size", sol::property(&ll::Image::getSize),
        "channelType", sol::property(&ll::Image::getChannelType),
        "channelCount", sol::property(&ll::Image::getChannelCount<ll::ChannelCount>),
        "width", sol::property(&ll::Image::getWidth),
        "height", sol::property(&ll::Image::getHeight),
        "depth", sol::property(&ll::Image::getDepth),
        "shape", sol::property(&ll::Image::getShape)
        );

    // TODO
    lib.new_usertype<ll::ImageViewDescriptor>("ImageViewDescriptor",
        sol::constructors<ll::ImageViewDescriptor()>(),
        "filterMode", sol::property(&ll::ImageViewDescriptor::getFilterMode, &ll::ImageViewDescriptor::setFilterMode)
        );


    ///////////////////////////////////////////////////////
    // Nodes
    ///////////////////////////////////////////////////////
    lib.new_usertype<ll::PortDescriptor>("PortDescriptor",
        sol::constructors<ll::PortDescriptor(), ll::PortDescriptor(uint32_t, const std::string&, ll::PortDirection, ll::PortType)>(),
        "binding"   , &ll::PortDescriptor::binding,
        "name"      , &ll::PortDescriptor::name,
        "direction" , &ll::PortDescriptor::direction,
        "type"      , &ll::PortDescriptor::type
        );

    lib.new_usertype<ll::Program>("Program");

    lib.new_usertype<ll::ComputeNodeDescriptor>("ComputeNodeDescriptor",
        "functionName", sol::property(&ll::ComputeNodeDescriptor::getFunctionName, &ll::ComputeNodeDescriptor::setFunctionName),
        "program", sol::property(&ll::ComputeNodeDescriptor::getProgram, &ll::ComputeNodeDescriptor::setProgram),
        "localShape", sol::property(&ll::ComputeNodeDescriptor::getLocalShape, &ll::ComputeNodeDescriptor::setLocalShape),
        "gridShape", sol::property(&ll::ComputeNodeDescriptor::getGridShape, &ll::ComputeNodeDescriptor::setGridShape),
        "addPort", &ll::ComputeNodeDescriptor::addPort,
        "configureGridShape", &ll::ComputeNodeDescriptor::configureGridShape
        );
}


Interpreter::Interpreter() :
    m_lua {std::make_unique<sol::state>()} {

    // load default libraries
    m_lua->open_libraries(sol::lib::base, sol::lib::math);

    // library namespace
    auto lib = (*m_lua)["ll"].get_or_create<sol::table>();

    registerTypes(lib);
}


Interpreter::~Interpreter() {

}


void Interpreter::run(const std::string& code) {
    m_lua->script(code);
}


void Interpreter::runFile(const std::string& filename) {
    m_lua->script_file(filename);
}

} // namespace ll
