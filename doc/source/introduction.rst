Introduction
============

Designing and developing real-time Computer Vision algorithms is a difficult task. On one hand, one needs to formulate the algorithm mathematically in such a way that enables fast calculations on modern compute hardware. On the other hand, such mathematical formulation has to be transformed into machine code, optimized and tested to satisfy product requirements. Accomplishing both tasks successfully is the job of research and engineering teams with highly specialized knowledge.


Lluvia is a cross-platform GPU compute engine designed for Computer Vision applications. It is built on top of the Vulkan graphics and compute API, which enables it to run compute tasks on any Vulkan compatible GPU.

The core libraries are developed in C++. This runtime offers a low-overhead abstraction around Vulkan API complexities.

Algorithms are developed by coding compute shaders in GLSL and connecting them together to form a compute graph.

The engine is designed for real-time applications

Nodes are described using two files: one compute shader written in GSLS for the GPU compute side and a Lua script that binds it with other nodes in the graph.

**Figure:** a graph diagram.


Lluvia's runtime is written in C++. It offers bindings for Python and Kotlin (coming soon).

Graphs prototyped using the Python or Kotlin API can be run using the C++ runtime with no changes to the shader or Lua code, thus boosting the runtime performance of the algorithms as well as minimizing the engineering effort of re-writing the same algorithm twice.


Design Principles
-----------------

1. **Runtime performance is critical:** Robotic applications, for instance, require that short response times of the vision pipeline in order for the processed information to be usable.

2. **Bulk of computations are done in the GPU:** Most commonly used operations in Computer Vision such as image filtering, geometric transforms, and pixel-based operations, can nicely be mapped to the GPU parallel architecture.

3. **Keep non-advanced users away from C++ as much as possible:** Experienced CV scientist and engineers are not always proficient in low-level system programming (i.e., C/C++). While they can produce Computer Vision algorithm prototypes to proof an idea, taking that idea to production can in many cases require a complete rework of the code thus slowing the time-to-market of new ideas. Graphs prototyped in one of Lluvia's high-level language bindings (Python) can be taken and run directly on the C++ runtime with little effort. However, advanced users still have access to the low-level C++ API to further optimize their pipelines.

3. **C++ API for the advanced users:**

4. **Lower the time from prototype to production:** Once algorithms are designed and tested using a high-level language binding such as Python, it should be relatively straight forward to migrate the algorithm to the native C++ runtime of the platform (Android, Linux, etc).


Workflow
--------

With lluvia, it is possible to create a workflow on which prototyping Computer Vision algorithms can be performed on a high-level dynamic language such as Python. In there, one can use tools such as Numpy and Matplotlib for debugging the compute kernels.

1. Define a compute node and describe its inputs, outputs and parameters. This is done using Lua programming language.

2. Code the node's compute shader in GLSL.

3. Test the node.

The output of this process is a Lua file describing the node's ports and parameters and a GLSL file with the compute shader. These files can then be connected with other nodes and run on any platform.


Alternatives
------------

* Halide.
* Mediapipe.
* OpenCV.
