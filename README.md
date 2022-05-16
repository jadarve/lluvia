# Low Level Unified VIsion Algorithms (LLUVIA)

**[lluvia.ai](https://lluvia.ai)**

<!-- ![Linux](https://github.com/jadarve/lluvia/workflows/linux/badge.svg) ![Windows](https://github.com/jadarve/lluvia/workflows/windows/badge.svg) [![Documentation Status](https://readthedocs.org/projects/lluvia/badge/?version=latest)](https://lluvia.io/en/latest/?badge=latest) -->

[![CircleCI](https://circleci.com/gh/jadarve/lluvia/tree/master.svg?style=svg)](https://circleci.com/gh/jadarve/lluvia/tree/master)

Lluvia is a computer vision engined designed for real-time applications. It uses the Vulkan API to access the GPU and dispatch computations. Users can describe computations as a compute pipeline where nodes are compute shaders running on the GPU.

[![Real Time Optical Flow ](http://img.youtube.com/vi/mRZ6YdWb8fE/0.jpg)](https://youtu.be/mRZ6YdWb8fE)

This demo is for my real-time optical flow algorithm running using Lluvia on a GTX-1080 GPU. For images of 1016x544 resolution, computation take around 1 millisecond to complete.


# Supported Platforms

* Linux.
* Windows.
* Android (work in progress).

# Try it out

Try the [examples in Google Colab](https://drive.google.com/drive/folders/19Isz8r22pwjy78lLW4FQiTSY2tIoDXtS?usp=sharing). Make sure the runtime is configured to GPU.

# Workflow

Lluvia uses a compute graph to organize and schedule computations on the GPU. The development workflow circles around coding and debugging nodes in such a graph until the whole algorithm is built:

1. The node's inputs, outputs and parameters are described in a [Lua script](https://www.lua.org) . This description will later be used to instantiate nodes in the graph.

2. The node's computation in the GPU is coded as a compute shader in [Open GL Shading Language (GLSL)](https://www.khronos.org/opengl/wiki/Core_Language_(GLSL)). Shaders are compiled into [SPIR-V intemediate representation](https://www.khronos.org/registry/spir-v) for later load into the GPU.

3. The node's description and compute shader are loaded into Lluvia's runtime. After this, nodes can be instantiated to build the compute graph and be dispatched to the GPU.

From a user perspective, one needs to only care about describing nodes (inputs, outputs, compute shader, etc.) and connecting nodes to form a graph. Lluvia takes care of the low-level details of dispatching the graph for execution onto the GPU. This workflow allows porting the compute graph from one platform to another with ease.


# License

Apache 2.0. See [LICENSE](https://github.com/jadarve/lluvia/blob/master/LICENSE) for more details
