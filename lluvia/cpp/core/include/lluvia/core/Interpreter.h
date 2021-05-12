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
#include <memory>
#include <string>
#include <type_traits>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-lambda-capture"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#pragma clang diagnostic pop

#include "lluvia/core/error.h"

namespace ll {

class Session;


class Interpreter {

public:
    Interpreter();
    Interpreter(const Interpreter& interpreter) = delete;
    Interpreter(Interpreter&& interpreter)      = default;

    ~Interpreter();

    Interpreter& operator = (const Interpreter& interpreter) = delete;
    Interpreter& operator = (Interpreter&& interpreter)      = default;

    void run(const std::string& code);
    void runFile(const std::string& filename);

    sol::load_result load(const std::string& code);

    void setActiveSession(ll::Session* session);

    template<typename T, typename... Args>
    T loadAndRun(const std::string&& code, Args&&... args) {

        auto loadCode = load(std::forward<const std::string>(code));

        auto scriptFunction = static_cast<sol::protected_function>(loadCode);
        sol::protected_function_result scriptResult = scriptFunction(std::forward<Args>(args)...);

        if (!scriptResult.valid()) {
            const sol::error err = scriptResult;

            ll::throwSystemError(ll::ErrorCode::InterpreterError,
                                 "error running code: " + sol::to_string(loadCode.status()) + "\n\t" + err.what());
        }

        // return static_cast<T>(scriptResult);
        return scriptResult.get<T>();
    }

    template<typename... Args>
    void loadAndRunNoReturn(const std::string&& code, Args&&... args) {

        auto loadCode = load(std::forward<const std::string>(code));

        auto scriptFunction = static_cast<sol::protected_function>(loadCode);
        sol::protected_function_result scriptResult = scriptFunction(std::forward<Args>(args)...);

        if (!scriptResult.valid()) {
            const sol::error err = scriptResult;

            ll::throwSystemError(ll::ErrorCode::InterpreterError,
                                 "error running code: " + sol::to_string(loadCode.status()) + "\n\t" + err.what());
        }
    }

private:
    std::unique_ptr<sol::state> m_lua;
    sol::table                  m_lib;
    sol::table                  m_libImpl;
};


} // namespace ll;

#endif // LLUVIA_CORE_INTERPRETER_H_
