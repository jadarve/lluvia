"""
    lluvia.core.enums.compute_dimension
    -----------------------------------

    :copyright: 2021, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cdef extern from 'lluvia/core/ComputeDimension.h' namespace 'll':

    cdef enum _ComputeDimension 'll::ComputeDimension':
        _ComputeDimension_D1 'll::ComputeDimension::D1'
        _ComputeDimension_D2 'll::ComputeDimension::D2'
        _ComputeDimension_D3 'll::ComputeDimension::D3'


cpdef enum ComputeDimension:
    D1 = <uint32_t> _ComputeDimension_D1
    D2 = <uint32_t> _ComputeDimension_D2
    D3 = <uint32_t> _ComputeDimension_D3
