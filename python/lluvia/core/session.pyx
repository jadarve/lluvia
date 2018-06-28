"""
    lluvia.core.session
    -------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport session

import subprocess
import tempfile

from cython.operator cimport dereference as deref

from libc.stdint cimport uint64_t
from libcpp cimport bool
from libcpp.memory cimport shared_ptr
from libcpp.string cimport string
from libcpp.vector cimport vector

from command_buffer cimport CommandBuffer, _CommandBuffer, move

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
        mem.__session = self
        
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
        """
        Creates a ComputeNode from a given descriptor.

        Parameters
        ----------
        desc : lluvia.ComputeNodeDescriptor
            Compute node descriptor for this node.


        Returns
        node : lluvia.ComputeNode
        """
        cdef compute_node.ComputeNode node = compute_node.ComputeNode()
        node.__session = self
        node.__node    = self.__session.get().createComputeNode(desc.__descriptor)
        
        return node


    def readComputeNodeDescriptor(self, str filePath):
        """
        Reads a ComputeNodeDescriptor from a given file.

        The JSON file must have the following structure:

            {
                "function": "main",
                "grid_x": 1,
                "grid_y": 1,
                "grid_z": 1,
                "local_x": 1,
                "local_y": 1,
                "local_z": 1,
                "parameters": [
                    "Buffer",
                    "ImageView"
                    "SampledImageView"
                ],
                "spirv": "base 64 SPIR-V code"
            }


        Parameters
        ----------
        filePath : string
            File path to JSON file.


        Returns
        -------
        desc : lluvia.ComputeNodeDescriptor


        Raises
        ------
        IOError : if there are problems reading the JSON file.
        """

        cdef compute_node.ComputeNodeDescriptor desc = compute_node.ComputeNodeDescriptor()
        
        desc.__descriptor = self.__session.get().readComputeNodeDescriptor(filePath)
        return desc


    def readComputeNode(self, str filePath):
        """
        Reads a ComputeNode from a given file.

        The JSON file must have the following structure:

            {
                "function": "main",
                "grid_x": 1,
                "grid_y": 1,
                "grid_z": 1,
                "local_x": 1,
                "local_y": 1,
                "local_z": 1,
                "parameters": [
                    "Buffer",
                    "ImageView"
                    "SampledImageView"
                ],
                "spirv": "base 64 SPIR-V code"
            }


        Parameters
        ----------
        filePath : string
            File path to JSON file.


        Returns
        -------
        desc : lluvia.ComputeNode


        Raises
        ------
        IOError : if there are problems reading the JSON file.
        """

        desc = self.readComputeNodeDescriptor(filePath)
        return self.createComputeNode(desc)


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

        cdef CommandBuffer cmdBuffer = CommandBuffer()
        cmdBuffer.__commandBuffer = shared_ptr[_CommandBuffer](move(self.__session.get().createCommandBuffer()))

        # hold a reference to this session to avoid deleting it before the command buffer
        cmdBuffer.__session = self

        return cmdBuffer


    def run(self, obj):
        """
        Runs a CommandBuffer or ComputeNode
        
        
        Parameters
        ----------
        obj : CommandBuffer or ComputeNode
        """

        cdef compute_node.ComputeNode node = None
        cdef CommandBuffer cmdBuffer = None

        if type(obj) == compute_node.ComputeNode:
            node = obj
            self.__session.get().run(deref(node.__node.get()))

        elif type(obj) == CommandBuffer:
            cmdBuffer = obj
            self.__session.get().run(deref(cmdBuffer.__commandBuffer.get()))
    

    def compileProgram(self, shaderCode, includeDirs=None, compileFlags=['-Werror']):
        """
        Compiles a Program from GLSL shader code.

        This method assumes that glslc command is avialable in the system.


        Parameters
        ----------
        shaderCode : file or str.
            If file, it must contain the GLSL code of the shader to compile. The
            file is not closed during the execution of this method.
            
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

        if type(shaderCode) is file:
            shaderFile = shaderCode

        else:
            shaderFile = tempfile.NamedTemporaryFile(suffix='.comp')
            shaderFile.file.writelines(shaderCode)
            shaderFile.file.flush()

        # temp file for SPIR-V output
        with tempfile.NamedTemporaryFile(suffix='.spv') as outputFile:

            command = ['glslc', '-o', outputFile.name] + compileFlags

            if includeDirs is not None:
                for incDir in includeDirs:
                    command += ['-I', incDir]
            
            command.append(shaderFile.name)
            command = ' '.join(command)
            
            proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            proc.wait()
            
            if proc.returncode != 0:
                raise RuntimeError(proc.stderr.read())

            return self.createProgram(outputFile.name)


    def compileComputeNode( self,
                            shaderCode,
                            parameters,
                            localSize    = (1, 1, 1),
                            gridSize     = (1, 1, 1),
                            includeDirs  = None,
                            compileFlags = ['-Werror']):
        """
        Compiles a ComputeNode from GLSL shader code.

        This method assumes that glslc command is avialable in the system.

        Parameters
        ----------
        shaderCode : file or str.
            If file, it must contain the GLSL code of the shader to compile. The
            file is not closed during the execution of this method.

            If str,  it must be valid GLSL code. A temporal file is created and
            its path is passed to glslc for compilation.

        parameters : list of strings.
            List of parameters the compute node receives. Each string
            must be one of the values defined in lluvia.ParameterType:
                - Buffer
                - ImageView
                - SampledImageView

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

        desc = compute_node.ComputeNodeDescriptor()
        desc.program      = self.compileProgram(shaderCode, includeDirs, compileFlags)
        desc.functionName = 'main'
        desc.grid         = gridSize
        desc.local        = localSize

        for param in parameters:
            desc.addParameter(param)

        return self.createComputeNode(desc)
