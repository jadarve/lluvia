#include "lluvia/core/Duration.h"

#include "lluvia/core/error.h"
#include "lluvia/core/vulkan/Device.h"

namespace ll {

Duration::Duration(const std::shared_ptr<ll::vulkan::Device> device)
    : m_device { std::move(device) }
{

    // there will be two queries, one for the start time
    // and another one for the end time.
    auto desc = vk::QueryPoolCreateInfo()
                    .setQueryType(vk::QueryType::eTimestamp)
                    .setQueryCount(2);

    m_queryPool = m_device->get().createQueryPool(desc);
}

Duration::~Duration()
{
    m_device->get().destroyQueryPool(m_queryPool);
}

std::chrono::nanoseconds Duration::getDuration() const
{

    return std::chrono::nanoseconds(getNanoseconds());
}

int64_t Duration::getNanoseconds() const
{

    auto queryData = std::array<int64_t, 2> {};

    auto result = m_device->get().getQueryPoolResults(
        m_queryPool,
        getStartTimeQueryIndex(),
        uint32_t { 2 },
        sizeof(int64_t) * queryData.size(),
        queryData.data(),
        sizeof(int64_t),
        vk::QueryResultFlagBits::e64 | vk::QueryResultFlagBits::eWait);

    ll::throwSystemErrorIf(result != vk::Result::eSuccess,
        ll::ErrorCode::VulkanError,
        "error obtaining duration value");

    return queryData[1] - queryData[0];
}

vk::QueryPool Duration::getQueryPool() const noexcept
{
    return m_queryPool;
}

uint32_t Duration::getStartTimeQueryIndex() const noexcept
{
    return 0;
}

uint32_t Duration::getEndTimeQueryIndex() const noexcept
{
    return 1;
}

} // namespace ll
