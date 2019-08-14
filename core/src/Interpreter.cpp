/**
@file       Interpreter.cpp
@brief      Interpreter class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Interpreter.h"

#include "lluvia/core/ComputeNodeDescriptor.h"
#include "lluvia/core/types.h"


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-lambda-capture"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#pragma clang diagnostic pop

namespace ll {


void registerTypes(sol::table& lib) {

    lib.new_usertype<ll::vec3ui>("vec3ui",
        "x", &ll::vec3ui::x,
        "y", &ll::vec3ui::y,
        "z", &ll::vec3ui::z);

    lib.new_usertype<ll::ComputeNodeDescriptor>("ComputeNodeDescriptor",
        "functionName", sol::property(&ll::ComputeNodeDescriptor::getFunctionName, &ll::ComputeNodeDescriptor::setFunctionName)
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

} // namespace ll
