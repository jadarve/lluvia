/**
@file       Interpreter.h
@brief      Interpreter class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_INTERPRETER_H_
#define LLUVIA_CORE_INTERPRETER_H_

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace ll {


class Interpreter {

public:
    Interpreter();

    template<typename T>
    void run(T&& code) {
        static_assert(std::is_convertible<T, std::string>::value, "typename T must be convertible to std::string");
        m_lua.script(code);
    }

    template<typename T, typename S>
    T get(S&& name) {
        static_assert(std::is_convertible<S, std::string>::value, "typename S must be convertible to std::string");
        return static_cast<T>(m_lua[name]);
    }

private:
    sol::state m_lua;
};


} // namespace ll;

#endif // LLUVIA_CORE_INTERPRETER_H_