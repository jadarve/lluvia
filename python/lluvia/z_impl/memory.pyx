"""
    lluvia.core.memory
    -------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport memory

cdef class Memory:

    def __cinit__(self):
        pass


    def __dealloc__(self):
        # nothing to do
        pass
