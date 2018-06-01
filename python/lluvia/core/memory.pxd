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

from buffer cimport _Buffer

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

        shared_ptr[_Buffer] createBuffer(const uint64_t size, const vk.BufferUsageFlags usageFlags)


cdef class Memory:
    
    cdef shared_ptr[_Memory] __memory
