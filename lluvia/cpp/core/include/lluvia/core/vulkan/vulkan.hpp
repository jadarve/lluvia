#ifndef LLUVIA_CORE_VULKAN_VULKAN_HPP_
#define LLUVIA_CORE_VULKAN_VULKAN_HPP_

// Disable some diagnostics before including vulkan.hpp
// This is necessary as the .cpp files of the core library
// have more strict checks.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#pragma clang diagnostic ignored "-pedantic"


// On Android avoid defining the vulkan function prototypes,
// as those are loaded dynamically when the instance is created.
// FIXME: However, disabling the prototypes on Linux fails the Python tests.
#ifdef __ANDROID__
#define VK_NO_PROTOTYPES 1
#endif

#include <vulkan/vulkan.hpp>

#pragma clang diagnostic pop

#endif // LLUVIA_CORE_VULKAN_VULKAN_HPP_
