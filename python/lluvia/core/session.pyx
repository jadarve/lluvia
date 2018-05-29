"""
    lluvia.core.session
    -------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport session
from session cimport _Session

from cython.operator cimport dereference as deref
from libc.stdint cimport uint64_t
from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector

cimport memory
from memory cimport Memory, _Memory
cimport vulkan as vk


__all__ = ['Session']


cdef class Session:

    def __cinit__(self):
        self.__session = _Session.create()

    def __dealloc__(self):
        # nothing to do
        pass


    def getSupportedMemoryFlags(self):
        """
        Returns the supported memory flags for creating memories in this session.

        The length of the returned vector equals the number of Vulkan memory types
        available for the physical device this session was created from.

        Returns
        -------
        supportedMemoryFlags : list of string lists
            The supported memory flags combinations supported by this session.
        """

        cdef vector[vk.MemoryPropertyFlags] vkFlags = self.__session.get().getSupportedMemoryFlags()

        supportedMemoryFlags = []

        cdef vector[string] stringFlagsList;
        for flags in vkFlags:
            stringFlagsList = memory.memoryPropertyFlagsToVectorString(flags)
            supportedMemoryFlags.append(stringFlagsList)

        return supportedMemoryFlags


    def createMemory(self, flags, uint64_t pageSize, bool exactFlagsMatch = False):
        """
        Creates a new memory.

        ```
            import lluvia as ll

            session = ll.Session()

            flags = [ll.MemoryFlags.DeviceLocal,
                     ll.MemoryFlags.HostVisible,
                     ll.MemoryFlags.HostCoherent]

            memory = session.createMemory(flags, 4096, False)
        ```

        Parameters
        ----------
        flags : string or list of strings.
            Flags to determine the type of memory to be created.
            Each flag should be one of the strings returned by 
            lluvia.MemoryFlags.

        pageSize : uint64_t greater than zero.
            The size in bytes of each page the new memory object
            will allocate when there is no space for creating new objects.

        exactFlagsMatch : bool defaults to False.
            Tells whether or not flags should match exactly one of
            the values in lluvia.Session.getSupportedMemoryFlags()
            or if it is enough that it contains at least the flags values.

        Returns
        -------
        memory : a new lluvia.Memory object.

        """
        assert(pageSize > 0)

        # converts one string object to a list
        if type(flags) is str:
            flags = [flags]

        # convert flags to vk.MemoryPropertyFlags
        cdef list flagsList = flags
        cdef vk.MemoryPropertyFlags vkFlags = memory.vectorStringToMemoryPropertyFlags(flagsList)

        cdef Memory mem = Memory()
        mem.__memory = self.__session.get().createMemory(vkFlags, pageSize, False)

        return mem
