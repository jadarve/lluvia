/**
@file       Interpreter.h
@brief      Interpreter class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_INTERPRETER_H_
#define LLUVIA_CORE_INTERPRETER_H_

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>


namespace sol {
class state;

} // namespace sol;

namespace ll {


class Interpreter {

public:
    Interpreter();
    Interpreter(const Interpreter& interpreter) = default;
    Interpreter(Interpreter&& interpreter)      = default;

    ~Interpreter();

    Interpreter& operator = (const Interpreter& interpreter) = default;
    Interpreter& operator = (Interpreter&& interpreter)      = default;

    void run(const std::string& code);
    void runFile(const std::string& filename);

private:
    std::unique_ptr<sol::state> m_lua;
};


} // namespace ll;

#endif // LLUVIA_CORE_INTERPRETER_H_