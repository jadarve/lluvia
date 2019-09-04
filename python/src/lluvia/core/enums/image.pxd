"""
    lluvia.core.enums.image
    -----------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cdef extern from 'lluvia/core/ImageDescriptor.h' namespace 'll':
    
    cdef enum _ChannelType 'll::ChannelType':
        _ChannelType_Uint8   'll::ChannelType::Uint8'
        _ChannelType_Int8    'll::ChannelType::Int8'

        _ChannelType_Uint16  'll::ChannelType::Uint16'
        _ChannelType_Int16   'll::ChannelType::Int16'
        _ChannelType_Float16 'll::ChannelType::Float16'

        _ChannelType_Uint32  'll::ChannelType::Uint32'
        _ChannelType_Int32   'll::ChannelType::Int32'
        _ChannelType_Float32 'll::ChannelType::Float32'

        _ChannelType_Uint64  'll::ChannelType::Uint64'
        _ChannelType_Int64   'll::ChannelType::Int64'
        _ChannelType_Float64 'll::ChannelType::Float64'


    cdef enum _ChannelCount 'll::ChannelCount':
        _ChannelCount_C1 'll::ChannelCount::C1'
        _ChannelCount_C2 'll::ChannelCount::C2'
        _ChannelCount_C3 'll::ChannelCount::C3'
        _ChannelCount_C4 'll::ChannelCount::C4'


cdef extern from 'lluvia/core/ImageViewDescriptor.h' namespace 'll':
    
    cdef enum _ImageAxis 'll::ImageAxis':
        _ImageAxis_U 'll::ImageAxis::U'
        _ImageAxis_V 'll::ImageAxis::V'
        _ImageAxis_W 'll::ImageAxis::W'


    cdef enum _ImageFilterMode 'll::ImageFilterMode':
        _ImageFilterMode_Nearest 'll::ImageFilterMode::Nearest'
        _ImageFilterMode_Linear  'll::ImageFilterMode::Linear'


    cdef enum _ImageAddressMode 'll::ImageAddressMode':
        _ImageAddressMode_Repeat            'll::ImageAddressMode::Repeat'
        _ImageAddressMode_MirroredRepeat    'll::ImageAddressMode::MirroredRepeat'
        _ImageAddressMode_ClampToEdge       'll::ImageAddressMode::ClampToEdge'
        _ImageAddressMode_ClampToBorder     'll::ImageAddressMode::ClampToBorder'
        _ImageAddressMode_MirrorClampToEdge 'll::ImageAddressMode::MirrorClampToEdge'


cdef extern from 'vulkan/vulkan.hpp' namespace 'vk':
    
    cdef enum _ImageUsageFlagBits 'vk::ImageUsageFlagBits':
        _ImageUsageFlagBits_TransferSrc            'vk::ImageUsageFlagBits::eTransferSrc'
        _ImageUsageFlagBits_TransferDst            'vk::ImageUsageFlagBits::eTransferDst'
        _ImageUsageFlagBits_Sampled                'vk::ImageUsageFlagBits::eSampled'
        _ImageUsageFlagBits_Storage                'vk::ImageUsageFlagBits::eStorage'
        _ImageUsageFlagBits_ColorAttachment        'vk::ImageUsageFlagBits::eColorAttachment'
        _ImageUsageFlagBits_DepthStencilAttachment 'vk::ImageUsageFlagBits::eDepthStencilAttachment'
        _ImageUsageFlagBits_TransientAttachment    'vk::ImageUsageFlagBits::eTransientAttachment'
        _ImageUsageFlagBits_InputAttachment        'vk::ImageUsageFlagBits::eInputAttachment'


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


cpdef enum ChannelType:
    Uint8   = <uint32_t> _ChannelType_Uint8
    Int8    = <uint32_t> _ChannelType_Int8

    Uint16  = <uint32_t> _ChannelType_Uint16
    Int16   = <uint32_t> _ChannelType_Int16
    Float16 = <uint32_t> _ChannelType_Float16

    Uint32  = <uint32_t> _ChannelType_Uint32
    Int32   = <uint32_t> _ChannelType_Int32
    Float32 = <uint32_t> _ChannelType_Float32

    Uint64  = <uint32_t> _ChannelType_Uint64
    Int64   = <uint32_t> _ChannelType_Int64
    Float64 = <uint32_t> _ChannelType_Float64


cpdef enum ChannelCount:
    C1 = <uint32_t> _ChannelCount_C1
    C2 = <uint32_t> _ChannelCount_C2
    C3 = <uint32_t> _ChannelCount_C3
    C4 = <uint32_t> _ChannelCount_C4


cpdef enum ImageAxis:
    U = <uint32_t> _ImageAxis_U
    V = <uint32_t> _ImageAxis_V
    W = <uint32_t> _ImageAxis_W


cpdef enum ImageFilterMode:
    Nearest = <uint32_t> _ImageFilterMode_Nearest
    Linear  = <uint32_t> _ImageFilterMode_Linear


cpdef enum ImageAddressMode:
    Repeat            = <uint32_t> _ImageAddressMode_Repeat
    MirroredRepeat    = <uint32_t> _ImageAddressMode_MirroredRepeat
    ClampToEdge       = <uint32_t> _ImageAddressMode_ClampToEdge
    ClampToBorder     = <uint32_t> _ImageAddressMode_ClampToBorder
    MirrorClampToEdge = <uint32_t> _ImageAddressMode_MirrorClampToEdge


cpdef enum ImageUsageFlagBits:
    TransferSrc            = <uint32_t> _ImageUsageFlagBits_TransferSrc
    TransferDst            = <uint32_t> _ImageUsageFlagBits_TransferDst
    Sampled                = <uint32_t> _ImageUsageFlagBits_Sampled
    Storage                = <uint32_t> _ImageUsageFlagBits_Storage
    ColorAttachment        = <uint32_t> _ImageUsageFlagBits_ColorAttachment
    DepthStencilAttachment = <uint32_t> _ImageUsageFlagBits_DepthStencilAttachment
    TransientAttachment    = <uint32_t> _ImageUsageFlagBits_TransientAttachment
    InputAttachment        = <uint32_t> _ImageUsageFlagBits_InputAttachment


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
