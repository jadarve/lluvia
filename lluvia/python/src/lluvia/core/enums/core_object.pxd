"""
    lluvia.core.enums.core_object
    -----------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

cdef extern from 'lluvia/core/Object.h' namespace 'll':

    cdef enum _ObjectType 'll::ObjectType':

        _ObjectType_Buffer    'll::ObjectType::Buffer'
        _ObjectType_Image     'll::ObjectType::Image'
        _ObjectType_ImageView 'll::ObjectType::ImageView'


cpdef enum ObjectType:
    Buffer    = <uint32_t> _ObjectType_Buffer
    Image     = <uint32_t> _ObjectType_Image
    ImageView = <uint32_t> _ObjectType_ImageView
