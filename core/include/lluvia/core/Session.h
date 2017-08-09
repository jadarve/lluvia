#ifndef LLUVIA_CORE_SESSION_H_
#define LLUVIA_CORE_SESSION_H_


#include <vector>
#include <memory>

#include <vulkan/vulkan.hpp>


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
    ~Session();

private:
    void init();
    bool initInstance();
    bool initDevice();
    bool initQueue();
    uint32_t getComputeFamilyQueueIndex();

    std::shared_ptr<impl::SessionHandle> handle;

};


/**
 * \brief Creates a new Session using default settings.
 */
Session createSession();
    
} // namespace ll

#endif // LLUVIA_CORE_SESSION_H_
