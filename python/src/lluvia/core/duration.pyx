"""
    lluvia.core.duration
    --------------------

    :copyright: 2020, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport duration


cdef class Duration:

    def __cinit__(self):
        pass

    def __dealloc__(self):
        pass

    property nanoseconds:
        def __get__(self):
            return self.__duration.get().getNanoseconds()
