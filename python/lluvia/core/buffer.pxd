"""
    lluvia.core.buffer
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint64_t

from libcpp cimport bool
from libcpp.memory cimport shared_ptr, unique_ptr
from libcpp.vector cimport vector
from libcpp.string cimport string

from memory cimport Memory
from object cimport _Object
from session cimport Session

cimport vulkan as vk

cdef extern from 'lluvia/core/Buffer.h' namespace 'll':

    vk.BufferUsageFlags vectorStringToBufferUsageFLags(const vector[string]& flagsVector)
    vector[string] bufferUsageFlagsToVectorString(const vk.BufferUsageFlags flags)

    cdef cppclass _BufferMapDeleter 'll::Buffer::BufferMapDeleter':
        pass

    cdef cppclass _Buffer 'll::Buffer' (_Object):

        uint64_t getSize() const
        vk.BufferUsageFlags getUsageFlags() const
        bool isMappable() const

        unique_ptr[T, _BufferMapDeleter] map[T]()
        

cdef class Buffer:
    
    cdef Memory              __memory
    cdef Session             __session
    cdef shared_ptr[_Buffer] __buffer
