"""
    lluvia.core.parameter
    ---------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

# from libcpp cimport bool

cdef class Parameter:

    def __init__(self, value=0):
        pass

    def __cinit__(self):
        pass

    def __assign__(self, other):

        return self

    def __rassign__(self, other):
        return 0


    def __set(self, value):

        cdef Parameter p
        if type(value) == Parameter:
            p = value
            self.__p = _Parameter(p.__p)

        if type(value) == int:
            self.__p.set[int](value)

        elif type(value) == float:
            self.__p.set[float](value)

        elif type(value) == bool:
            self.__p.set[bool](value)
