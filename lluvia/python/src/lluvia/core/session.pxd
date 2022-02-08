"""
    lluvia.core.session
    -------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint64_t

from libcpp cimport bool
from libcpp.memory cimport unique_ptr
from libcpp.memory cimport shared_ptr
from libcpp.string cimport string
from libcpp.vector cimport vector

from lluvia.core.memory.memory cimport _Memory
from lluvia.core.memory.memory_property_flags cimport _MemoryPropertyFlags

from lluvia.core.command_buffer cimport _CommandBuffer
from lluvia.core.compute_dimension cimport _ComputeDimension
from lluvia.core.duration cimport _Duration

from lluvia.core.node_new.compute_node cimport _ComputeNode
from lluvia.core.node_new.compute_node_descriptor cimport _ComputeNodeDescriptor
from lluvia.core.node_new.node_builder_descriptor cimport _NodeBuilderDescriptor
from lluvia.core.node_new.container_node_descriptor cimport _ContainerNodeDescriptor

from lluvia.core.device.device_descriptor cimport _DeviceDescriptor
from lluvia.core.node cimport _ContainerNode
from lluvia.core.program cimport _Program
from lluvia.core.types cimport _vec3ui

cdef extern from 'lluvia/core/SessionDescriptor.h' namespace 'll':

    cdef cppclass _SessionDescriptor 'll::SessionDescriptor':

        _SessionDescriptor()

        _SessionDescriptor& enableDebug(bool enable)
        bool isDebugEnabled()

        _SessionDescriptor& setDeviceDescriptor(const _DeviceDescriptor& deviceDescriptor)


cdef extern from 'lluvia/core/Session.h' namespace 'll':

    cdef cppclass _Session 'll::Session':

        @staticmethod
        shared_ptr[_Session] create()

        @staticmethod
        shared_ptr[_Session] create(const _SessionDescriptor& descriptor) except +

        @staticmethod
        vector[_DeviceDescriptor] getAvailableDevices() except +

        vector[_MemoryPropertyFlags] getSupportedMemoryFlags() const

        const _DeviceDescriptor& getDeviceDescriptor()

        shared_ptr[_Memory] getHostMemory() except +

        shared_ptr[_Memory] createMemory(const _MemoryPropertyFlags& flags, const uint64_t pageSize, bool exactFlagsMatch) except +
        shared_ptr[_Program] createProgram(const string& spirvPath) except +

        _ComputeNodeDescriptor createComputeNodeDescriptor(const string& builderName) except +
        shared_ptr[_ComputeNode] createComputeNode(const _ComputeNodeDescriptor& descriptor) except +
        shared_ptr[_ComputeNode] createComputeNode(const string& builderName) except +

        _ContainerNodeDescriptor createContainerNodeDescriptor(const string& builderName) except +
        shared_ptr[_ContainerNode] createContainerNode(const _ContainerNodeDescriptor& descriptor) except +
        shared_ptr[_ContainerNode] createContainerNode(const string& builderName) except +

        void setProgram(const string& name, const shared_ptr[_Program]& program)
        shared_ptr[_Program] getProgram(const string& name) except +

        vector[_NodeBuilderDescriptor] getNodeBuilderDescriptors() except +

        unique_ptr[_Duration] createDuration() except +

        unique_ptr[_CommandBuffer] createCommandBuffer() except +

        void run(const _ComputeNode& node) except +
        void run(const _ContainerNode& node) except +
        void run(const _CommandBuffer& cmdBuffer) except +

        void script(const string& code) except +
        void scriptFile(const string& filename) except +

        void loadLibrary(const string& filename) except +

        _vec3ui getGoodComputeLocalShape(_ComputeDimension dimensions) const

        string help(const string& builderName) except +
        bool hasReceivedVulkanWarningMessages() except +


cdef class Session:
    cdef shared_ptr[_Session] __session
