"""
    lluvia.core.program
    -------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint8_t

from libcpp.memory cimport shared_ptr
from libcpp.vector cimport vector


cdef extern from 'lluvia/core/Program.h' namespace 'll':

    cdef cppclass _Program 'll::Program':

        const vector[uint8_t]& getSpirV() const
        

cdef class Program:
    
    cdef shared_ptr[_Program] __program
