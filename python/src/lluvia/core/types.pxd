"""
    lluvia.core.types
    -----------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cdef extern from 'lluvia/core/types.h' namespace 'll':

    # cdef cppclass _vec3[T] 'll::vec3':
    #     T x
    #     T y
    #     T z

    cdef struct _vec3ui 'll::vec3ui':
        uint32_t x
        uint32_t y
        uint32_t z
