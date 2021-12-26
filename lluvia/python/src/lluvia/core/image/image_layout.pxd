"""
    lluvia.core.image.image_layout
    ------------------------------

    :copyright: 2021, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

cdef extern from 'lluvia/core/image/ImageLayout.h' namespace 'll':

    cdef cppclass _ImageLayout 'll::ImageLayout':
        pass
    
    cdef enum _ImageLayoutBits 'll::ImageLayout':
        _ImageLayoutBits_Undefined             'll::ImageLayout::Undefined'
        _ImageLayoutBits_General               'll::ImageLayout::General'
        _ImageLayoutBits_ShaderReadOnlyOptimal 'll::ImageLayout::ShaderReadOnlyOptimal'
        _ImageLayoutBits_TransferSrcOptimal    'll::ImageLayout::TransferSrcOptimal'
        _ImageLayoutBits_TransferDstOptimal    'll::ImageLayout::TransferDstOptimal'
        _ImageLayoutBits_Preinitialized        'll::ImageLayout::Preinitialized'


cpdef enum ImageLayout:
    Undefined             = <uint32_t> _ImageLayoutBits_Undefined
    General               = <uint32_t> _ImageLayoutBits_General
    ShaderReadOnlyOptimal = <uint32_t> _ImageLayoutBits_ShaderReadOnlyOptimal
    TransferSrcOptimal    = <uint32_t> _ImageLayoutBits_TransferSrcOptimal
    TransferDstOptimal    = <uint32_t> _ImageLayoutBits_TransferDstOptimal
    Preinitialized        = <uint32_t> _ImageLayoutBits_Preinitialized
