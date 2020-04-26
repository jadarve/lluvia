# cython: language_level=3, boundscheck=False, emit_code_comments=True, embedsignature=True

"""
    lluvia.core.session
    -------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

import subprocess
import tempfile
import sys
import io

from cython.operator cimport dereference as deref

from libc.stdint cimport uint32_t, uint64_t
from libcpp cimport bool
from libcpp.memory cimport shared_ptr
from libcpp.string cimport string
from libcpp.vector cimport vector

from lluvia.core cimport vulkan as vk
from lluvia.core import impl
from lluvia.core.command_buffer cimport CommandBuffer, _CommandBuffer, move, _buildCommandBuffer
from lluvia.core.duration cimport Duration, _Duration, moveDuration, _buildDuration
from lluvia.core.enums import BufferUsageFlagBits, MemoryPropertyFlagBits
from lluvia.core.memory cimport Memory, _Memory, _buildMemory
from lluvia.core.program cimport Program

from lluvia.core.node cimport ComputeNode,\
                              ComputeNodeDescriptor,\
                              ContainerNodeDescriptor,\
                              ContainerNode,\
                              _buildComputeNode,\
                              _buildContainerNode


__all__ = [
    'createSession',
    'Session'
]


def createSession():
    """
    Creates a new lluvia.Session object.

    Returns
    -------
    session : Session.
        New session.
    """

    cdef Session out = Session()
    out.__session = _Session.create()
    return out


cdef class Session:

    def __cinit__(self):
        pass

    def __dealloc__(self):
        # nothing to do
        pass

    def getSupportedMemoryPropertyFlags(self):
        """
        Returns the supported memory property flags for
        creating memories in this session.

        The length of the returned vector equals the number
        of Vulkan memory types available for the physical device
        this session was created from.

        See https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#VkMemoryPropertyFlagBits
        for more information.


        Returns
        -------
        supportedMemoryFlags : list of string lists
            The supported memory property flags combinations supported
            by this session.
        """

        cdef vector[vk.MemoryPropertyFlags] vkFlags = self.__session.get().getSupportedMemoryFlags()

        supportedMemoryFlags = list()

        cdef uint32_t flags_u32 = 0
        for flags in vkFlags:

            flags_u32 = <uint32_t> flags
            flagBits = impl.expandFlagBits(flags_u32, MemoryPropertyFlagBits)
            supportedMemoryFlags.append(flagBits)

        return supportedMemoryFlags

    def getHostMemory(self):
        """
        Returns a memory object that is HOST_LOCAL and HOST_COHERENT. This memory can
        be used to create uniform buffers to pass to shaders.

        Returns
        -------
        mem : ll.Memory
        """

        cdef Memory mem = Memory()
        mem.__memory = self.__session.get().getHostMemory()

        return mem

    def createMemory(self,
                     flags=MemoryPropertyFlagBits.DeviceLocal,
                     uint64_t pageSize=33554432L,
                     bool exactFlagsMatch=False):
        """
        Creates a new memory.

        ```
            import lluvia as ll

            session = ll.createSession()
            memory = session.createMemory([ll.MemoryPropertyFlagBits.HostVisible,
                                           ll.MemoryPropertyFlagBits.HostCoherent],
                                          4096, False)
        ```

        The flags parameter can contain one string or a list of strings
        specifying the memory property flags the new memory will be created
        with. The possible values are defined in lluvia.MemoryPropertyFlagBits:

        - DeviceLocal
        - HostCached
        - HostCoherent
        - HostVisible
        - LazilyAllocated

        See https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#VkMemoryPropertyFlagBits for more information.


        Parameters
        ----------
        flags : string or list of strings. Defaults to 'DeviceLocal'
            Flags to determine the type of memory to be created.
            Each flag should be one of the strings in
            lluvia.MemoryPropertyFlags.

        pageSize : uint64_t. Defaults to 32MB (33554432L).
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

        cdef uint32_t flattenFlags = impl.flattenFlagBits(flags, MemoryPropertyFlagBits)
        cdef vk.MemoryPropertyFlags vkFlags = <vk.MemoryPropertyFlags> flattenFlags

        return _buildMemory(self.__session.get().createMemory(vkFlags, pageSize, exactFlagsMatch), self)

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

        cdef Program prog = Program()

        try:
            prog.__program = self.__session.get().createProgram(impl.encodeString(path))
            return prog

        except IOError as e:
            raise IOError('Error reading SPIR-V file at: {0}. Error: {1}'.format(path, e))

    def setProgram(self, str name, Program program):

        self.__session.get().setProgram(impl.encodeString(name), program.__program)

    def getProgram(self, str name):

        cdef Program out = Program()
        out.__program = self.__session.get().getProgram(impl.encodeString(name))
        return out

    def createComputeNodeDescriptor(self, str builderName):

        cdef ComputeNodeDescriptor desc = ComputeNodeDescriptor()
        desc.__descriptor = self.__session.get().createComputeNodeDescriptor(impl.encodeString(builderName))
        return desc

    def createComputeNode(self, ComputeNodeDescriptor desc):
        """
        Creates a ComputeNode from a given descriptor.

        Parameters
        ----------
        desc : lluvia.ComputeNodeDescriptor
            Compute node descriptor for this node.


        Returns
        node : lluvia.ComputeNode
        """

        return _buildComputeNode(self.__session.get().createComputeNode(desc.__descriptor), self)

    def createContainerNodeDescriptor(self, str builderName):

        cdef ContainerNodeDescriptor desc = ContainerNodeDescriptor()
        desc.__descriptor = self.__session.get().createContainerNodeDescriptor(impl.encodeString(builderName))
        return desc

    def createContainerNode(self, ContainerNodeDescriptor desc):

        return _buildContainerNode(self.__session.get().createContainerNode(desc.__descriptor), self)

    def createDuration(self):
        """
        Creates a Duration object.

        Returns
        -------
        d : ll.Duration.
            A new Duration object.
        """

        return _buildDuration(shared_ptr[_Duration](moveDuration(self.__session.get().createDuration())))

    def createCommandBuffer(self):
        """
        Creates a command buffer object.

        Command buffers are used to record commands to be executed
        by the device. Once the recording finishes, the command buffer
        can be sent for execution using the `run` method.

        Raises
        ------
        RuntimeError : if the command buffer cannot be created.
        """

        return _buildCommandBuffer(shared_ptr[_CommandBuffer](move(self.__session.get().createCommandBuffer())), self)

    def script(self, str code):

        self.__session.get().script(impl.encodeString(code))

    def scriptFile(self, str filename):

        self.__session.get().scriptFile(impl.encodeString(filename))

    def run(self, obj):
        """
        Runs a CommandBuffer or ComputeNode

        Parameters
        ----------
        obj : CommandBuffer or ComputeNode
        """

        cdef ComputeNode node = None
        cdef ContainerNode containerNode = None
        cdef CommandBuffer cmdBuffer = None

        if type(obj) == ComputeNode:
            node = obj
            self.__session.get().run(deref(node.__node.get()))
        
        elif type(obj) == ContainerNode:
            containerNode = obj
            self.__session.get().run(deref(containerNode.__node.get()))

        elif type(obj) == CommandBuffer:
            cmdBuffer = obj
            self.__session.get().run(deref(cmdBuffer.__commandBuffer.get()))
        
        else:
            raise RuntimeError('Unsupported obj type: %s'.format(type(obj)))

        

    def compileProgram(self, shaderCode,
                       includeDirs=None,
                       compileFlags=['-Werror']):
        """
        Compiles a Program from GLSL shader code.

        This method assumes that glslc command is avialable in the system.


        Parameters
        ----------
        shaderCode : file or str.
            If file, it must contain the GLSL code of the shader to compile.
            The file is not closed during the execution of this method.

            If str,  it must be valid GLSL code. A temporal file is created and
            its path is passed to glslc for compilation.

        includeDirs : list of strings. Defaults to None.
            List of include directories to pass to glslc through -I flag.

        compileFlags : list of strings. Defaults to ['-Werror'].
            Extra compile flags to pass to glslc.


        Returns
        -------
        program : lluvia.Program.
            Compiled program.


        Raises
        ------
        RuntimeError : if the compilation fails.
        """

        shaderFile = None

        if type(shaderCode) is not str:
            shaderFile = shaderCode

        else:
            shaderFile = tempfile.NamedTemporaryFile(suffix='.comp')
            shaderFile.file.write(impl.encodeString(shaderCode))
            shaderFile.file.flush()

        # temp file for SPIR-V output
        with tempfile.NamedTemporaryFile(suffix='.spv') as outputFile:

            command = ['glslc', '-o', outputFile.name] + compileFlags

            if includeDirs is not None:

                if type(includeDirs) is str:
                    includeDirs = [includeDirs]

                for incDir in includeDirs:
                    command += ['-I', incDir]

            command.append(shaderFile.name)
            command = ' '.join(command)

            proc = subprocess.Popen(command,
                                    shell=True,
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE)
            proc.wait()

            if proc.returncode != 0:
                raise RuntimeError(proc.stderr.read())

            return self.createProgram(outputFile.name)

    def compileComputeNode(self,
                           ports,
                           shaderCode,
                           functionName='main',
                           builderName='',
                           localSize=(1, 1, 1),
                           gridSize=(1, 1, 1),
                           includeDirs=None,
                           compileFlags=['-Werror']):
        """
        Compiles a ComputeNode from GLSL shader code.

        This method assumes that glslc command is avialable in the system.

        Parameters
        ----------
        ports : list of PortDescriptor.
            List of port descriptors the compute node receives.

        shaderCode : file or str.
            If file, it must contain the GLSL code of the shader to compile.
            The file is not closed during the execution of this method.

            If str,  it must be valid GLSL code. A temporal file is created and
            its path is passed to glslc for compilation.

        functionName : str. Defaults to 'main'.
            Function name whitin the shader the compute node will execute.

        builderName : str. Defaults to '' (empty string).
            Builder name associated to this node.

        localSize : list or tuple of length 3. Defaults to (1, 1, 1).
            Local group size for each XYZ dimension. Each value
            must be greater or equal to 1.

        gridSize : list or tuple of length 3. Defaults to (1, 1, 1).
            Grid size for each XYZ dimension. The grid size defines
            the number of local groups in each dimension. Each value
            must be greater or equal to 1.

        includeDirs : list of strings. Defaults to None.
            List of include directories to pass to glslc through -I flag.

        compileFlags : list of strings. Defaults to ['-Werror'].
            Extra compile flags to pass to glslc.


        Returns
        -------
        node : lluvia.ComputeNode.
            Compiled node.


        Raises
        ------
        RuntimeError : if the compilation fails.


        See also
        --------
        compileProgram : Compiles a Program from GLSL shader code.
        """

        desc = ComputeNodeDescriptor()
        desc.program = self.compileProgram(shaderCode,
                                           includeDirs,
                                           compileFlags)
        desc.functionName = functionName
        desc.builderName = builderName
        desc.grid = gridSize
        desc.local = localSize

        for port in ports:
            desc.addPort(port)

        return self.createComputeNode(desc)
