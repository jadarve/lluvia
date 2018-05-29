"""
    lluvia.core.memory
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libcpp.memory cimport shared_ptr

cdef extern from 'lluvia/core/Memory.h' namespace 'll':

    cdef cppclass _Memory 'll::Memory':

        pass


cdef class Memory:
    
    cdef shared_ptr[_Memory] _memory
