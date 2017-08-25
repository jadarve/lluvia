#ifndef LLUVIA_CORE_SESSION_H_
#define LLUVIA_CORE_SESSION_H_


#include <vector>
#include <memory>

#include <vulkan/vulkan.hpp>

#include "lluvia/core/Memory.h"
#include "lluvia/core/Buffer.h"


namespace ll {


// implementation namespace
namespace impl {

typedef struct {
    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    // vk::Device device;
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
    ~Session();


    bool configureMemory(const vk::MemoryPropertyFlags flags, const uint64_t heapSize);

    // ll::Buffer allocateBuffer(const vk::MemoryPropertyFlags flags, const size_t size);


private:
    void init();
    bool initInstance();
    bool initDevice();
    bool initQueue();
    uint32_t getComputeFamilyQueueIndex();

    std::shared_ptr<impl::SessionHandle> handle;
    std::shared_ptr<vk::Device> device;

    std::vector<ll::Memory> memories;

};

} // namespace ll

#endif // LLUVIA_CORE_SESSION_H_
