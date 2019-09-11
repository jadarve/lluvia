"""
    lluvia.core.program
    -------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport program

from libc.stdint cimport uint8_t
from libcpp.vector cimport vector


__all__ = ['Program']


cdef class Program:

    def __cinit__(self):
        pass

    def __dealloc__(self):
        # nothing to do
        pass

    property spirV:
        def __get__(self):
            """
            Spir-V code.
            """
            cdef vector[uint8_t] spirV = self.__program.get().getSpirV()
            return spirV
