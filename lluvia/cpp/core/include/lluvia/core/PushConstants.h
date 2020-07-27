/**
@file       PushConstants.h
@brief      PushConstants class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_PUSH_CONSTANTS_H_
#define LLUVIA_CORE_PUSH_CONSTANTS_H_

#include "lluvia/core/error.h"

#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

namespace ll {


class PushConstants {

public:
    PushConstants()                                  = default;
    PushConstants(const PushConstants&)              = default;
    PushConstants(PushConstants&&)                   = default;

    ~PushConstants()                                 = default;

    PushConstants& operator = (const PushConstants&) = default;
    PushConstants& operator = (PushConstants&&)      = default;

    template<typename T>
    void set(T&& data) {

        const auto dataSize = sizeof(data);

        if (dataSize != m_data.capacity()) {
            m_data.resize(dataSize);
        }

        std::memcpy(static_cast<void*>(&m_data[0]), &data, dataSize);
    }

    template<typename T>
    T get() const {
        
        if (sizeof(T) != m_data.size()) {
            ll::throwSystemError(ll::ErrorCode::PushConstantError,
                "Size of receiving object must be equal to the size of internal buffer, expected: " 
                + std::to_string(m_data.size())
                + " got: " + std::to_string(sizeof(T)));
        }

        T out{};
        std::memcpy(&out, &m_data[0], sizeof(out));

        return out;
    }

    template<typename T>
    void push(T&& data) {

        const auto currentSize = getSize();
        if (currentSize == 0) {
            set(std::forward<T>(data));
            return;
        }

        const auto dataSize = sizeof(data);

        auto new_data = std::vector<uint8_t>(currentSize + dataSize);
        std::memcpy(&new_data[0], &m_data[0], currentSize);
        std::memcpy(&new_data[currentSize], &data, dataSize);

        m_data = std::move(new_data);
    }

    
    inline void* getPtr() const noexcept {
        return (void*)(&m_data[0]);
    }

    size_t getSize() const noexcept {
        return m_data.size();
    }

    void pushFloat(const float& d) {push(d);};
    void setFloat(const float& d) {set(d);}
    float getFloat() const {return get<float>();}

    void pushInt32(const int32_t& d) {push(d);};
    void setInt32(const int32_t &d) { set(d); }
    int32_t getInt32() const { return get<int32_t>();}

private:
    std::vector<uint8_t> m_data {};
};

} // namespace ll

#endif // LLUVIA_CORE_PUSH_CONSTANTS_H_