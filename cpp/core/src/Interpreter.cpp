/**
@file       Interpreter.cpp
@brief      Interpreter class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Interpreter.h"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/CommandBuffer.h"
#include "lluvia/core/ComputeNode.h"
#include "lluvia/core/ComputeNodeDescriptor.h"
#include "lluvia/core/ContainerNode.h"
#include "lluvia/core/ContainerNodeDescriptor.h"
#include "lluvia/core/Image.h"
#include "lluvia/core/ImageDescriptor.h"
#include "lluvia/core/ImageView.h"
#include "lluvia/core/ImageViewDescriptor.h"
#include "lluvia/core/Memory.h"
#include "lluvia/core/MemoryAllocationInfo.h"
#include "lluvia/core/Node.h"
#include "lluvia/core/Object.h"
#include "lluvia/core/Parameter.h"
#include "lluvia/core/Program.h"
#include "lluvia/core/Session.h"
#include "lluvia/core/types.h"

#include "lluvia/core/impl/LuaLibrary.h"

#include <vulkan/vulkan.hpp>

#include <iostream>

namespace ll {

template<typename T, std::size_t N, const std::array<std::tuple<const char*, T>, N>& values>
void registerEnum(sol::table& lib, const std::string& enumName) {

    // TODO: make table read only and not able to add new elements
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
    registerEnum<ll::NodeType, ll::impl::NodeTypeStrings.size(), ll::impl::NodeTypeStrings>(lib, "NodeType");
    registerEnum<ll::ObjectType, ll::impl::ObjectTypeStrings.size(), ll::impl::ObjectTypeStrings>(lib, "ObjectType");
    registerEnum<ll::ParameterType, ll::impl::ParameterTypeStrings.size(), ll::impl::ParameterTypeStrings>(lib, "ParameterType");
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

    lib.new_usertype<ll::MemoryAllocationInfo>("MemoryAllocationInfo",
        "offset", &ll::MemoryAllocationInfo::offset,
        "size", &ll::MemoryAllocationInfo::size,
        "leftPadding", &ll::MemoryAllocationInfo::leftPadding,
        "page", &ll::MemoryAllocationInfo::page
        );

    lib.new_usertype<ll::Parameter>("Parameter",
        sol::constructors<ll::Parameter(), ll::Parameter(const ll::Parameter&), ll::Parameter(ll::Parameter&&)>(),
        "type", sol::property(&ll::Parameter::getType),
        "__getInt", &ll::Parameter::get<int32_t>,
        "__getFloat", &ll::Parameter::get<float>,
        "__getBool", &ll::Parameter::get<bool>,
        "__setInt", &ll::Parameter::set<int32_t>,
        "__setFloat", &ll::Parameter::set<float>,
        "__setBool", &ll::Parameter::set<bool>
        );

    ///////////////////////////////////////////////////////
    // Descriptors
    ///////////////////////////////////////////////////////

    // TODO: usageFlags, tiling
    lib.new_usertype<ll::ImageDescriptor>("ImageDescriptor",
        sol::constructors<ll::ImageDescriptor(), ll::ImageDescriptor(const ll::ImageDescriptor&)>(),
        "channelType", sol::property(&ll::ImageDescriptor::getChannelType, &ll::ImageDescriptor::setChannelType),
        "channelCount", sol::property(&ll::ImageDescriptor::getChannelCount<ll::ChannelCount>, &ll::ImageDescriptor::setChannelCount),
        "width", sol::property(&ll::ImageDescriptor::getWidth, &ll::ImageDescriptor::setWidth),
        "height", sol::property(&ll::ImageDescriptor::getHeight, &ll::ImageDescriptor::setHeight),
        "depth", sol::property(&ll::ImageDescriptor::getDepth, &ll::ImageDescriptor::setDepth),
        "shape", sol::property(&ll::ImageDescriptor::getShape, &ll::ImageDescriptor::setShape)
        );

    lib.new_usertype<ll::ImageViewDescriptor>("ImageViewDescriptor",
        sol::constructors<ll::ImageViewDescriptor(), ll::ImageViewDescriptor(const ll::ImageViewDescriptor&)>(),
        "filterMode", sol::property(&ll::ImageViewDescriptor::getFilterMode, &ll::ImageViewDescriptor::setFilterMode),
        "addressModeU", sol::property(&ll::ImageViewDescriptor::getAddressModeU),
        "addressModeV", sol::property(&ll::ImageViewDescriptor::getAddressModeV),
        "addressModeW", sol::property(&ll::ImageViewDescriptor::getAddressModeW),
        "normalizedCoordinates", sol::property(&ll::ImageViewDescriptor::isNormalizedCoordinates, &ll::ImageViewDescriptor::setNormalizedCoordinates),
        "isSampled", sol::property(&ll::ImageViewDescriptor::isSampled, &ll::ImageViewDescriptor::setIsSampled),
        "setAddressMode", sol::overload(
            (ll::ImageViewDescriptor& (ll::ImageViewDescriptor::*)(ll::ImageAddressMode) noexcept) &ll::ImageViewDescriptor::setAddressMode,
            (ll::ImageViewDescriptor& (ll::ImageViewDescriptor::*)(ll::ImageAxis, ll::ImageAddressMode) noexcept) &ll::ImageViewDescriptor::setAddressMode
            )
        );

    lib.new_usertype<ll::PortDescriptor>("PortDescriptor",
        sol::constructors<ll::PortDescriptor(), ll::PortDescriptor(uint32_t, const std::string&, ll::PortDirection, ll::PortType)>(),
        "binding"   , &ll::PortDescriptor::binding,
        "name"      , &ll::PortDescriptor::name,
        "direction" , &ll::PortDescriptor::direction,
        "type"      , &ll::PortDescriptor::type
        );

    lib.new_usertype<ll::ComputeNodeDescriptor>("ComputeNodeDescriptor",
        "functionName", sol::property(&ll::ComputeNodeDescriptor::getFunctionName, &ll::ComputeNodeDescriptor::setFunctionName),
        "builderName", sol::property(&ll::ComputeNodeDescriptor::getBuilderName, &ll::ComputeNodeDescriptor::setBuilderName),
        "program", sol::property(&ll::ComputeNodeDescriptor::getProgram,
                                 (ComputeNodeDescriptor& (ll::ComputeNodeDescriptor::*)(const std::shared_ptr<ll::Program>&) noexcept) &ll::ComputeNodeDescriptor::setProgram
                                ),
        "localShape", sol::property(&ll::ComputeNodeDescriptor::getLocalShape, &ll::ComputeNodeDescriptor::setLocalShape),
        "gridShape", sol::property(&ll::ComputeNodeDescriptor::getGridShape, &ll::ComputeNodeDescriptor::setGridShape),
        "addPort", &ll::ComputeNodeDescriptor::addPort,
        "configureGridShape", &ll::ComputeNodeDescriptor::configureGridShape,
        "__addParameter", &ll::ComputeNodeDescriptor::addParameter, // user facing setParameter() implemented in library.lua
        "__getParameter", &ll::ComputeNodeDescriptor::getParameter  // user facing getParameter() implemented in library.lua
        );

    lib.new_usertype<ll::ContainerNodeDescriptor>("ContainerNodeDescriptor",
        "builderName", sol::property(&ll::ContainerNodeDescriptor::getBuilderName, &ll::ContainerNodeDescriptor::setBuilderName),
        "addPort", &ll::ContainerNodeDescriptor::addPort,
        "__addParameter", &ll::ContainerNodeDescriptor::addParameter, // user facing setParameter() implemented in library.lua
        "__getParameter", &ll::ContainerNodeDescriptor::getParameter  // user facing getParameter() implemented in library.lua
        );

    ///////////////////////////////////////////////////////
    // Objects
    ///////////////////////////////////////////////////////
    lib.new_usertype<ll::Object>("Object",
        sol::no_constructor,
        "type", sol::property(&ll::Object::getType)
        );

    lib.new_usertype<ll::Buffer>("Buffer",
        sol::no_constructor,
        sol::base_classes, sol::bases<ll::Object>(),
        "size", sol::property(&ll::Buffer::getSize),
        "isMappable", sol::property(&ll::Buffer::isMappable),
        "allocationInfo", sol::property(&ll::Buffer::getAllocationInfo),
        "memory", sol::property(&ll::Buffer::getMemory)
        );

    // TODO: layout, usageFlags, createImageView
    lib.new_usertype<ll::Image>("Image",
        sol::no_constructor,
        sol::base_classes, sol::bases<ll::Object>(),
        "size", sol::property(&ll::Image::getSize),
        "descriptor", sol::property(&ll::Image::getDescriptor),
        "allocationInfo", sol::property(&ll::Image::getAllocationInfo),
        "memory", sol::property(&ll::Image::getMemory),
        "channelType", sol::property(&ll::Image::getChannelType),
        "channelCount", sol::property(&ll::Image::getChannelCount<ll::ChannelCount>),
        "width", sol::property(&ll::Image::getWidth),
        "height", sol::property(&ll::Image::getHeight),
        "depth", sol::property(&ll::Image::getDepth),
        "shape", sol::property(&ll::Image::getShape),
        "changeImageLayout", &ll::Image::changeImageLayout
        );

    // TODO: usage flags, layout
    lib.new_usertype<ll::ImageView>("ImageView",
        sol::no_constructor,
        sol::base_classes, sol::bases<ll::Object>(),
        "image", sol::property(&ll::ImageView::getImage),
        "size", sol::property(&ll::ImageView::getSize),
        "descriptor", sol::property(&ll::ImageView::getDescriptor),
        "imageDescriptor", sol::property(&ll::ImageView::getImageDescriptor),
        "allocationInfo", sol::property(&ll::ImageView::getAllocationInfo),
        "memory", sol::property(&ll::ImageView::getMemory),
        "channelType", sol::property(&ll::ImageView::getChannelType),
        "channelCount", sol::property(&ll::ImageView::getChannelCount<ll::ChannelCount>),
        "width", sol::property(&ll::ImageView::getWidth),
        "height", sol::property(&ll::ImageView::getHeight),
        "depth", sol::property(&ll::ImageView::getDepth),
        "shape", sol::property(&ll::ImageView::getShape),
        "changeImageLayout", &ll::ImageView::changeImageLayout
        );


    ///////////////////////////////////////////////////////
    // Nodes
    ///////////////////////////////////////////////////////
    lib.new_usertype<ll::Program>("Program",
        sol::no_constructor
        );

    lib.new_usertype<ll::Node>("Node",
        sol::no_constructor,
        "type", sol::property(&ll::Node::getType),
        "state", sol::property(&ll::Node::getState),
        "init", &ll::Node::init,
        "record", &ll::Node::record,
        "__getPort", &ll::Node::getPort, // user facing getPort() implemented in library.lua
        "__bind", &ll::Node::bind        // user facing bind() implemented in library.lua
        );

    lib.new_usertype<ll::ComputeNode>("ComputeNode",
        sol::no_constructor,
        "type", sol::property(&ll::ComputeNode::getType),
        "state", sol::property(&ll::ComputeNode::getState),
        "functionName", sol::property(&ll::ComputeNode::getFunctionName),
        "program", sol::property(&ll::ComputeNode::getProgram),
        "descriptor", sol::property(&ll::ComputeNode::getDescriptor),
        "localX", sol::property(&ll::ComputeNode::getLocalX),
        "localY", sol::property(&ll::ComputeNode::getLocalY),
        "localZ", sol::property(&ll::ComputeNode::getLocalZ),
        "localShape", sol::property(&ll::ComputeNode::getLocalShape),
        "gridX", sol::property(&ll::ComputeNode::getGridX, &ll::ComputeNode::getGridX),
        "gridY", sol::property(&ll::ComputeNode::getGridY, &ll::ComputeNode::setGridY),
        "gridZ", sol::property(&ll::ComputeNode::getGridZ, &ll::ComputeNode::setGridZ),
        "gridShape", sol::property(&ll::ComputeNode::getGridShape, &ll::ComputeNode::setGridShape),
        "configureGridShape", &ll::ComputeNode::configureGridShape,
        "init", &ll::ComputeNode::init,
        "record", &ll::ComputeNode::record,
        "__getPort", &ll::ComputeNode::getPort, // user facing getPort() implemented in library.lua
        "__bind", &ll::ComputeNode::bind        // user facing bind() implemented in library.lua
        );

    lib.new_usertype<ll::ContainerNode>("ContainerNode",
        sol::no_constructor,
        "type", sol::property(&ll::ContainerNode::getType),
        "state", sol::property(&ll::ContainerNode::getState),
        "descriptor", sol::property(&ll::ContainerNode::getDescriptor),
        "init", &ll::ContainerNode::init,
        "record", &ll::ContainerNode::record,
        "__getPort", &ll::ContainerNode::getPort,   // user facing getPort() implemented in library.lua
        "__bind", &ll::ContainerNode::bind,         // user facing bind() implemented in library.lua
        "__bindNode", &ll::ContainerNode::bindNode, // user facing bindNode() implemented in library.lua 
        "__getNode", &ll::ContainerNode::getNode    // user facing getNode() implemented in library.lua
        );

    lib.new_usertype<ll::Session>("Session",
        sol::no_constructor,
        "isImageDescriptorSupported", &ll::Session::isImageDescriptorSupported,
        "getProgram", &ll::Session::getProgram,
        "createComputeNode", (std::shared_ptr<ll::ComputeNode> (ll::Session::*)(const std::string& builderName)) &ll::Session::createComputeNode,
        "createContainerNode", (std::shared_ptr<ll::ContainerNode> (ll::Session::*)(const std::string& builderName)) &ll::Session::createContainerNode
        );

    lib.new_usertype<ll::Memory>("Memory",
        sol::no_constructor,
        "pageSize", sol::property(&ll::Memory::getPageSize),
        "pageCount", sol::property(&ll::Memory::getPageCount),
        "isMappable", sol::property(&ll::Memory::isMappable),
        "isPageMappable", &ll::Memory::isPageMappable,
        "createBuffer", &ll::Memory::createBuffer,
        "createImage", &ll::Memory::createImage,
        "createImageView", &ll::Memory::createImageView
        );

    lib.new_usertype<ll::CommandBuffer>("CommandBuffer",
        sol::no_constructor,
        "run", &ll::CommandBuffer::run,
        "memoryBarrier", &ll::CommandBuffer::memoryBarrier
        );
}


Interpreter::Interpreter() :
    m_lua {std::make_unique<sol::state>()} {

    // load default libraries
    m_lua->open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);

    // ll and impl namespaces
    m_lib = (*m_lua)["ll"].get_or_create<sol::table>();
    m_libImpl = m_lib["impl"].get_or_create<sol::table>();

    registerTypes(m_lib);
    
    m_libImpl["castObjectToBuffer"]    = [](std::shared_ptr<ll::Object> obj) {return std::static_pointer_cast<ll::Buffer>(obj);};
    m_libImpl["castObjectToImage"]     = [](std::shared_ptr<ll::Object> obj) {return std::static_pointer_cast<ll::Image>(obj);};
    m_libImpl["castObjectToImageView"] = [](std::shared_ptr<ll::Object> obj) {return std::static_pointer_cast<ll::ImageView>(obj);};

    m_libImpl["castBufferToObject"] = [](std::shared_ptr<ll::Buffer> buffer) {return std::static_pointer_cast<ll::Object>(buffer);};
    m_libImpl["castImageToObject"] = [](std::shared_ptr<ll::Image> image) {return std::static_pointer_cast<ll::Object>(image);};
    m_libImpl["castImageViewToObject"] = [](std::shared_ptr<ll::ImageView> imageView) {return std::static_pointer_cast<ll::Object>(imageView);};

    m_libImpl["castComputeNodeToNode"] = [](std::shared_ptr<ll::ComputeNode> node) {return std::static_pointer_cast<ll::Node>(node);};
    m_libImpl["castContainerNodeToNode"] = [](std::shared_ptr<ll::ContainerNode> node) {return std::static_pointer_cast<ll::Node>(node);};

    m_libImpl["castNodeToComputeNode"] = [](std::shared_ptr<ll::Node> node) {return std::static_pointer_cast<ll::ComputeNode>(node);};
    m_libImpl["castNodeToContainerNode"] = [](std::shared_ptr<ll::Node> node) {return std::static_pointer_cast<ll::ContainerNode>(node);};

    m_lua->script(ll::impl::LUA_LIBRARY_SRC);
}


Interpreter::~Interpreter() {

}


void Interpreter::run(const std::string& code) {
    
    auto result = m_lua->script(code);

    if (!result.valid()) {
        auto err = static_cast<sol::error>(result);
        std::cerr << "Interpreter::run(): " << err.what() << std::endl;
    }
}


void Interpreter::runFile(const std::string& filename) {
    auto result = m_lua->script_file(filename);

    if (!result.valid()) {
        auto err = static_cast<sol::error>(result);
        std::cerr << "Interpreter::run(): " << err.what() << std::endl;
    }
}


sol::load_result Interpreter::load(const std::string& code) {
    return m_lua->load(code);
}


void Interpreter::setActiveSession(ll::Session* session) {
    m_lib["activeSession"] = session;
}

} // namespace ll
