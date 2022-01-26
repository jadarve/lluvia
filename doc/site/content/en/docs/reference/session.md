---
title: "Session"
linkTitle: "Session"
date: 2021-06-12T00:00:00-05:00
weight: 1
collapsible: false
# description: "How to create a session."
---

A `Session` is the main object in a Lluvia application. It holds the references to the underlying GPU devices used for computation:

{{< tabpane >}}
{{< tab header="Python" lang="python">}}
import lluvia as ll

session = ll.createSession(enableDebug=True)
{{< /tab >}}
{{< tab header="C++" lang="c++">}}
#include <lluvia/core.h>

#include <memory>

int main() {

    auto desc = ll::SessionDescriptor().enableDebug(true);

    std::shared_ptr<ll::Session> session = ll::Session::create(desc);

    return 0;
}
{{< /tab >}}
{{< /tabpane >}}

The `enableDebug` flag enables the [Vulkan validation layers](https://github.com/KhronosGroup/Vulkan-ValidationLayers/blob/master/docs/khronos_validation_layer.md) for receiving messages about bad usage of the API. This can be useful while building your compute pipelines, but should be disabled in Production for reducing the communication overhead with the GPU.

Several object types are creating from a session, among the most important are:

```mermaid
graph
    Session --> Memory
    Session --> Program
    Session --> CommandBuffer
    Session --> Duration
    Session --> ComputeNode
    Session --> ContainerNode
```


## What's next

Check the [Memory](/docs/reference/memory) page to know about the different memory types in Lluvia.
