"""
    lluvia.core.impl.stdcpp
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libcpp.memory cimport shared_ptr, unique_ptr

cdef extern from "<memory>" namespace "std" nogil:

    shared_ptr[T] static_pointer_cast[T](...)
    

cdef extern from "<utility>" namespace "std":

     unique_ptr[int] move(unique_ptr[int]&& ptr)
