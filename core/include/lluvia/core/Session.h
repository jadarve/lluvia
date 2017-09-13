#ifndef LLUVIA_CORE_SESSION_H_
#define LLUVIA_CORE_SESSION_H_

#include <memory>
#include <vector>
#include <tuple>

#include <vulkan/vulkan.hpp>

#include "lluvia/core/Memory.h"
#include "lluvia/core/Buffer.h"


namespace ll {


// implementation namespace
namespace impl {

typedef struct {
    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    vk::Queue queue;
    uint32_t computeQueueFamilyIndex;

} SessionHandle;


} // namespace impl



/**
 * \brief The Session class contains all the state required to run compute Graphs.
 */
class Session {

public:
    Session();
    Session(const Session& session) = default;
    Session(Session&& Memory)       = default;

    ~Session();

    Session& operator = (const Session& session) = default;
    Session& operator = (Session&& session)      = default;

    std::vector<vk::MemoryPropertyFlags> getSupportedMemoryFlags() const;

    /**
     * Exceptions:
     * std::bad_alloc
     */
    std::tuple<bool, uint32_t> configureMemory(const vk::MemoryPropertyFlags flags, const uint64_t pageSize);

    // ll::Buffer allocateBuffer(const vk::MemoryPropertyFlags flags, const size_t size);
    // ll::Buffer allocateBuffer(const uint32_t memoryIndex, const size_t size);


private:
    void init();
    bool initInstance();
    bool initDevice();
    bool initQueue();
    uint32_t getComputeFamilyQueueIndex();

    std::shared_ptr<impl::SessionHandle> handle;

    std::vector<ll::Memory> memories;

};

} // namespace ll

#endif // LLUVIA_CORE_SESSION_H_
