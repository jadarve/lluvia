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

from lluvia.core.core_buffer cimport _Buffer, BufferUsageFlags
from lluvia.core.image cimport _Image, _ImageDescriptor
from lluvia.core.session cimport Session

cdef extern from 'lluvia/core/memory/MemoryPropertyFlags.h' namespace 'll':

    cdef cppclass MemoryPropertyFlags 'll::MemoryPropertyFlags':
        pass
    
    cdef enum _MemoryPropertyFlagBits 'll::MemoryPropertyFlagBits':
        _MemoryPropertyFlagBits_DeviceLocal     'll::MemoryPropertyFlagBits::DeviceLocal'
        _MemoryPropertyFlagBits_HostCached      'll::MemoryPropertyFlagBits::HostCached'
        _MemoryPropertyFlagBits_HostCoherent    'll::MemoryPropertyFlagBits::HostCoherent'
        _MemoryPropertyFlagBits_HostVisible     'll::MemoryPropertyFlagBits::HostVisible'


cdef extern from 'lluvia/core/memory/MemoryAllocationInfo.h' namespace 'll':

    cdef struct _MemoryAllocationInfo 'll::MemoryAllocationInfo':
        uint64_t offset
        uint64_t size
        uint64_t leftPadding
        uint32_t page


cdef extern from 'lluvia/core/memory/Memory.h' namespace 'll':

    cdef cppclass _Memory 'll::Memory':

        # vk.MemoryPropertyFlags getMemoryPropertyFlags() const
        MemoryPropertyFlags getMemoryPropertyFlags() const
        uint64_t getPageSize()  const
        uint32_t getPageCount() const
        bool isMappable() const
        bool isPageMappable(const uint64_t page) const

        shared_ptr[_Buffer] createBuffer(const uint64_t size, const BufferUsageFlags usageFlags) except +
        shared_ptr[_Image] createImage(const _ImageDescriptor& descriptor) except +


cpdef enum MemoryPropertyFlagBits:
    DeviceLocal     = <uint32_t> _MemoryPropertyFlagBits_DeviceLocal
    HostCached      = <uint32_t> _MemoryPropertyFlagBits_HostCached
    HostCoherent    = <uint32_t> _MemoryPropertyFlagBits_HostCoherent
    HostVisible     = <uint32_t> _MemoryPropertyFlagBits_HostVisible
    # LazilyAllocated = <uint32_t> _MemoryPropertyFlagBits_LazilyAllocated


cdef class MemoryAllocationInfo:
    cdef _MemoryAllocationInfo __allocationInfo


cdef _buildMemory(shared_ptr[_Memory] ptr, Session session)

cdef class Memory:
    cdef shared_ptr[_Memory] __memory
    cdef Session __session
