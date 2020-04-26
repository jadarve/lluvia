"""
    lluvia.core.core_buffer
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint64_t

from libcpp cimport bool
from libcpp.memory cimport shared_ptr, unique_ptr
from libcpp.vector cimport vector
from libcpp.string cimport string

from .memory cimport Memory, _Memory, _MemoryAllocationInfo
from .core_object cimport _Object
from .session cimport Session
from .vulkan cimport BufferUsageFlags


cdef extern from 'lluvia/core/Buffer.h' namespace 'll':

    cdef cppclass _BufferMapDeleter 'll::Buffer::BufferMapDeleter':
        pass

    cdef cppclass _Buffer 'll::Buffer' (_Object):

        const shared_ptr[_Memory]& getMemory()   const

        uint64_t getSize() const
        _MemoryAllocationInfo getAllocationInfo() const
        BufferUsageFlags getUsageFlags() const
        bool isMappable() const

        unique_ptr[T, _BufferMapDeleter] map[T]()


cdef _buildBuffer(shared_ptr[_Buffer] ptr, Session session, Memory memory)

cdef class Buffer:
    cdef shared_ptr[_Buffer] __buffer
    cdef Session             __session
    cdef Memory              __memory