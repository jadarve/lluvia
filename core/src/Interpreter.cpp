/**
@file       Interpreter.cpp
@brief      Interpreter class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Interpreter.h"

#include "lluvia/core/Node.h"
#include "lluvia/core/Program.h"
#include "lluvia/core/ComputeNodeDescriptor.h"
#include "lluvia/core/types.h"


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-lambda-capture"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#pragma clang diagnostic pop

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

    registerEnum<ll::NodeState, ll::impl::NodeStateStrings.size(), ll::impl::NodeStateStrings>(lib, "NodeState");
    registerEnum<ll::PortDirection, ll::impl::PortDirectionStrings.size(), ll::impl::PortDirectionStrings>(lib, "PortDirection");
    registerEnum<ll::PortType, ll::impl::PortTypeStrings.size(), ll::impl::PortTypeStrings>(lib, "PortType");

    lib.new_usertype<ll::vec3ui>("vec3ui",
        sol::constructors<ll::vec3ui(), ll::vec3ui(const uint32_t&), ll::vec3ui(const uint32_t&, const uint32_t&, const uint32_t&)>(),
        "x", &ll::vec3ui::x,
        "y", &ll::vec3ui::y,
        "z", &ll::vec3ui::z);

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
