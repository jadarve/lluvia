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
import os

from libcpp cimport nullptr
from cython.operator cimport dereference as deref

from libc.stdint cimport uint32_t, uint64_t
from libcpp cimport bool
from libcpp.memory cimport shared_ptr
from libcpp.string cimport string
from libcpp.vector cimport vector

from lluvia.core import impl

# Import all Python symbols defined in the module's __init__.py and wrap them in ll_memory
import lluvia.core.memory as ll_memory

from lluvia.core.device.device_descriptor cimport DeviceDescriptor, _DeviceDescriptor

# Import all C-types needed by Cython
from lluvia.core.memory.memory cimport _buildMemory, _Memory, Memory
from lluvia.core.memory.memory_property_flags cimport _MemoryPropertyFlags

from lluvia.core.command_buffer cimport CommandBuffer, _CommandBuffer, move, _buildCommandBuffer
from lluvia.core.duration cimport Duration, _Duration, moveDuration, _buildDuration

from lluvia.core.enums.compute_dimension cimport ComputeDimension
from lluvia.core.compute_dimension cimport _ComputeDimension

# from lluvia.core cimport vulkan as vk
from lluvia.core import impl
from lluvia.core.node_new.node_type import NodeType

from lluvia.core.program cimport Program

from lluvia.core.node cimport ContainerNodeDescriptor,\
                              ContainerNode,\
                              NodeBuilderDescriptor,\
                              _buildContainerNode

from lluvia.core.node_new.compute_node cimport ComputeNode, _buildComputeNode
from lluvia.core.node_new.compute_node_descriptor cimport ComputeNodeDescriptor

from lluvia.core.types cimport _vec3ui

import lluvia.nodes as llnodes
import lluvia.glsl.lib as llGslsLib

__all__ = [
    'getAvailableDevices',
    'createSession',
    'Session'
]

def getAvailableDevices():
    """
    Returns the list of available devices.

    Returns
    -------
    devices : list of DeviceDescriptor
        The list of DeviceDescriptor avaiable to create a session from.

    See Also
    --------
    createSession : Creates a new lluvia.Session object.
    """
    cdef vector[_DeviceDescriptor] devices = _Session.getAvailableDevices()

    output = list()
    cdef DeviceDescriptor desc
    for dev in devices:
        desc = DeviceDescriptor()
        desc.__desc = dev

        output.append(desc)
    
    return output


def createSession(bool enableDebug = False, bool loadNodeLibrary = True, DeviceDescriptor device = None):
    """
    Creates a new lluvia.Session object.

    Parameters
    ----------
    enableDebug : bool defaults to False.
        Whether or not to enable the debug extensions on the GPU
        device. When enabled, the vulkan VK_LAYER_KHRONOS_validation
        layer is activated, and messages about bad usage of the API
        will appear.

        Disable debug for reducing overhead.
    
    loadNodeLibrary : bool defaults to True.
        Whether or not the standard Lluvia node library embedded in the
        Python package should be loaded as part of the session creation.
    
    device : ll.DeviceDescriptor. Defaults to None.
        The device used to create the session from. If None, the session will
        be created from the first device available in getAvailableDevices.

    Returns
    -------
    session : Session.
        New session.
    
    See Also
    --------
    getAvailableDevices : Returns the list of available devices.
    """

    cdef _SessionDescriptor desc = _SessionDescriptor()
    desc.enableDebug(enableDebug)

    if device is not None:
        desc.setDeviceDescriptor(device.__desc)

    cdef Session session = Session()
    session.__session = _Session.create(desc)

    if loadNodeLibrary:
        session.loadLibrary(os.path.join(llnodes.__path__[0], 'lluvia_node_library.zip'))

    return session


