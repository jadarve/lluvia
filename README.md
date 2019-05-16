# Low Level Unified VIsion Algorithms (LLUVIA)

|    |    |
|----|----|
| Cirrus CI | [![Cirrus CI Build Status](https://api.cirrus-ci.com/github/jadarve/lluvia.svg)](https://cirrus-ci.com/github/jadarve/lluvia)  |



# Supported Platforms

* [Linux and Mac OSX](https://github.com/jadarve/lluvia/wiki/Linux-and-Mac-OSX-Build).
* [Android](https://github.com/jadarve/lluvia/wiki/Android-build).


# Overview

Lluvia is a computer vision engine designed for real-time applications. It is coded in C++14 and uses the Vulkan graphics and compute API. Compute kernels are coded in GLSL and can be dispatched to the GPU either using C++ or Python.

For instance, the following compute shader computes the sum of two arrays `C = A + B`

```glsl
#version 450

#include "lluvia/core.glsl"

layout(binding = 0) buffer in0  {int A[]; };
layout(binding = 1) buffer in1  {int B[]; };
layout(binding = 2) buffer out0 {int C[]; };

void main() {

    const uint index = LL_GLOBAL_COORDS_1D;
    C[index] = A[index] + B[index];
}
```

The shader is compiled using **glslc** provided by the [LunarG Vulkan SDK](https://vulkan.lunarg.com/). The resulting SPIRV code and some auxiliary information is stored in a JSON file for easy load from the application.

## Running in C++

The C++ API gives programmers fine control over the different objects required for executing the kernel. The main object is the `session` which controls the communication between the engine and the GPU.

```c++
#include <iostream>
#include <lluvia/core.h>
#include <vulkan/vulkan.hpp>

int main() {
    auto session = ll::Session::create();

    const size_t length = 32;
    const size_t sizeBytes = length * sizeof(int);

    auto hostMemory = session->createMemory(
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits:: eHostCoherent,
        sizeBytes);

    auto A = hostMemory->createBuffer(sizeBytes);
    auto B = hostMemory->createBuffer(sizeBytes);
    auto C = hostMemory->createBuffer(sizeBytes);

    // write some values into the buffers
    {
        auto mapA = A->map<int>();
        auto mapB = B->map<int>();

        for (int i = 0; i < length; ++i) {
            mapA[i] = i;
            mapB[i] = 2*i;
        }

    } // release mapA and mapB

    auto node = session->readComputeNode("add.json");
    node.bind(0, A);
    node.bind(1, B);
    node.bind(2, C);

    session->run(node);

    {
        auto mapC = C->map<int>();

        for (int i = 0; i < length; ++i) {
            std::cout << mapC[i] << std::endl;
        }
    } // release mapC

}
```

# Running in Python

The Python API shares most of the classes defined for the C++ API but simplifies some operations to make them look more Pythonic. This Python package is intended for rapid prototyping.

```python
import numpy as np
import lluvia as ll

code = """
#version 450

layout(binding = 0) buffer in0  {int A[]; };
layout(binding = 1) buffer in1  {int B[]; };
layout(binding = 2) buffer out0 {int C[]; };

void main() {

    const uint index = gl_GlobalInvocationID.x;
    C[index] = A[index] + B[index];
}
"""

length = 32
dtype  = np.int32

session = ll.Session()
memory = session.createMemory()

A = memory.createBufferFromHost(np.arange(0, length, dtype=dtype))
B = memory.createBufferFromHost(np.arange(0, length, dtype=dtype))
C = memory.createBufferFromHost(np.zeros(length, dtype=dtype))

node = session.compileComputeNode(code, ['Buffer', 'Buffer', 'Buffer'])
node.grid = (length, 1, 1)
node.bind(0, A)
node.bind(1, B)
node.bind(2, C)
node.run()

print(C.toHost(dtype=np.int32))
```


# License

Apache 2.0. See [LICENSE](https://github.com/jadarve/lluvia/blob/master/LICENSE) for more details
