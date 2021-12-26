"""
    lluvia.core.image.image_address_mode
    ------------------------------------

    :copyright: 2021, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cdef extern from 'lluvia/core/image/ImageAddressMode.h' namespace 'll':

    cdef cppclass _ImageAddressMode 'll::ImageAddressMode':
        pass

    cdef enum _ImageAddressModeBits 'll::ImageAddressMode':
        _ImageAddressModeBits_Repeat            'll::ImageAddressMode::Repeat'
        _ImageAddressModeBits_MirroredRepeat    'll::ImageAddressMode::MirroredRepeat'
        _ImageAddressModeBits_ClampToEdge       'll::ImageAddressMode::ClampToEdge'
        _ImageAddressModeBits_ClampToBorder     'll::ImageAddressMode::ClampToBorder'
        _ImageAddressModeBits_MirrorClampToEdge 'll::ImageAddressMode::MirrorClampToEdge'


cpdef enum ImageAddressMode:
    Repeat            = <uint32_t> _ImageAddressModeBits_Repeat
    MirroredRepeat    = <uint32_t> _ImageAddressModeBits_MirroredRepeat
    ClampToEdge       = <uint32_t> _ImageAddressModeBits_ClampToEdge
    ClampToBorder     = <uint32_t> _ImageAddressModeBits_ClampToBorder
    MirrorClampToEdge = <uint32_t> _ImageAddressModeBits_MirrorClampToEdge
