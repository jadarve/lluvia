"""
    lluvia.core.memory
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport memory
cimport vulkan as vk

from libcpp.string cimport string
from libcpp.vector cimport vector

from . import impl

__all__ = ['Memory', 'MemoryFlags']


MemoryFlags = impl.createEnumeration('MemoryFlags', False,
    DeviceLocal     = 'DEVICE_LOCAL',
    HostCached      = 'HOST_CACHED',
    HostCoherent    = 'HOST_COHERENT',
    HostVisible     = 'HOST_VISIBLE',
    LazylyAllocated = 'LAZILY_ALLOCATED',
    __doc__ = """
        Supported memory flags for creating a ll.Memory object.

        The following enum names are defined

        - DeviceLocal
        - HostCached
        - HostCoherent
        - HostVisible
        - LazilyAllocated

        See https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#VkMemoryPropertyFlagBits for more information.

        See also
        --------
        Session.createMemory() : creates a new memory.
        """)


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
