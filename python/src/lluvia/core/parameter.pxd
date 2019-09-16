"""
    lluvia.core.parameter
    ---------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""


cdef extern from 'lluvia/core/Parameter.h' namespace 'll':

    cdef cppclass _ParameterType 'll::ParameterType':
        pass

    cdef cppclass _Parameter 'll::Parameter':

        _Parameter()
        _Parameter(const _Parameter&)

        _ParameterType getType() const

        void set[T](T&)

        T get[T]() const



cdef class Parameter:

    cdef _Parameter __p
