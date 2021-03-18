"""
    lluvia.core.compute_dimension
    -----------------------------

    :copyright: 2021, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cdef extern from 'lluvia/core/ComputeDimension.h' namespace 'll':

    cdef cppclass _ComputeDimension 'll::ComputeDimension':
        pass
