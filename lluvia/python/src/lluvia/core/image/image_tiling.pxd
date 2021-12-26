"""
    lluvia.core.image.image_tiling
    ------------------------------

    :copyright: 2021, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

cdef extern from 'lluvia/core/image/ImageTiling.h' namespace 'll':

    cdef cppclass _ImageTiling 'll::ImageTiling':
        pass

    cdef enum _ImageTilingBits 'll::ImageTiling':
        _ImageTilingBits_Optimal              'll::ImageTiling::Optimal'
        _ImageTilingBits_Linear               'll::ImageTiling::Linear'


cpdef enum ImageTiling:
    Optimal              = <uint32_t> _ImageTilingBits_Optimal
    Linear               = <uint32_t> _ImageTilingBits_Linear
