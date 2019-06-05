/**
@file       Interpreter.h
@brief      Interpreter class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_SCRIPTING_INTERPRETER_H_
#define LLUVIA_CORE_SCRIPTING_INTERPRETER_H_

#include <chaiscript/chaiscript.hpp>

#include <memory>

// namespace chaiscript {

// class ChaiScript;

// } // namespace chaiscript


namespace ll {
namespace scripting {


class Interpreter {

public:
    Interpreter();

private:
    std::unique_ptr<chaiscript::ChaiScript> chai;
};

} // namesace scripting
} // namespace ll

#endif // LLUVIA_CORE_SCRIPTING_INTERPRETER_H_
