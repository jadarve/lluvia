#include "lluvia/core/Duration.h"

namespace ll {

Duration::Duration(const vk::Device &device):
    m_device {device} {

    // there will be two queries, one for the start time
    // and another one for the end time.
    auto desc = vk::QueryPoolCreateInfo()
                    .setQueryType(vk::QueryType::eTimestamp)
                    .setQueryCount(2);

    m_queryPool = m_device.createQueryPool(desc);
}


Duration::~Duration() {
    m_device.destroyQueryPool(m_queryPool);
}


std::chrono::nanoseconds Duration::getDuration() const {

    auto queryData = std::array<int64_t, 2> {};

    m_device.getQueryPoolResults(
        m_queryPool,
        getStartTimeQueryIndex(),
        uint32_t{2},
        sizeof(int64_t) * queryData.size(),
        queryData.data(),
        sizeof(int64_t),
        vk::QueryResultFlagBits::e64
    );

    return std::chrono::nanoseconds(queryData[1] - queryData[0]);
}


vk::QueryPool Duration::getQueryPool() const noexcept {
    return m_queryPool;
}


uint32_t Duration::getStartTimeQueryIndex() const noexcept {
    return 0;
}


uint32_t Duration::getEndTimeQueryIndex() const noexcept {
    return 1;
}

} // namespace ll
