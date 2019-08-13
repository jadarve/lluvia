/**
@file       Interpreter.cpp
@brief      Interpreter class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Interpreter.h"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace ll {

Interpreter::Interpreter() :
    m_lua {std::make_unique<sol::state>()} {

    // load default libraries
    m_lua->open_libraries(sol::lib::base, sol::lib::math);

    // TODO: register types
}


Interpreter::~Interpreter() {

}


void Interpreter::run(const std::string& code) {

    m_lua->script(code);
}

} // namespace ll
