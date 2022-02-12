"""
    lluvia.core.node.parameter
    --------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from lluvia.core.node.parameter_type cimport _ParameterType

cdef extern from 'lluvia/core/node/Parameter.h' namespace 'll':

    cdef cppclass _Parameter 'll::Parameter':

        _Parameter()
        _Parameter(const _Parameter&)

        _ParameterType getType() const

        void set[T](T&)

        T get[T]() const



cdef class Parameter:

    cdef _Parameter __p
