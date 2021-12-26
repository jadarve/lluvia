"""
    lluvia.core.memory.memory_allocation_info
    -----------------------------------------

    :copyright: 2021, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint64_t, uint32_t


cdef extern from 'lluvia/core/memory/MemoryAllocationInfo.h' namespace 'll':

    cdef struct _MemoryAllocationInfo 'll::MemoryAllocationInfo':
        uint64_t offset
        uint64_t size
        uint64_t leftPadding
        uint32_t page


cdef class MemoryAllocationInfo:
    cdef _MemoryAllocationInfo __allocationInfo
