/**
@file       Interpreter.cpp
@brief      Interpreter class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/impl/Interpreter.h"

#include <chaiscript/chaiscript.hpp>
#include <chaiscript/dispatchkit/bootstrap_stl.hpp>
#include <chaiscript/dispatchkit/function_call.hpp>


namespace ll {
namespace impl {


Interpreter::Interpreter() {
    
    chai = std::make_unique<chaiscript::ChaiScript>();
}

} // namesace impl
} // namespace ll
