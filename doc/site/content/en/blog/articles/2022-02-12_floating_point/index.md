---
date: 2022-02-12
title: "Working with floating point precision"
linkTitle: "Working with floating point precision"
description: "Discusses how to use different floating point precision available in our device, and how to take advange of smaller representations to improve runtime performance."
author: Juan Adarve
resources:
- src: "**.{png,jpg}"
  title: "Image #:counter"
---

GPU devices support several working with floating point numbers of several precisions. Here precision refers to the number of bits used for representing a given number. Typical representations are:

* **`FP16`**: or half precision. Numbers are represented in 16 bits.
* **`FP32`**: or single precision. It uses 32 bits for representing a number.
* **`FP64`**: or doble precision. 64 bits are used for represeting a number.

Where **`FP64`** is used when numerical precision is required, **`FP16`** is suitable for fast, less exact calculations, and **`FP32`** sits in the middle. The [IEEE 754 standard][3] defines the specification of floating point numbers used in modern computers. It defines the rules for interpreting the bit fields that form a number, as well as the arithmetic rules to process them.

The Vulkan API offers support for the three floating point precisions. However, not all GPUs support every format. The [Vulkan GPU Info page][2] is great tool to check support for a given feature.

## Improvements in runtime performance

Smaller bit representation of floating point numbers have an advantage in terms of runtime performance. Consider the case of a **RGBA** image. If the image channel type is `ll.ChannelType.Float16`, the four pixel values will fit in 8 bytes, compared to 16 bytes if `ll.ChannelType.Float32` was used. This reduction in memory footprint increases the pixel transfer rate from memory to the compute device.

To illustrate this, let's consider the [*optical flow filter* node](nodes/lluvia/opticalflow/flowfilter/flowfilter/). The code below configures the flowfilter algorithm both with `ll.FloatPrecision.FP16` and `ll.FloatPrecision.FP32`, it runs each node for `N = 10000` times and collects the runtime using the `duration` probe.

{{< tabpane >}}
{{< tab header="Python" lang="python">}}
import lluvia as ll
import numpy as np

session = ll.createSession()
memory = session.createMemory([ll.MemoryPropertyFlagBits.DeviceLocal])

host_rgba = np.zeros((1016, 544, 4), dtype=np.uint8)
in_rgba = memory.createImageViewFromHost(host_rgba)

RGBA2Gray = session.createComputeNode('lluvia/color/RGBA2Gray')
RGBA2Gray.bind('in_rgba', in_rgba)
RGBA2Gray.init()

N = 10000
runtimeMilliseconds = {
    ll.FloatPrecision.FP16 : np.zeros((N), dtype=np.float32),
    ll.FloatPrecision.FP32 : np.zeros((N), dtype=np.float32)
}

for precision in [ll.FloatPrecision.FP32, ll.FloatPrecision.FP16]:

    flowFilter = session.createContainerNode('lluvia/opticalflow/flowfilter/FlowFilter')
    flowFilter.setParameter('levels',            ll.Parameter(2))
    flowFilter.setParameter('max_flow',          ll.Parameter(2))
    flowFilter.setParameter('smooth_iterations', ll.Parameter(2))
    flowFilter.setParameter('gamma',             ll.Parameter(0.0005))
    flowFilter.setParameter('gamma_low',         ll.Parameter(0.0005))
    
    # use selected floating point precision
    flowFilter.setParameter('float_precision',   ll.Parameter(precision.value))
    
    flowFilter.bind('in_gray', RGBA2Gray.getPort('out_gray'))
    flowFilter.init()

    duration = session.createDuration()

    cmdBuffer = session.createCommandBuffer()
    cmdBuffer.begin()
    cmdBuffer.run(RGBA2Gray)
    cmdBuffer.memoryBarrier()

    # probe the runtime of the flowfilter node
    cmdBuffer.durationStart(duration)
    cmdBuffer.run(flowFilter)
    cmdBuffer.memoryBarrier()
    cmdBuffer.durationEnd(duration)

    cmdBuffer.end()
    
    # run the command buffer N times and collect the runtime of the flow algorithm
    for n in range(N):
        session.run(cmdBuffer)
        runtimeMilliseconds[precision][n] = duration.nanoseconds / 1e6
{{< /tab >}}
{{< /tabpane >}}

Here, the `ll.FloatPrecision.FP16, ll.FloatPrecision.FP32` are new enum values for representing 16-bit and 32-bit floating point precision, respectively. The line `flowFilter.setParameter('float_precision', ll.Parameter(precision.value))` configures the node with the given precision. Internally, the `float_precision` is used to instanciate any floating point image with the requested precision.

{{< alert title="Note:" >}}
By convention, any node that allows selecting floating point precision will define the **`float_precision`** parameter and will expect one of the `ll.FloatPrecision` enum values.
{{< /alert >}}


The figure below shows the collected runtime for both floating point precisions. The median runtime for **`FP16`** is 0.501ms, and for **`FP32`** is 0.770ms. That is, the **`FP16`** algorithm improves the runtime by 35% compared to **`FP32`**.

{{< imgproc flowfilter_fp_comparison Fill "1080x360" >}}
Optical flow filter runtime using FP16 and FP32 floating point precision.
{{< /imgproc >}}

## Modifications to GLSL shader code

