/**
@file       Interpreter.cpp
@brief      Interpreter class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/scripting/Interpreter.h"

#include <chaiscript/chaiscript.hpp>
#include <chaiscript/dispatchkit/bootstrap_stl.hpp>
#include <chaiscript/dispatchkit/function_call.hpp>


namespace ll {
namespace scripting {


Interpreter::Interpreter() {
    
    chai = std::make_unique<chaiscript::ChaiScript>();

    chai->add(chaiscript::bootstrap::standard_library::vector_type<std::vector<std::string>>("vector_string"));
    chai->add(chaiscript::bootstrap::standard_library::vector_type<std::vector<uint32_t>>("vector_uint32_t"));

    chai->add(chaiscript::vector_conversion<std::vector<std::string>>());

    
    // chai.add(chaiscript::fun(&imgdesc::width), "width");
    // chai.add(chaiscript::fun(&imgdesc::height), "height");
    // chai.add(chaiscript::fun(&imgdesc::channels), "channels");
    // chai.add(chaiscript::user_type<imgdesc>(), "imgdesc");

    // chai.add(chaiscript::fun(&bufferDesc::size), "size");
    // chai.add(chaiscript::fun(&bufferDesc::usageFlags), "usageFlags");
    // chai.add(chaiscript::user_type<bufferDesc>(), "bufferDesc");
}

} // namesace scripting
} // namespace ll
