"""
    lluvia.core.image.image_filter_mode
    -----------------------------------

    :copyright: 2021, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cdef extern from 'lluvia/core/image/ImageFilterMode.h' namespace 'll':

    cdef cppclass _ImageFilterMode 'll::ImageFilterMode':
        pass

    cdef enum _ImageFilterModeBits 'll::ImageFilterMode':
        _ImageFilterModeBits_Nearest 'll::ImageFilterMode::Nearest'
        _ImageFilterModeBits_Linear  'll::ImageFilterMode::Linear'


cpdef enum ImageFilterMode:
    Nearest = <uint32_t> _ImageFilterModeBits_Nearest
    Linear  = <uint32_t> _ImageFilterModeBits_Linear
