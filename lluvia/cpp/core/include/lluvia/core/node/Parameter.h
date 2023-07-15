/**
@file       Parameter.h
@brief      Parameter class and related enums.
@copyright  2022, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NODE_PARAMETER_H_
#define LLUVIA_CORE_NODE_PARAMETER_H_

#include "lluvia/core/error.h"
#include "lluvia/core/node/ParameterType.h"

#include <array>
#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ll {

class Parameter {

public:
    Parameter()                 = default;
    Parameter(const Parameter&) = default;
    Parameter(Parameter&&)      = default;

    Parameter(ParameterType type);

    ~Parameter() = default;

    Parameter& operator=(const Parameter&) = default;
    Parameter& operator=(Parameter&&)      = default;

    ll::ParameterType getType() const noexcept;

    template <typename T>
    void set(const T& value)
    {

        if constexpr (std::is_integral_v<T>) {
            m_type        = ll::ParameterType::Int;
            m_value.v_int = static_cast<int32_t>(value);
        } else if constexpr (std::is_floating_point_v<T>) {
            m_type          = ll::ParameterType::Float;
            m_value.v_float = static_cast<float>(value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            m_type           = ll::ParameterType::String;
            m_value.v_string = std::string {value};
        } else {
            // at least we are sure the first if is false
            static_assert(!std::is_integral_v<T>, "type T does not match any of the supported types");
        }
    }

    template <typename T>
    const T get() const
    {

        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_same_v<T, bool>) {

            switch (m_type) {
            case ParameterType::Int:
                return static_cast<int32_t>(m_value.v_int);
            case ParameterType::Float:
                return static_cast<float>(m_value.v_float);
            case ParameterType::String:
                throw std::system_error(createErrorCode(ll::ErrorCode::InvalidParameterType),
                    "String parameter type not supported when casting to integral or floating point types");
            }
        } else if constexpr (std::is_same_v<T, std::string>) {

            if (m_type == ParameterType::String) {
                return m_value.v_string;
            } else {
                throw std::system_error(createErrorCode(ll::ErrorCode::InvalidParameterType),
                    "String parameter type not supported when casting to integral or floating point types");
            }

        } else {
            // at least we are sure the first if is false
            static_assert(!std::is_integral_v<T>, "type T does not match any of the supported types");
        }
    }

private:
    ll::ParameterType m_type {ll::ParameterType::Int};

    struct {
        union {
            int32_t v_int;
            float   v_float;
        };

        // complex types cannot go into unions
        std::string v_string;
    } m_value;
};

} // namespace ll

#endif // LLUVIA_CORE_NODE_PARAMETER_H_
