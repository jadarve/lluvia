Introduction
============

Designing and developing real-time Computer Vision algorithms is a difficult task. On one hand, one needs to formulate the algorithm mathematically in such a way that enables fast calculations on modern compute hardware. On the other hand, such mathematical formulation has to be transformed into machine code, optimized and tested to satisfy system requirements such as hardware usage and frame rate. Accomplishing both tasks successfully is the job of research and engineering teams with highly specialized knowledge.

Lluvia has been designed around the idea of reducing the effort for designing and implementing Computer Vision algorithms for real-time applications. The engine is built on top the `Vulkan graphics and compute API <https://www.khronos.org/vulkan/>`_. By using Vulkan, it is possible to run the algorithms on any modern GPU that supports the API. The core libraries are coded in C++ and can be compiled for several operating systems, currently Linux and Android. Wrappers for high-level languages such as Python are maintained as well.


Workflow
--------

Lluvia uses a compute graph to organize and schedule computations on the GPU. The development workflow circles around coding and debugging nodes in such a graph until the whole algorithm is built:

1. The node's inputs, outputs and parameters are described in a `Lua script <https://www.lua.org/>`_. This description will later be used to instantiate nodes in the graph.

2. The node's computation in the GPU is coded as a compute shader in `Open GL Shading Language (GLSL) <https://www.khronos.org/opengl/wiki/Core_Language_(GLSL)>`_. Shaders are compiled into `SPIR-V intemediate representation <https://www.khronos.org/registry/spir-v>`_ for later load into the GPU.

3. The node's description and compute shader are loaded into Lluvia's runtime. After this, nodes can be instantiated to build the compute graph and be dispatched to the GPU.

From a user perspective, one needs to only care about describing nodes (inputs, outputs, compute shader, etc.) and connecting nodes to form a graph. Lluvia takes care of the low-level details of dispatching the graph for execution onto the GPU. This workflow allows porting the compute graph from one platform to another will ease.

Check the Getting Started guides in `Python <gettingStarted_python.html>`_ and `C++ <gettingStarted_cpp.html>`_ for examples on how to describe computations in Lluvia.


Alternatives
------------

There are many other alternatives to use for coding and deploying Computer Vision algorithms. The list below is by no means an exhaustive review. Please contact me if you want other frameworks to be included.

* `OpenCV: <https://opencv.org/>`_ The go-to alternative for fast prototyping and deployment of CV algorithms. OpenCV is a mature project that can be used on many platforms (Linux, OSX, Windows, Android). It contains a bast library of algorithms, some of them with GPU implementations.

* `Halide: <https://halide-lang.org/>`_ Halide is a programming language for coding high-performance image processing algorithms. The language is embedded in C++ and can dispatch execution of the algorithms to CPUs and GPUs depending on the available hardware.

* `Mediapipe: <http://mediapipe.dev/>`_ A framework for developing complex Computer Vision pipelines combining several frameworks such as OpenCV, TensorFlow, TFLite.

