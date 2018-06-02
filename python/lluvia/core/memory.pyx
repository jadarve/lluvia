"""
    lluvia.core.memory
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport memory

import  buffer
cimport buffer
cimport vulkan as vk

from libcpp.string cimport string
from libcpp.vector cimport vector

from . import impl


__all__ = ['Memory', 'MemoryPropertyFlags']


MemoryPropertyFlags = ['DeviceLocal',
                       'HostCached',
                       'HostCoherent',
                       'HostVisible',
                       'LazylyAllocated']


cdef class Memory:

    def __cinit__(self):
        pass


    def __dealloc__(self):
        # nothing to do
        pass


    property memoryFlags:
        def __get__(self):
            """
            Memory flags for this memory.
            """

            cdef vk.MemoryPropertyFlags vkFlags = self.__memory.get().getMemoryPropertyFlags()
            cdef vector[string] stringFlagsList = memoryPropertyFlagsToVectorString(vkFlags)
            return stringFlagsList

        def __set__(self, value):
            raise RuntimeError('memoryFlags cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property pageSize:
        def __get__(self):
            """
            Page size in bytes.
            """
            
            return self.__memory.get().getPageSize()

        def __set__(self, value):
            raise RuntimeError('pageSize cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property pageCount:
        def __get__(self):
            """
            Page size in bytes.
            """
            
            return self.__memory.get().getPageCount()

        def __set__(self, value):
            raise RuntimeError('pageCount cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property isMappable:
        def __get__(self):
            """
            Page size in bytes.
            """
            
            return self.__memory.get().isMappable()

        def __set__(self, value):
            raise RuntimeError('isMappable cannot be set')

        def __del__(self):
            # nothing to do
            pass


    def isPageMappable(self, uint64_t page):
        """
        Determines if page is currently available to be mapped.

        This test checks if page is available to be mapped to host-memory by
        a given objects such as a Buffer.


        Parameters
        ----------
        page : uint64_t
            Page number.


        Returns
        -------
        True if page is mappable, False otherwise
        """

        return self.__memory.get().isPageMappable(page)


    def createBuffer(self, uint64_t size, usageFlags):
        """
        Creates a new buffer allocated into this memory.


        Parameters
        ----------
        size : uint64_t greater than zero.
            The size of the buffer in bytes.

        usageFlags : string or list of strings.
            Usage flags for this buffer. It must be a combination of the
            values defined in lluvia.BufferUsageFlags:
                - IndexBuffer
                - IndirectBuffer
                - StorageBuffer
                - StorageTexelBuffer
                - TransferDst
                - TransferSrc
                - UniformBuffer
                - UniformTexelBuffer
                - VertexBuffer


        Returns
        -------
        buf : lluvia.Buffer object.
        """

        assert(size > 0)

        if type(usageFlags) is str:
            usageFlags = [usageFlags]

        impl.validateFlagStrings(buffer.BufferUsageFlags, usageFlags)

        cdef list flagsList = usageFlags
        cdef vk.BufferUsageFlags vkUsageFlags = buffer.vectorStringToBufferUsageFLags(flagsList)

        cdef buffer.Buffer buf = buffer.Buffer()
        buf.__buffer = self.__memory.get().createBuffer(size, vkUsageFlags)

        return buf
