"""
    lluvia.core.image.image_axis
    -----------------------------------

    :copyright: 2021, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cdef extern from 'lluvia/core/image/ImageAxis.h' namespace 'll':

    cdef cppclass _ImageAxis 'll::ImageAxis':
        pass

    cdef enum _ImageAxisBits 'll::ImageAxis':
        _ImageAxisBits_U 'll::ImageAxis::U'
        _ImageAxisBits_V 'll::ImageAxis::V'
        _ImageAxisBits_W 'll::ImageAxis::W'

cpdef enum ImageAxis:
    U = <uint32_t> _ImageAxisBits_U
    V = <uint32_t> _ImageAxisBits_V
    W = <uint32_t> _ImageAxisBits_W
