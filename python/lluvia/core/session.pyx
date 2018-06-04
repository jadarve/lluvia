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

import  memory
cimport memory
from memory cimport Memory, _Memory
cimport vulkan as vk

import  program
cimport program

import compute_node
cimport compute_node

from . import impl

__all__ = ['Session']


cdef class Session:

    def __cinit__(self):
        self.__session = _Session.create()

    def __dealloc__(self):
        # nothing to do
        pass


    def getSupportedMemoryPropertyFlags(self):
        """
        Returns the supported memory property flags for creating memories in this session.

        The length of the returned vector equals the number of Vulkan memory types
        available for the physical device this session was created from.

        See https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#VkMemoryPropertyFlagBits for more information.


        Returns
        -------
        supportedMemoryFlags : list of string lists
            The supported memory property flags combinations supported by this session.
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
            memory = session.createMemory(['HostVisible', 'HostCoherent'], 4096, False)
        ```

        The flags parameter can contain one string or a list of strings
        specifying the memory property flags the new memory will be created
        with. The possible string values are defined in lluvia.MemoryPropertyFlags:

        - DeviceLocal
        - HostCached
        - HostCoherent
        - HostVisible
        - LazilyAllocated

        See https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#VkMemoryPropertyFlagBits for more information.


        Parameters
        ----------
        flags : string or list of strings.
            Flags to determine the type of memory to be created.
            Each flag should be one of the strings in
            lluvia.MemoryPropertyFlags.

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


        See also
        --------
        getSupportedMemoryPropertyFlags : Returns the supported memory property
            flags for creating memories in this session.

        """
        assert(pageSize > 0)

        # converts one string object to a list
        if type(flags) is str:
            flags = [flags]

        # sanitize flags
        impl.validateFlagStrings(memory.MemoryPropertyFlags, flags)

        # convert flags to vk.MemoryPropertyFlags
        cdef list flagsList = flags
        cdef vk.MemoryPropertyFlags vkFlags = memory.vectorStringToMemoryPropertyFlags(flagsList)

        cdef Memory mem = Memory()
        mem.__memory = self.__session.get().createMemory(vkFlags, pageSize, False)

        return mem


    def createProgram(self, str path):
        """
        Creates a Program object reading the SPIR-V code from a given file.

        Parameters
        ----------
        path : string
            path to the file where the SPIR-V code is stored.


        Returns
        -------
        program : lluvia.Program


        Raises
        ------
        IOError : if there is any problem reading the file at the given path.
        """

        cdef program.Program prog = program.Program()

        try:
            prog.__program = self.__session.get().createProgram(path)
            return prog

        except IOError as e:
            raise IOError('Error reading SPIR-V file at: {0}. Error: {1}'.format(path, e))


    def createComputeNode(self, compute_node.ComputeNodeDescriptor desc):

        cdef compute_node.ComputeNode node = compute_node.ComputeNode()
        node.__node = self.__session.get().createComputeNode(desc.__descriptor)
        
        return node
