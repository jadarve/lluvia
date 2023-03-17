# cython: language_level=3, boundscheck=False, emit_code_comments=True, embedsignature=True

"""
    lluvia.core.node.parameter
    --------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t
from libcpp cimport bool as boolean

from lluvia.core.node.parameter_type import ParameterType

__all__ = [
    'Parameter'
]

cdef class Parameter:

    def __init__(self, value=0):
        self.set(value)

    def __cinit__(self):
        pass

    property type:
        def __get__(self):
            return ParameterType(<uint32_t> self.__p.getType())

    def set(self, value):

        vType = type(value)

        cdef Parameter p
        if vType == Parameter:
            p = value
            self.__p = _Parameter(p.__p)

        if vType == int or vType == bool:
            self.__p.set[int](value)

        elif vType == float:
            self.__p.set[float](value)
        
        elif vType == str:
            self.__p.set[str](value)

        else:
            raise RuntimeError('Unknown parameter type {0}'.format(vType))

    def get(self):

        pType = self.type
        if pType == ParameterType.Int:
            return self.__p.get[int]()

        if pType == ParameterType.Float:
            return self.__p.get[float]()
        
        if pType == ParameterType.String:
            return self.__p.get[str]()

        raise RuntimeError('Unknown parameter type: {0}'.format(pType))

    def __str__(self):
        return str(self.get())

    def __repr__(self):
        return 'Parameter(value={0})'.format(self.get())
