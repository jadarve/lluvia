---
title: "Buffer"
date: 2021-06-12T00:00:00-05:00
weight: 1
collapsible: false
---

Buffers are unstructured regions of contiguous memory. Buffers are created from `Memory` objects:

{{< tabpane >}}
{{< tab header="Python" lang="python">}}
import lluvia as ll

session = ll.createSession()

hostMemory = session.createMemory([ll.MemoryPropertyFlagBits.DeviceLocal,
                                   ll.MemoryPropertyFlagBits.HostVisible,
                                   ll.MemoryPropertyFlagBits.HostCoherent])

aBuffer = hostMemory.createBuffer(1024, usageFlags=[ll.BufferUsageFlagBits.TransferDst,
                                                    ll.BufferUsageFlagBits.TransferSrc,
                                                    ll.BufferUsageFlagBits.StorageBuffer])
{{< /tab >}}
{{< tab header="C++" lang="c++">}}
#include <iostream>
#include <vulkan/vulkan.hpp>
#include "lluvia/core.h"

#include <vulkan/vulkan.hpp>

int main() {
    
    ll::SessionDescriptor desc = ll::SessionDescriptor().enableDebug(true);

    std::shared_ptr<ll::Session> session = ll::Session::create(desc);

    hostMemory = session.createMemory([ll.MemoryPropertyFlagBits.DeviceLocal,
                                   ll.MemoryPropertyFlagBits.HostVisible,
                                   ll.MemoryPropertyFlagBits.HostCoherent])

    const auto usageFlags = vk::BufferUsageFlags { vk::BufferUsageFlagBits::eStorageBuffer
                                                 | vk::BufferUsageFlagBits::eTransferSrc
                                                 | vk::BufferUsageFlagBits::eTransferDst};

    auto aBuffer = hostMemory->createBuffer(1024, usageFlags);
}
{{< /tab >}}
{{< /tabpane >}}

The first parameter is the requested size in bytes. The `usageFlags` indicated the intended usage of this buffer; the values are taken directly from the [Vulkan BufferUsageFlagBits](https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkBufferUsageFlagBits.html). The most used values are:

| Flag            | Description                                                                 |
|-----------------|-----------------------------------------------------------------------------|
| `StorageBuffer` | Indicates that the buffer is going to be used for general storage.          |
| `TransferDst`   | Indicates that the buffer can be used as destination for transfer commands. |
| `TransferSrc`   | Indicates that the buffer can be used as source for transfer commands.      |