In terms of GLSL shader code, there are no changes to make to support FP16 or FP32 imagse. However, it is important to understand the underlying functioning. For instance, consider the GLSL implementation of the [`RGBA2HSVA`](/nodes/lluvia/color/rgba2hsva/) compute node. Notice that the **`out_hsva`** port is bound to the shader as a `rgba32f` image:

{{< tabpane >}}
{{< tab header="GLSL" lang="glsl">}}
#version 450

#include <lluvia/core.glsl>
#include <lluvia/core/color.glsl>

layout(binding = 0, rgba8ui) uniform uimage2D in_rgba;
layout(binding = 1, rgba32f) uniform writeonly image2D  out_hsva;

layout(push_constant) uniform const_0 {
    float min_chroma;
} params;

void main() {

    const float min_chroma = params.min_chroma;

    const ivec2 coords  = LL_GLOBAL_COORDS_2D;
    const ivec2 imgSize = imageSize(out_hsva);

    if (coords.x > imgSize.x || coords.y > imgSize.y) {
        return;
    }

    const uvec4 RGBA = imageLoad(in_rgba, coords);
    const vec4  HSVA = color_rgba2hsva(RGBA, min_chroma);

    imageStore(out_hsva, coords, HSVA);
}
{{< /tab >}}
{{< /tabpane >}}

Images compatible with the `rgba32f` image format can be bound as output. The [shader image load store extension][1] defines the compatibility rules to be able to bind images to shaders. For this case in particular, it is possible to bind either a `rgba16f` or `rgba32f` images to the output. The shader will execute all arithmetic operations using 32-bit floating point precision. When storing an image texel using `imageStore(out_hsva, coords, HSVA);`, the shader will reinterpret it the `HSVA` `vec4` either as a 16 or 32 bit floating vector, according to the image bound to `out_hsva`.

{{< alert >}}
The [shader image load store extension](https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shader_image_load_store.txt) describes the way texels are re-interepret during load/store operations.
{{< /alert >}}

In terms of *Lua* code to describe the node, these are the considerations to support different precisions:

* Define the `float_precision` parameter with default value to `ll.FloatPrecision.FP32`.
* Allocate the node objects according to the selected precision.

{{< tabpane >}}
{{< tab header="Lua" lang="lua">}}
local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/color/RGBA2HSVA'

function builder.newDescriptor()

    local desc = ll.ComputeNodeDescriptor.new()
    desc:init(builder.name, ll.ComputeDimension.D2)

    -- define the float_precision parameter with default value
    desc:setParameter('float_precision', ll.FloatPrecision.FP32)

    local in_rgba = ll.PortDescriptor.new(0, 'in_rgba', ll.PortDirection.In, ll.PortType.ImageView)
    in_rgba:checkImageChannelCountIs(ll.ChannelCount.C4)
    in_rgba:checkImageChannelTypeIs(ll.ChannelType.Uint8)

    desc:addPort(in_rgba)
    desc:addPort(ll.PortDescriptor.new(1, 'out_hsva', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)

    local in_rgba = node:getPort('in_rgba')

    -- receive the selected float_precision
    local float_precision = node:getParameter('float_precision')

    -- transform float precision to a suitable image channel type
    local outImageChannelType = ll.floatPrecisionToImageChannelType(float_precision)

    -------------------------------------------------------
    -- allocate out_hsva
    -------------------------------------------------------
    local imgDesc = ll.ImageDescriptor.new()
    imgDesc.width = in_rgba.width
    imgDesc.height = in_rgba.height
    imgDesc.depth = in_rgba.depth
    imgDesc.channelCount = ll.ChannelCount.C4
    imgDesc.channelType = outImageChannelType

    local imgViewDesc = ll.ImageViewDescriptor.new()
    imgViewDesc.filterMode = ll.ImageFilterMode.Nearest
    imgViewDesc.normalizedCoordinates = false
    imgViewDesc.isSampled = false
    imgViewDesc:setAddressMode(ll.ImageAddressMode.Repeat)

    -- ll::Memory where out_hsva will be allocated
    local memory = in_rgba.memory
    local out_hsva = memory:createImageView(imgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_hsva:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_hsva', out_hsva)
    node:configureGridShape(ll.vec3ui.new(out_hsva.width, out_hsva.height, 1))
end

-- register builder in the system
ll.registerNodeBuilder(builder)
{{< /tab >}}
{{< /tabpane >}}

In there, line `local outImageChannelType = ll.floatPrecisionToImageChannelType(float_precision)` transforms the recevied `ll.FloatPrecision` value to the corresponding `ll.ChannelType` needed to create `out_hsva`. After the `out_hsva` image is created, it is bound to the node and is ready for running.


## Discussion

There are several floating point precisions available to use in compute shaders: **`FP16`**, **`FP132`**, and **`FP64`**, are the ones more commonly available in commodity GPU hardware. The ability to control the underlying floating point precision used in compute pipelines can enable improvements in runtime performance, as the transfer rate of data from and to memory can increase. The choice of a given precision must be made carefully, as it might affect the accuracy of the algorithm.

## References

* [IEEE 754 standard][3]
* [Vulkan GPU Info page][2]
* [OpenGL shader image load-store extension][1]

[1]: https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shader_image_load_store.txt
[2]: http://vulkan.gpuinfo.org/listdevices.php
[3]: https://en.wikipedia.org/wiki/IEEE_754
