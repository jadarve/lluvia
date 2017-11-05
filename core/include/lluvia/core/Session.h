#ifndef LLUVIA_CORE_SESSION_H_
#define LLUVIA_CORE_SESSION_H_

#include "lluvia/core/Buffer.h"
#include "lluvia/core/Memory.h"
#include "lluvia/core/Shader.h"

#include <vulkan/vulkan.hpp>

#include <memory>
#include <string>
#include <tuple>
#include <vector>


namespace ll {

/**
 * \brief The Session class contains all the state required to run compute Graphs.
 */
class Session {

public:
    Session();
    Session(const Session& session)              = default;
    Session(Session&& session)                   = default;

    ~Session();

    Session& operator = (const Session& session) = default;
    Session& operator = (Session&& session)      = default;

    std::vector<vk::MemoryPropertyFlags> getSupportedMemoryFlags() const;

    /**
     * Exceptions:
     * std::bad_alloc
     */
    std::tuple<bool, uint32_t> configureMemory(const vk::MemoryPropertyFlags flags, const uint64_t pageSize);

    // ll::Memory createMemory(const vk::MemoryPropertyFlags flags, const uint64_t pageSize);
    std::unique_ptr<ll::Memory> createMemory(const vk::MemoryPropertyFlags flags, const uint64_t pageSize);

    ll::Buffer createBuffer(const uint32_t memoryIndex, const size_t size);

    // std::tuple<bool, ll::Shader> createShader(const std::string& spirvPath) const;

private:
    bool initInstance();
    bool initDevice();
    bool initQueue();
    uint32_t getComputeFamilyQueueIndex();

    vk::Instance         instance;
    vk::PhysicalDevice   physicalDevice;
    vk::Device           device;
    vk::Queue            queue;
    uint32_t             computeQueueFamilyIndex;

    std::shared_ptr<int> referenceCounter {nullptr};
};

} // namespace ll

#endif // LLUVIA_CORE_SESSION_H_
