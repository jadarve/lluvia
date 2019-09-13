"""
    lluvia.core.enums.parameter
    ---------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cdef extern from 'lluvia/core/Parameter.h' namespace 'll':

    cdef enum _ParameterType 'll::ParameterType':
        _ParameterType_Int   'll::ParameterType::Int'
        _ParameterType_Float 'll::ParameterType::Float'


cpdef enum ParameterType:
    Int   = <uint32_t> _ParameterType_Int
    Float = <uint32_t> _ParameterType_Float
