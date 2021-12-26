"""
    lluvia.core.memory.memory_allocation_info
    -----------------------------------------

    :copyright: 2021, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""


__all__ = [
    'MemoryAllocationInfo'
]


cdef class MemoryAllocationInfo:

    def __cinit__(self):
        pass

    def __init__(self):
        pass

    def __dealloc__(self):
        pass

    property offset:
        def __get__(self):
            return self.__allocationInfo.offset

    property size:
        def __get__(self):
            return self.__allocationInfo.size

    property leftPadding:
        def __get__(self):
            return self.__allocationInfo.leftPadding

    property page:
        def __get__(self):
            return self.__allocationInfo.page

    def __str__(self):
        return 'p:{0} o:{1} l:{2} s:{3}'.format(self.page,
                                                self.offset,
                                                self.leftPadding,
                                                self.size)
