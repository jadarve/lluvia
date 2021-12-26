"""
    lluvia.core.buffer_usage_flags
    ------------------------------

    :copyright: 2021, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cdef extern from 'lluvia/core/buffer/BufferUsageFlags.h' namespace 'll':

    cdef cppclass _BufferUsageFlags 'll::BufferUsageFlags':
        pass
    
    cdef enum _BufferUsageFlagBits 'll::BufferUsageFlagBits':
        _BufferUsageFlagBits_StorageBuffer     'll::BufferUsageFlagBits::StorageBuffer'
        _BufferUsageFlagBits_TransferDst       'll::BufferUsageFlagBits::TransferDst'
        _BufferUsageFlagBits_TransferSrc       'll::BufferUsageFlagBits::TransferSrc'
        _BufferUsageFlagBits_UniformBuffer     'll::BufferUsageFlagBits::UniformBuffer'


cpdef enum BufferUsageFlagBits:
    StorageBuffer     = <uint32_t> _BufferUsageFlagBits_StorageBuffer
    TransferDst       = <uint32_t> _BufferUsageFlagBits_TransferDst
    TransferSrc       = <uint32_t> _BufferUsageFlagBits_TransferSrc
    UniformBuffer     = <uint32_t> _BufferUsageFlagBits_UniformBuffer
