# Low Level Unified VIsion Algorithms (LLUVIA)

![Build Linux](https://github.com/jadarve/lluvia/workflows/build/badge.svg) [![Documentation Status](https://readthedocs.org/projects/lluvia/badge/?version=latest)](https://lluvia.io/en/latest/?badge=latest)

Lluvia is a computer vision engined designed for real-time applications. It uses the Vulkan API to access the GPU and dispatch computations. Users can describe computations as a compute pipeline where nodes are compute shaders running on the GPU.

This demo is for my real-time optical flow algorithm running using Lluvia on a GTX-1080 GPU. For images of 1016x544 resolution, computation take around 1 millisecond to complete.

[![Real Time Optical Flow ](http://img.youtube.com/vi/mRZ6YdWb8fE/0.jpg)](https://youtu.be/mRZ6YdWb8fE)

See the **samples** folder for more demos.

# Mode of Use

Lluvia uses Lua as embedded scripting language for connecting nodes together, offloading developers of doing so in C++. A node is made of two source files: a GLSL compute shader and a Lua script that describes its interconnection. These files can then be exported into the engine either using the C++ or Python APIs for later use.

Consider converting from a RGBA image to Gray scale; the GLSL compute shaders looks as follows:

```GLSL
#version 450

#include <lluvia/core.glsl>
#include <lluvia/core/color.glsl>

layout(binding = 0, rgba8ui) uniform uimage2D in_rgba;
layout(binding = 1, r8ui)    uniform uimage2D out_gray;

void main() {

    const ivec2 coords  = LL_GLOBAL_COORDS_2D;
    const ivec2 imgSize = imageSize(out_gray);

    if (coords.x > imgSize.x || coords.y > imgSize.y) {
        return;
    }

    const uvec4 RGBA = imageLoad(in_rgba, coords);
    const uint  gray = color_rgba2gray(RGBA);

    imageStore(out_gray, coords, uvec4(gray));
}

```

while the Lua script that instantiates and configures that shader is:

```Lua
local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'RGBA2Gray'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('RGBA2Gray')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_rgba', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)
    
    local in_rgba = node:getPort('in_rgba')

    -- ll::Memory where out_gray will be allocated
    local memory = in_rgba.memory

    -- instantiate out_gray output
    -- 32-bits floating point output, one color channel, nearest interpolation
    local out_gray = memory:createImageView(
        ll.ImageDescriptor.new(1, in_rgba.height, in_rgba.width, ll.ChannelCount.C1, ll.ChannelType.Float32),
        ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false))

    -- set output in a correct layout and clear its content
    out_gray:changeImageLayout(ll.ImageLayout.General)
    out_gray:clear()
    
    -- bind the output to the node
    node:bind('out_gray', out_gray)
    node:configureGridShape(ll.vec3ui.new(out_gray.width, out_gray.height, 1))
end

ll.registerNodeBuilder('RGBA2Gray', builder)

```

Both files can then be exported into the engine to instantiate as many such nodes as required. Lluvia takes care of recording the operations for running a pipeline and submit it to the GPU efficiently.

# Supported Platforms

* Linux.
* Android (on the work).

# License

Apache 2.0. See [LICENSE](https://github.com/jadarve/lluvia/blob/master/LICENSE) for more details
