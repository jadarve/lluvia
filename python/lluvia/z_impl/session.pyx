"""
    lluvia.core.session
    -------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport session


cdef class Session:

    def __cinit__(self):
        self.__session = _Session.create()
        print('session built!')


    def __dealloc__(self):
        # nothing to do
        pass


    def createMemory(flags, pageSize, exactFlagsMatch = False):
        print('createMemory')
