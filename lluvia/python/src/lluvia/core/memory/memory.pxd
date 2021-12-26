"""
    lluvia.core.memory
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint64_t, uint32_t

from libcpp cimport bool
from libcpp.memory cimport shared_ptr

from lluvia.core.memory.memory_property_flags cimport _MemoryPropertyFlags

from lluvia.core.buffer.buffer cimport _Buffer
from lluvia.core.buffer.buffer_usage_flags cimport _BufferUsageFlags

from lluvia.core.image.image cimport _Image, _ImageDescriptor

from lluvia.core.session cimport Session

cdef extern from 'lluvia/core/memory/Memory.h' namespace 'll':

    cdef cppclass _Memory 'll::Memory':

        _MemoryPropertyFlags getMemoryPropertyFlags() const
        uint64_t getPageSize()  const
        uint32_t getPageCount() const
        bool isMappable() const
        bool isPageMappable(const uint64_t page) const

        shared_ptr[_Buffer] createBuffer(const uint64_t size, const _BufferUsageFlags usageFlags) except +
        shared_ptr[_Image] createImage(const _ImageDescriptor& descriptor) except +


cdef _buildMemory(shared_ptr[_Memory] ptr, Session session)

cdef class Memory:
    cdef shared_ptr[_Memory] __memory
    cdef Session __session
