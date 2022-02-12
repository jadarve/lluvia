"""
    lluvia.core.float_precision
    ---------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cdef extern from 'lluvia/core/FloatPrecision.h' namespace 'll':

    cdef cppclass _FloatPrecision 'll::FloatPrecision':
        pass

    cdef enum _FloatPrecisionBits 'll::FloatPrecision':
        _FloatPrecisionBits_FP16 'll::FloatPrecision::FP16'
        _FloatPrecisionBits_FP32 'll::FloatPrecision::FP32'
        _FloatPrecisionBits_FP64 'll::FloatPrecision::FP64'

cpdef enum FloatPrecision:
    FP16 = <uint32_t> _FloatPrecisionBits_FP16
    FP32 = <uint32_t> _FloatPrecisionBits_FP32
    FP64 = <uint32_t> _FloatPrecisionBits_FP64
