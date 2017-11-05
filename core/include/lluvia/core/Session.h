#ifndef LLUVIA_CORE_SESSION_H_
#define LLUVIA_CORE_SESSION_H_

#include "lluvia/core/Buffer.h"
#include "lluvia/core/Memory.h"
#include "lluvia/core/Program.h"

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
    static std::unique_ptr<ll::Session> create();

    Session(const Session& session)              = delete;
    Session(Session&& session)                   = delete;

    ~Session();

    Session& operator = (const Session& session) = delete;
    Session& operator = (Session&& session)      = delete;

    std::vector<vk::MemoryPropertyFlags> getSupportedMemoryFlags() const;

    std::unique_ptr<ll::Memory> createMemory(const vk::MemoryPropertyFlags flags, const uint64_t pageSize) const;

    std::unique_ptr<ll::Program> createProgram(const std::string& spirvPath) const;

private:
    // Session objects should be created through factory methods
    Session();

    bool initInstance();
    bool initDevice();
    bool initQueue();
    uint32_t getComputeFamilyQueueIndex();

    vk::Instance         instance;
    vk::PhysicalDevice   physicalDevice;
    vk::Device           device;
    vk::Queue            queue;
    uint32_t             computeQueueFamilyIndex;
};

} // namespace ll

#endif // LLUVIA_CORE_SESSION_H_
