"""
    lluvia.core.memory.memory_property_flags
    ----------------------------------------

    :copyright: 2021, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

cdef extern from 'lluvia/core/memory/MemoryPropertyFlags.h' namespace 'll':

    cdef cppclass _MemoryPropertyFlags 'll::MemoryPropertyFlags':
        pass
    
    cdef enum _MemoryPropertyFlagBits 'll::MemoryPropertyFlagBits':
        _MemoryPropertyFlagBits_DeviceLocal     'll::MemoryPropertyFlagBits::DeviceLocal'
        _MemoryPropertyFlagBits_HostCached      'll::MemoryPropertyFlagBits::HostCached'
        _MemoryPropertyFlagBits_HostCoherent    'll::MemoryPropertyFlagBits::HostCoherent'
        _MemoryPropertyFlagBits_HostVisible     'll::MemoryPropertyFlagBits::HostVisible'


cpdef enum MemoryPropertyFlagBits:
    DeviceLocal     = <uint32_t> _MemoryPropertyFlagBits_DeviceLocal
    HostCached      = <uint32_t> _MemoryPropertyFlagBits_HostCached
    HostCoherent    = <uint32_t> _MemoryPropertyFlagBits_HostCoherent
    HostVisible     = <uint32_t> _MemoryPropertyFlagBits_HostVisible
