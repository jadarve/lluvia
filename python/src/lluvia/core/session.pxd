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

# from command_buffer cimport _CommandBuffer
# from compute_node cimport _ComputeNodeDescriptor, _ComputeNode
from memory cimport _Memory
# from program cimport _Program

cimport vulkan as vk

cdef extern from 'lluvia/core/Session.h' namespace 'll':

    cdef cppclass _Session 'll::Session':

        @staticmethod
        shared_ptr[_Session] create()

        vector[vk.MemoryPropertyFlags] getSupportedMemoryFlags() const

        shared_ptr[_Memory] createMemory(const vk.MemoryPropertyFlags flags, const uint64_t pageSize, bool exactFlagsMatch) const
        # shared_ptr[_Program] createProgram(const string& spirvPath) except +

        # shared_ptr[_ComputeNode] createComputeNode(const _ComputeNodeDescriptor& descriptor) except +

        # _ComputeNodeDescriptor readComputeNodeDescriptor(const string& filePath) except +
        # shared_ptr[_ComputeNode] readComputeNode(const string& filePath) except +

        # unique_ptr[_CommandBuffer] createCommandBuffer() except +

        # void run(const _ComputeNode& node) except +
        # void run(const _CommandBuffer& cmdBuffer) except +


cdef class Session:

    cdef shared_ptr[_Session] __session
