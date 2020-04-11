"""
    lluvia.core.duration
    --------------------

    :copyright: 2020, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport int64_t
from libcpp.memory cimport shared_ptr, unique_ptr


cdef extern from 'lluvia/core/Duration.h' namespace 'll':

    cdef cppclass _Duration 'll::Duration':

        int64_t getNanoseconds() except +


cdef extern from "<utility>" namespace "std":

    unique_ptr[_Duration] moveDuration 'std::move' (unique_ptr[_Duration]&& ptr)


cdef class Duration:
    cdef shared_ptr[_Duration] __duration
