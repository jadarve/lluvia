"""
    lluvia.core.memory
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint64_t, uint32_t

from libcpp cimport bool
from libcpp.memory cimport shared_ptr
from libcpp.vector cimport vector
from libcpp.string cimport string

from core_buffer cimport _Buffer
from image cimport _Image, _ImageDescriptor
from session cimport Session

cimport vulkan as vk

cdef extern from 'lluvia/core/Memory.h' namespace 'll':

    vk.MemoryPropertyFlags vectorStringToMemoryPropertyFlags (const vector[string]& flagsVector)
    vector[string] memoryPropertyFlagsToVectorString (const vk.MemoryPropertyFlags flags)


    cdef cppclass _Memory 'll::Memory':

        vk.MemoryPropertyFlags getMemoryPropertyFlags() const
        uint64_t getPageSize()  const
        uint32_t getPageCount() const
        bool isMappable() const
        bool isPageMappable(const uint64_t page) const

        shared_ptr[_Buffer] createBuffer(const uint64_t size, const vk.BufferUsageFlags usageFlags) except +
        shared_ptr[_Image] createImage(const _ImageDescriptor& descriptor, const vk.ImageUsageFlags usageFlags) except +


cdef class Memory:
    
    cdef Session             __session
    cdef shared_ptr[_Memory] __memory
