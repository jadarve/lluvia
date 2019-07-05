/**
@file       Interpreter.cpp
@brief      Interpreter class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Interpreter.h"

namespace ll {

Interpreter::Interpreter() {

    // default libraries
    m_lua.open_libraries(sol::lib::base, sol::lib::math);
}

} // namespace ll
