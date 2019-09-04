"""
    lluvia.core.enums.vulkan
    ------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

cdef extern from 'vulkan/vulkan.hpp' namespace 'vk':

    cdef enum _BufferUsageFlagBits:
        _BufferUsageFlagBits_IndexBuffer        'vk::BufferUsageFlagBits::eIndexBuffer'
        _BufferUsageFlagBits_IndirectBuffer     'vk::BufferUsageFlagBits::eIndirectBuffer'
        _BufferUsageFlagBits_StorageBuffer      'vk::BufferUsageFlagBits::eStorageBuffer'
        _BufferUsageFlagBits_StorageTexelBuffer 'vk::BufferUsageFlagBits::eStorageTexelBuffer'
        _BufferUsageFlagBits_TransferDst        'vk::BufferUsageFlagBits::eTransferDst'
        _BufferUsageFlagBits_TransferSrc        'vk::BufferUsageFlagBits::eTransferSrc'
        _BufferUsageFlagBits_UniformBuffer      'vk::BufferUsageFlagBits::eUniformBuffer'
        _BufferUsageFlagBits_UniformTexelBuffer 'vk::BufferUsageFlagBits::eUniformTexelBuffer'
        _BufferUsageFlagBits_VertexBuffer       'vk::BufferUsageFlagBits::eVertexBuffer'


    cdef enum _MemoryPropertyFlagBits 'vk::MemoryPropertyFlagBits':
        _MemoryPropertyFlagBits_DeviceLocal     'vk::MemoryPropertyFlagBits::eDeviceLocal'
        _MemoryPropertyFlagBits_HostCached      'vk::MemoryPropertyFlagBits::eHostCached'
        _MemoryPropertyFlagBits_HostCoherent    'vk::MemoryPropertyFlagBits::eHostCoherent'
        _MemoryPropertyFlagBits_HostVisible     'vk::MemoryPropertyFlagBits::eHostVisible'
        _MemoryPropertyFlagBits_LazilyAllocated 'vk::MemoryPropertyFlagBits::eLazilyAllocated'


cpdef enum BufferUsageFlagBits:
    IndexBuffer        = <uint32_t> _BufferUsageFlagBits_IndexBuffer
    IndirectBuffer     = <uint32_t> _BufferUsageFlagBits_IndirectBuffer
    StorageBuffer      = <uint32_t> _BufferUsageFlagBits_StorageBuffer
    StorageTexelBuffer = <uint32_t> _BufferUsageFlagBits_StorageTexelBuffer
    TransferDst        = <uint32_t> _BufferUsageFlagBits_TransferDst
    TransferSrc        = <uint32_t> _BufferUsageFlagBits_TransferSrc
    UniformBuffer      = <uint32_t> _BufferUsageFlagBits_UniformBuffer
    UniformTexelBuffer = <uint32_t> _BufferUsageFlagBits_UniformTexelBuffer
    VertexBuffer       = <uint32_t> _BufferUsageFlagBits_VertexBuffer


cpdef enum MemoryPropertyFlagBits:
    DeviceLocal     = <uint32_t> _MemoryPropertyFlagBits_DeviceLocal
    HostCached      = <uint32_t> _MemoryPropertyFlagBits_HostCached
    HostCoherent    = <uint32_t> _MemoryPropertyFlagBits_HostCoherent
    HostVisible     = <uint32_t> _MemoryPropertyFlagBits_HostVisible
    LazilyAllocated = <uint32_t> _MemoryPropertyFlagBits_LazilyAllocated
