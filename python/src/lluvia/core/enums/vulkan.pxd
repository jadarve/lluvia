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


    cdef enum _ImageLayout 'vk::ImageLayout':
        _ImageLayout_Undefined                                'vk::ImageLayout::eUndefined'
        _ImageLayout_General                                  'vk::ImageLayout::eGeneral'
        _ImageLayout_ColorAttachmentOptimal                   'vk::ImageLayout::eColorAttachmentOptimal'
        _ImageLayout_DepthStencilAttachmentOptimal            'vk::ImageLayout::eDepthStencilAttachmentOptimal'
        _ImageLayout_DepthStencilReadOnlyOptimal              'vk::ImageLayout::eDepthStencilReadOnlyOptimal'
        _ImageLayout_ShaderReadOnlyOptimal                    'vk::ImageLayout::eShaderReadOnlyOptimal'
        _ImageLayout_TransferSrcOptimal                       'vk::ImageLayout::eTransferSrcOptimal'
        _ImageLayout_TransferDstOptimal                       'vk::ImageLayout::eTransferDstOptimal'
        _ImageLayout_Preinitialized                           'vk::ImageLayout::ePreinitialized'
        _ImageLayout_PresentSrcKHR                            'vk::ImageLayout::ePresentSrcKHR'
        _ImageLayout_SharedPresentKHR                         'vk::ImageLayout::eSharedPresentKHR'
        _ImageLayout_DepthReadOnlyStencilAttachmentOptimalKHR 'vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimalKHR'
        _ImageLayout_DepthAttachmentStencilReadOnlyOptimalKHR 'vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimalKHR'


    cdef enum _ImageTiling 'vk::ImageTiling':
        _ImageTiling_Optimal 'vk::ImageTiling::eOptimal'
        _ImageTiling_Linear  'vk::ImageTiling::eLinear'


    cdef enum _ImageType 'vk::ImageType':
        _ImageType_1D 'vk::ImageType::e1D'
        _ImageType_2D 'vk::ImageType::e2D'
        _ImageType_3D 'vk::ImageType::e3D'


    # The image formats below are extracted from ll::getVulkanImageFormat() at ImageDescriptor.cpp
    cdef enum _Format 'vk::Format':
        _Format_R8Uint             'vk::Format::eR8Uint'
        _Format_R8Sint             'vk::Format::eR8Sint'
        _Format_R16Uint            'vk::Format::eR16Uint'
        _Format_R16Sint            'vk::Format::eR16Sint'
        _Format_R16Sfloat          'vk::Format::eR16Sfloat'
        _Format_R32Uint            'vk::Format::eR32Uint'
        _Format_R32Sint            'vk::Format::eR32Sint'
        _Format_R32Sfloat          'vk::Format::eR32Sfloat'
        _Format_R64Uint            'vk::Format::eR64Uint'
        _Format_R64Sint            'vk::Format::eR64Sint'
        _Format_R64Sfloat          'vk::Format::eR64Sfloat'

        _Format_R8G8Uint           'vk::Format::eR8G8Uint'
        _Format_R8G8Sint           'vk::Format::eR8G8Sint'
        _Format_R16G16Uint         'vk::Format::eR16G16Uint'
        _Format_R16G16Sint         'vk::Format::eR16G16Sint'
        _Format_R16G16Sfloat       'vk::Format::eR16G16Sfloat'
        _Format_R32G32Uint         'vk::Format::eR32G32Uint'
        _Format_R32G32Sint         'vk::Format::eR32G32Sint'
        _Format_R32G32Sfloat       'vk::Format::eR32G32Sfloat'
        _Format_R64G64Uint         'vk::Format::eR64G64Uint'
        _Format_R64G64Sint         'vk::Format::eR64G64Sint'
        _Format_R64G64Sfloat       'vk::Format::eR64G64Sfloat'

        _Format_R8G8B8Uint         'vk::Format::eR8G8B8Uint'
        _Format_R8G8B8Sint         'vk::Format::eR8G8B8Sint'
        _Format_R16G16B16Uint      'vk::Format::eR16G16B16Uint'
        _Format_R16G16B16Sint      'vk::Format::eR16G16B16Sint'
        _Format_R16G16B16Sfloat    'vk::Format::eR16G16B16Sfloat'
        _Format_R32G32B32Uint      'vk::Format::eR32G32B32Uint'
        _Format_R32G32B32Sint      'vk::Format::eR32G32B32Sint'
        _Format_R32G32B32Sfloat    'vk::Format::eR32G32B32Sfloat'
        _Format_R64G64B64Uint      'vk::Format::eR64G64B64Uint'
        _Format_R64G64B64Sint      'vk::Format::eR64G64B64Sint'
        _Format_R64G64B64Sfloat    'vk::Format::eR64G64B64Sfloat'

        _Format_R8G8B8A8Uint       'vk::Format::eR8G8B8A8Uint'
        _Format_R8G8B8A8Sint       'vk::Format::eR8G8B8A8Sint'
        _Format_R16G16B16A16Uint   'vk::Format::eR16G16B16A16Uint'
        _Format_R16G16B16A16Sint   'vk::Format::eR16G16B16A16Sint'
        _Format_R16G16B16A16Sfloat 'vk::Format::eR16G16B16A16Sfloat'
        _Format_R32G32B32A32Uint   'vk::Format::eR32G32B32A32Uint'
        _Format_R32G32B32A32Sint   'vk::Format::eR32G32B32A32Sint'
        _Format_R32G32B32A32Sfloat 'vk::Format::eR32G32B32A32Sfloat'
        _Format_R64G64B64A64Uint   'vk::Format::eR64G64B64A64Uint'
        _Format_R64G64B64A64Sint   'vk::Format::eR64G64B64A64Sint'
        _Format_R64G64B64A64Sfloat 'vk::Format::eR64G64B64A64Sfloat'


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


