"""
    lluvia.core.core_buffer
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t, uint64_t

from libcpp cimport bool
from libcpp.memory cimport shared_ptr, unique_ptr
from libcpp.vector cimport vector
from libcpp.string cimport string

from lluvia.core.core_object cimport _Object
from lluvia.core.memory cimport Memory, _Memory, _MemoryAllocationInfo
from lluvia.core.session cimport Session


cdef extern from 'lluvia/core/buffer/BufferUsageFlags.h' namespace 'll':

    cdef cppclass BufferUsageFlags 'll::BufferUsageFlags':
        pass
    
    cdef enum _BufferUsageFlagBits 'll::BufferUsageFlagBits':
        _BufferUsageFlagBits_StorageBuffer     'll::BufferUsageFlagBits::StorageBuffer'
        _BufferUsageFlagBits_TransferDst       'll::BufferUsageFlagBits::TransferDst'
        _BufferUsageFlagBits_TransferSrc       'll::BufferUsageFlagBits::TransferSrc'
        _BufferUsageFlagBits_UniformBuffer     'll::BufferUsageFlagBits::UniformBuffer'


cdef extern from 'lluvia/core/buffer/Buffer.h' namespace 'll':

    cdef cppclass _BufferMapDeleter 'll::Buffer::BufferMapDeleter':
        pass

    cdef cppclass _Buffer 'll::Buffer' (_Object):

        const shared_ptr[_Memory]& getMemory()   const

        uint64_t getSize() const
        _MemoryAllocationInfo getAllocationInfo() const
        BufferUsageFlags getUsageFlags() const
        bool isMappable() const

        unique_ptr[T, _BufferMapDeleter] map[T]()


cpdef enum BufferUsageFlagBits:
    StorageBuffer     = <uint32_t> _BufferUsageFlagBits_StorageBuffer
    TransferDst       = <uint32_t> _BufferUsageFlagBits_TransferDst
    TransferSrc       = <uint32_t> _BufferUsageFlagBits_TransferSrc
    UniformBuffer     = <uint32_t> _BufferUsageFlagBits_UniformBuffer


cdef _buildBuffer(shared_ptr[_Buffer] ptr, Session session, Memory memory)


cdef class Buffer:
    cdef shared_ptr[_Buffer] __buffer
    cdef Session             __session
    cdef Memory              __memory