cdef class Session:

    def __cinit__(self):
        pass

    def __dealloc__(self):
        # nothing to do
        pass

    property deviceDescriptor:
        def __get__(self):
            cdef DeviceDescriptor desc = DeviceDescriptor()
            desc.__desc = self.__session.get().getDeviceDescriptor()
            return desc

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

        cdef vector[_MemoryPropertyFlags] supportedFlags = self.__session.get().getSupportedMemoryFlags()

        supportedMemoryFlags = list()

        cdef uint32_t flags_u32 = 0
        for flags in supportedFlags:

            flags_u32 = <uint32_t> flags
            flagBits = impl.expandFlagBits(flags_u32, ll_memory.MemoryPropertyFlagBits)
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
                     flags=ll_memory.MemoryPropertyFlagBits.DeviceLocal,
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

        cdef uint32_t flattenFlags = impl.flattenFlagBits(flags, ll_memory.MemoryPropertyFlagBits)
        cdef _MemoryPropertyFlags cflags = <_MemoryPropertyFlags> flattenFlags

        return _buildMemory(self.__session.get().createMemory(cflags, pageSize, exactFlagsMatch), self)

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
        IOError
            If there is any problem reading the file at the given path.
        """

        cdef Program prog = Program()

        try:
            prog.__program = self.__session.get().createProgram(impl.encodeString(path))
            return prog

        except IOError as e:
            raise IOError('Error reading SPIR-V file at: {0}. Error: {1}'.format(path, e))

    def setProgram(self, str name, program):
        """
        Sets a program into the registry with a given name

        Parameters
        ----------
        name : string
            The name of the program in the registry.

        program : string or lluvia.Program.
            If string, this parameter denotes a path to a SPIR-V file used to
            build the program. This is equivalent to call
            `session.setProgram(name, session.createProgram(program))`

        Raises
        ------
        ValueError
            If program's type is not string nor lluvia.Program
        """

        cdef Program p = Program()

        # if it is a path
        if type(program) is str:
            p = self.createProgram(program)

        elif type(program) is Program:
            p = program

        else:
            raise ValueError('Unknown type {0}, expecting string or ll.Program'.format(type(program)))

        self.__session.get().setProgram(impl.encodeString(name), p.__program)

    def getProgram(self, str name):
        """
        Returns a program from the registry

        Parameters
        ----------
        name : string.
            Name of the program.


        Returns
        -------
        program : lluvia.Program
            The program.

        Raises
        ------
        KeyError
            If the program does not exists in the registry
        """

        cdef Program out = Program()
        out.__program = self.__session.get().getProgram(impl.encodeString(name))

        if out.__program == nullptr:
            raise KeyError('program "{0}" not found'.format(name))

        return out

    def getNodeBuilderDescriptors(self):
        """
        Gets the node builder descriptors currently registered.

        Returns
        -------
        descriptors : list of NodeBuilderDescriptor
            The list of descriptors.
        """

        cdef vector[_NodeBuilderDescriptor] descriptors = self.__session.get().getNodeBuilderDescriptors()

        output = list()
        for d in descriptors:
            
            desc = NodeBuilderDescriptor(NodeType(<uint32_t> d.nodeType),
                impl.decodeString(d.name), impl.decodeString(d.summary))

            output.append(desc)

        return output

    def createComputeNodeDescriptor(self, str builderName):
        """
        Creates a ComputeNodeDescriptor from its name in the registry.

        Parameters
        ----------
        builderName : string.
            Name of the builder.
        
        Returns
        -------
        desc : lluvia.ComputeNodeDescriptor.
            The descriptor
        
        Raises
        ------
        RuntimeError
            If builderName is not in the registry.
        """

        cdef ComputeNodeDescriptor desc = ComputeNodeDescriptor()
        desc.__descriptor = self.__session.get().createComputeNodeDescriptor(impl.encodeString(builderName))
        return desc

    def createComputeNode(self, desc):
        """
        Creates a ComputeNode from a given descriptor or builder name.

        Parameters
        ----------
        desc : string or lluvia.ComputeNodeDescriptor
            If string, denotes the builder name used to create the ComputeNodeDescriptor for this node.
            Otherwise, a valid lluvia.ComputeNodeDescriptor must be provided.

        Returns
        -------
        node : lluvia.ComputeNode

        Raises
        ------
        RuntimeError
            If desc is a string and the node builder is not found in the registry.

        ValueError
            If desc type is not string nor ComputeNodeDescriptor.
        """

        cdef ComputeNodeDescriptor d = ComputeNodeDescriptor()

        if type(desc) is str:
            d = self.createComputeNodeDescriptor(desc)

        elif type(desc) is ComputeNodeDescriptor:
            d = desc

        else:
            raise ValueError('Unknown type {0}, expecting string or ComputeNodeDescriptor'.format(type(desc)))

        return _buildComputeNode(self.__session.get().createComputeNode(d.__descriptor), self)

    def createContainerNodeDescriptor(self, str builderName):
        """
        Creates a ContainerNodeDescriptor from its name in the registry.

        Parameters
        ----------
        builderName : string.
            Name of the builder.
        
        Returns
        -------
        desc : lluvia.ContainerNodeDescriptor.
            The descriptor
        
        Raises
        ------
        RuntimeError
            If builderName is not in the registry.
        """

        cdef ContainerNodeDescriptor desc = ContainerNodeDescriptor()
        desc.__descriptor = self.__session.get().createContainerNodeDescriptor(impl.encodeString(builderName))
        return desc

    def createContainerNode(self, desc):
        """
        Creates a ComputeNode from a given descriptor or builder name.

        Parameters
        ----------
        desc : string or lluvia.ContainerNodeDescriptor
            If string, denotes the builder name used to create the ContainerNodeDescriptor for this node.
            Otherwise, a valid lluvia.ContainerNodeDescriptor must be provided.

        Returns
        -------
        node : lluvia.ComputeNode

        Raises
        ------
        RuntimeError
            If desc is a string and the node builder is not found in the registry.

        ValueError
            If desc type is not string nor ContainerNodeDescriptor.
        """

        cdef ContainerNodeDescriptor d = ContainerNodeDescriptor()

        if type(desc) is str:
            d = self.createContainerNodeDescriptor(desc)

        elif type(desc) is ContainerNodeDescriptor:
            d = desc

        else:
            raise ValueError('Unknown type {0}, expecting string or ContainerNodeDescriptor'.format(type(desc)))

        return _buildContainerNode(self.__session.get().createContainerNode(d.__descriptor), self)

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
        RuntimeError
            If the command buffer cannot be created.
        """

        return _buildCommandBuffer(shared_ptr[_CommandBuffer](move(self.__session.get().createCommandBuffer())), self)

    def script(self, str code):
        """
        Runs a Lua script in the session's interpreter

        Parameters
        ----------
        code : string
            The Lua code.
        """

        self.__session.get().script(impl.encodeString(code))

    def scriptFile(self, str filename):
        """
        Read and run a Lua script file in the session's interpreter

        Parameters
        ----------
        code : string
            Path to the script file.
        """

        self.__session.get().scriptFile(impl.encodeString(filename))

    def loadLibrary(self, str filename):
        """
        Loads a library made of SPIR-V shader code and Lua scripts.

        Parameters
        ----------
        filename : string
            Path to the library file. The file must be a valid
            zip archive.

        Raises
        ------
        RuntimeError
            If there is problem reading the library file.
        """

        self.__session.get().loadLibrary(impl.encodeString(filename))

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
                       compileFlags=['-Werror'],
                       includeGlslLibrary=True):
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

        includeGlslLibrary : bool defaults to True.
            Whether or not the standard Lluvia GLSL library embedded in the
            Python package should be included (using -I).

        Returns
        -------
        program : lluvia.Program.
            Compiled program.


        Raises
        ------
        RuntimeError
            If the compilation fails.
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

            if includeDirs is None:
                includeDirs = list()
            
            elif type(includeDirs) is str:
                includeDirs = [includeDirs]

            # add the GLSL library embedded with the Python package
            if includeGlslLibrary:
                includeDirs.append(llGslsLib.__path__[0])
            
            # add -I flags
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
                           compileFlags=['-Werror'],
                           includeGlslLibrary=True):
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

        includeGlslLibrary : bool defaults to True.
            Whether or not the standard Lluvia GLSL library embedded in the
            Python package should be included (using -I).

        Returns
        -------
        node : lluvia.ComputeNode.
            Compiled node.

        Raises
        ------
        RuntimeError
            If the compilation fails.

        See also
        --------
        compileProgram : Compiles a Program from GLSL shader code.
        """

        desc = ComputeNodeDescriptor()
        desc.program = self.compileProgram(shaderCode,
                                           includeDirs,
                                           compileFlags,
                                           includeGlslLibrary)
        desc.functionName = functionName
        desc.builderName = builderName
        desc.grid = gridSize
        desc.local = localSize

        for port in ports:
            desc.addPort(port)

        return self.createComputeNode(desc)

    def getGoodComputeLocalShape(self, ComputeDimension dimensions):
        """
        Returns the suggested local grid shape for compute nodes given the number of dimensions.

        The local shape depends on the underlying device used.

        Parameters
        ----------
        dimensions : ComputeDimension
            The number of compute dimensions

        Returns
        -------
        localShape : 3-tuple
            The suggested local shape.

        """

        cdef _vec3ui localShape = self.__session.get().getGoodComputeLocalShape(<_ComputeDimension>dimensions)

        return (localShape.x, localShape.y, localShape.z)

    def help(self, str builderName):
        """
        Returns the help string of a given node builder.

        Parameters
        ----------
        builderName : str
            The builder name

        Returns
        -------
        helpString : str
            The help string.
        """

        return str(self.__session.get().help(impl.encodeString(builderName)), 'utf-8')

    def hasReceivedVulkanWarningMessages(self):
        """
        Tells whether or not this session has triggered Vulkan warning messages

        Returns
        -------
        b : bool
            Whether or not Vulkan warning messages have been received
        """

        return self.__session.get().hasReceivedVulkanWarningMessages()
