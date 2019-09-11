"""
    lluvia.core.memory
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from core_buffer cimport _Buffer
from image cimport _Image, _ImageDescriptor
from session cimport Session

cimport vulkan as vk

from libc.stdint cimport uint64_t, uint32_t

from libcpp cimport bool
from libcpp.memory cimport shared_ptr
from libcpp.vector cimport vector
from libcpp.string cimport string


cdef extern from 'lluvia/core/MemoryAllocationInfo.h' namespace 'll':

    cdef struct _MemoryAllocationInfo 'll::MemoryAllocationInfo':
        uint64_t offset
        uint64_t size
        uint64_t leftPadding
        uint32_t page


cdef extern from 'lluvia/core/Memory.h' namespace 'll':

    cdef cppclass _Memory 'll::Memory':

        vk.MemoryPropertyFlags getMemoryPropertyFlags() const
        uint64_t getPageSize()  const
        uint32_t getPageCount() const
        bool isMappable() const
        bool isPageMappable(const uint64_t page) const

        shared_ptr[_Buffer] createBuffer(const uint64_t size, const vk.BufferUsageFlags usageFlags) except +
        shared_ptr[_Image] createImage(const _ImageDescriptor& descriptor) except +


cdef class MemoryAllocationInfo:
    cdef _MemoryAllocationInfo __allocationInfo


cdef class Memory:

    cdef Session             __session
    cdef shared_ptr[_Memory] __memory
