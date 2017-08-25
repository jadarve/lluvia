#include "lluvia/core/Session.h"

#include <exception>
#include <iostream>

namespace ll {

using namespace std;

Session::Session() {

    handle = std::make_shared<impl::SessionHandle>();
    device = std::make_shared<vk::Device>();
    init();
}


Session::~Session() {

    if(handle.use_count() == 1) {

        // destroy all buffers
        // destroy memory managers

        device->destroy();
        handle->instance.destroy();
    }
}


bool configureMemory(const vk::MemoryPropertyFlags flags, const uint64_t heapSize) {

    return true;
}


void Session::init() {

    bool instanceCreated = false;
    bool deviceCreated = false;

    try {

        instanceCreated = initInstance();
        deviceCreated = initDevice();
        initQueue();

    } catch(...) {

        if(deviceCreated) {
            device->destroy();
        }

        if(instanceCreated) {
            handle->instance.destroy();
        }

        // rethrow
        throw;
    }
    
}


bool Session::initInstance() {
    
    const vk::ApplicationInfo appInfo = vk::ApplicationInfo()
            .setPApplicationName("computeKit")
            .setApplicationVersion(0)
            .setEngineVersion(0)
            .setPEngineName("lluvia")
            .setApiVersion(VK_MAKE_VERSION(1, 0, 54));

    const vk::InstanceCreateInfo instanceInfo = vk::InstanceCreateInfo()
            .setPApplicationInfo(&appInfo);

    vk::Result result = vk::createInstance(&instanceInfo, nullptr, &handle->instance);

    if(result == vk::Result::eErrorIncompatibleDriver) {
        throw std::system_error(std::error_code(), "Incompatible driver");
    }

    // TODO: let user to choose physical device
    handle->physicalDevice = handle->instance.enumeratePhysicalDevices()[0];

    return true;
}


bool Session::initDevice() {
    
    float queuePriority = 1.0f;
    handle->computeQueueFamilyIndex = getComputeFamilyQueueIndex();

    vk::DeviceQueueCreateInfo devQueueCreateInfo = vk::DeviceQueueCreateInfo()
        .setQueueCount(1)
        .setQueueFamilyIndex(handle->computeQueueFamilyIndex)
        .setPQueuePriorities(&queuePriority);

    vk::DeviceCreateInfo devCreateInfo = vk::DeviceCreateInfo()
            .setQueueCreateInfoCount(1)
            .setPQueueCreateInfos(&devQueueCreateInfo);

    *device = handle->physicalDevice.createDevice(devCreateInfo);
    return true;
}


bool Session::initQueue() {
    
    // get the first compute capable queue
    handle->queue = device->getQueue(handle->computeQueueFamilyIndex, 0);
    return true;
}


uint32_t Session::getComputeFamilyQueueIndex() {
    
    std::vector<vk::QueueFamilyProperties> queueProperties = handle->physicalDevice.getQueueFamilyProperties();

    uint32_t queueIndex = 0;
    for(auto prop : queueProperties) {
        
        auto compute = ((prop.queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute);
        if(compute != 0) {
            return queueIndex;
        }

        ++queueIndex;
    }

    // FIXME: change to Lluvia exception.
    throw std::system_error(std::error_code(), "No compute capable queue family found.");
}

} // namespace ll