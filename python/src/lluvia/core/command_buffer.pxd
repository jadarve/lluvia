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

from core_buffer cimport _Buffer
from image cimport _Image
from node cimport _ComputeNode
from session cimport Session

cimport vulkan as vk


cdef extern from 'lluvia/core/CommandBuffer.h' namespace 'll':

    cdef cppclass _CommandBuffer 'll::CommandBuffer':

        void begin() except +
        void end() except +

        void run(const _ComputeNode& node) except +

        void copyBuffer(const _Buffer& src, const _Buffer& dst) except +
        void copyBufferToImage(const _Buffer& src, const _Image& dst) except +
        void copyImageToBuffer(const _Image& src, const _Buffer& dst) except +
        void copyImageToImage(const _Image& src, const _Image& dst) except +

        void changeImageLayout(_Image& image, const vk.ImageLayout newLayout) except +

        void clearImage(_Image& image) except +

        void memoryBarrier() except +


cdef extern from "<utility>" namespace "std":

    unique_ptr[_CommandBuffer] move(unique_ptr[_CommandBuffer]&& ptr)


cdef class CommandBuffer:

    cdef shared_ptr[_CommandBuffer] __commandBuffer
    cdef Session __session
