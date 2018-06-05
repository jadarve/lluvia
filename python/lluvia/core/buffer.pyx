"""
    lluvia.core.buffer
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport buffer

cimport cython

from libcpp.memory cimport unique_ptr


__all__ = ['Buffer', 'BufferUsageFlags']


BufferUsageFlags = ['IndexBuffer',
                    'IndirectBuffer',
                    'StorageBuffer',
                    'StorageTexelBuffer',
                    'TransferDst',
                    'TransferSrc',
                    'UniformBuffer',
                    'UniformTexelBuffer',
                    'VertexBuffer']


@cython.infer_types(True)
cdef class Buffer:
    
    def __cinit__(self):
        pass


    def __dealloc__(self):
        pass


    property usageFlags:
        def __get__(self):
            """
            Usage flags for this buffer.
            """

            cdef vk.BufferUsageFlags vkFlags = self.__buffer.get().getUsageFlags()
            cdef vector[string] stringFlagsList = bufferUsageFlagsToVectorString(vkFlags)
            return stringFlagsList

        def __set__(self, value):
            raise RuntimeError('usageFlags cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property size:
        def __get__(self):
            """
            Size in bytes.
            """
            
            return self.__buffer.get().getSize()

        def __set__(self, value):
            raise RuntimeError('size cannot be set')

        def __del__(self):
            # nothing to do
            pass


    # @cython.infer_types(True)
    # def toHost(self):

    #     cdef unique_ptr[int] ptr = self.__buffer.get().map[int]()
