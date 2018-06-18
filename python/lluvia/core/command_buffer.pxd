"""
    lluvia.core.command_buffer
    --------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint64_t, uint32_t

from libcpp cimport bool
from libcpp.memory cimport shared_ptr, unique_ptr
from libcpp.vector cimport vector
from libcpp.string cimport string

from buffer cimport _Buffer
from image cimport _Image
from compute_node cimport _ComputeNode

cimport vulkan as vk


cdef extern from 'lluvia/core/CommandBuffer.h' namespace 'll':

    cdef cppclass _CommandBuffer 'll::CommandBuffer':

        void begin()
        void end()

        void run(const _ComputeNode& node)

        void copyBuffer(const _Buffer& src, const _Buffer& dst) except +
        void copyBufferToImage(const _Buffer& src, const _Image& dst)
        void copyImageToBuffer(const _Image& src, const _Buffer& dst)

        void changeImageLayout(_Image& image, const vk.ImageLayout newLayout)

        void memoryBarrier()


cdef extern from "<utility>" namespace "std":

     unique_ptr[_CommandBuffer] move(unique_ptr[_CommandBuffer]&& ptr)


cdef class CommandBuffer:
    
    cdef shared_ptr[_CommandBuffer] __commandBuffer