cpdef enum ImageLayout:
    Undefined                                = <uint32_t> _ImageLayout_Undefined
    General                                  = <uint32_t> _ImageLayout_General
    ColorAttachmentOptimal                   = <uint32_t> _ImageLayout_ColorAttachmentOptimal
    DepthStencilAttachmentOptimal            = <uint32_t> _ImageLayout_DepthStencilAttachmentOptimal
    DepthStencilReadOnlyOptimal              = <uint32_t> _ImageLayout_DepthStencilReadOnlyOptimal
    ShaderReadOnlyOptimal                    = <uint32_t> _ImageLayout_ShaderReadOnlyOptimal
    TransferSrcOptimal                       = <uint32_t> _ImageLayout_TransferSrcOptimal
    TransferDstOptimal                       = <uint32_t> _ImageLayout_TransferDstOptimal
    Preinitialized                           = <uint32_t> _ImageLayout_Preinitialized
    PresentSrcKHR                            = <uint32_t> _ImageLayout_PresentSrcKHR
    SharedPresentKHR                         = <uint32_t> _ImageLayout_SharedPresentKHR
    DepthReadOnlyStencilAttachmentOptimalKHR = <uint32_t> _ImageLayout_DepthReadOnlyStencilAttachmentOptimalKHR
    DepthAttachmentStencilReadOnlyOptimalKHR = <uint32_t> _ImageLayout_DepthAttachmentStencilReadOnlyOptimalKHR


cpdef enum ImageTiling:
    Optimal = <uint32_t> _ImageTiling_Optimal
    Linear  = <uint32_t> _ImageTiling_Linear


cpdef enum Format:
    R8Uint             = <uint32_t> _Format_R8Uint
    R8Sint             = <uint32_t> _Format_R8Sint
    R16Uint            = <uint32_t> _Format_R16Uint
    R16Sint            = <uint32_t> _Format_R16Sint
    R16Sfloat          = <uint32_t> _Format_R16Sfloat
    R32Uint            = <uint32_t> _Format_R32Uint
    R32Sint            = <uint32_t> _Format_R32Sint
    R32Sfloat          = <uint32_t> _Format_R32Sfloat
    R64Uint            = <uint32_t> _Format_R64Uint
    R64Sint            = <uint32_t> _Format_R64Sint
    R64Sfloat          = <uint32_t> _Format_R64Sfloat

    R8G8Uint           = <uint32_t> _Format_R8G8Uint
    R8G8Sint           = <uint32_t> _Format_R8G8Sint
    R16G16Uint         = <uint32_t> _Format_R16G16Uint
    R16G16Sint         = <uint32_t> _Format_R16G16Sint
    R16G16Sfloat       = <uint32_t> _Format_R16G16Sfloat
    R32G32Uint         = <uint32_t> _Format_R32G32Uint
    R32G32Sint         = <uint32_t> _Format_R32G32Sint
    R32G32Sfloat       = <uint32_t> _Format_R32G32Sfloat
    R64G64Uint         = <uint32_t> _Format_R64G64Uint
    R64G64Sint         = <uint32_t> _Format_R64G64Sint
    R64G64Sfloat       = <uint32_t> _Format_R64G64Sfloat

    R8G8B8Uint         = <uint32_t> _Format_R8G8B8Uint
    R8G8B8Sint         = <uint32_t> _Format_R8G8B8Sint
    R16G16B16Uint      = <uint32_t> _Format_R16G16B16Uint
    R16G16B16Sint      = <uint32_t> _Format_R16G16B16Sint
    R16G16B16Sfloat    = <uint32_t> _Format_R16G16B16Sfloat
    R32G32B32Uint      = <uint32_t> _Format_R32G32B32Uint
    R32G32B32Sint      = <uint32_t> _Format_R32G32B32Sint
    R32G32B32Sfloat    = <uint32_t> _Format_R32G32B32Sfloat
    R64G64B64Uint      = <uint32_t> _Format_R64G64B64Uint
    R64G64B64Sint      = <uint32_t> _Format_R64G64B64Sint
    R64G64B64Sfloat    = <uint32_t> _Format_R64G64B64Sfloat

    R8G8B8A8Uint       = <uint32_t> _Format_R8G8B8A8Uint
    R8G8B8A8Sint       = <uint32_t> _Format_R8G8B8A8Sint
    R16G16B16A16Uint   = <uint32_t> _Format_R16G16B16A16Uint
    R16G16B16A16Sint   = <uint32_t> _Format_R16G16B16A16Sint
    R16G16B16A16Sfloat = <uint32_t> _Format_R16G16B16A16Sfloat
    R32G32B32A32Uint   = <uint32_t> _Format_R32G32B32A32Uint
    R32G32B32A32Sint   = <uint32_t> _Format_R32G32B32A32Sint
    R32G32B32A32Sfloat = <uint32_t> _Format_R32G32B32A32Sfloat
    R64G64B64A64Uint   = <uint32_t> _Format_R64G64B64A64Uint
    R64G64B64A64Sint   = <uint32_t> _Format_R64G64B64A64Sint
    R64G64B64A64Sfloat = <uint32_t> _Format_R64G64B64A64Sfloat


cpdef enum ImageType:
    e1D = <uint32_t> _ImageType_1D
    e2D = <uint32_t> _ImageType_2D
    e3D = <uint32_t> _ImageType_3D
