# cython: language_level=3, boundscheck=False, emit_code_comments=True, embedsignature=True

"""
    lluvia.core.core_buffer
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint64_t, uint32_t
from libc.string cimport memcpy
from libcpp.memory cimport unique_ptr, shared_ptr

cimport numpy as np
import numpy as np

from lluvia.core import impl

from lluvia.core.memory cimport Memory, MemoryAllocationInfo, _buildMemory
from lluvia.core.memory import MemoryPropertyFlagBits

from lluvia.core.session cimport Session


__all__ = ['Buffer']


cdef _buildBuffer(shared_ptr[_Buffer] ptr, Session session, Memory memory):

    cdef Buffer buf = Buffer()
    buf.__buffer = ptr
    buf.__session = session

    if memory is None:
        buf.__memory = _buildMemory(ptr.get().getMemory(), session)
    
    return buf


cdef class Buffer:

    def __cinit__(self):
        self.__session = None
        self.__memory = None

    def __dealloc__(self):
        pass

    property session:
        def __get__(self):
            return self.__session

    property memory:
        def __get__(self):
            return self.__memory

    property usageFlags:
        def __get__(self):
            """
            Usage flags for this buffer.
            """

            cdef uint32_t flags_u32 = <uint32_t> self.__buffer.get().getUsageFlags()
            return impl.expandFlagBits(flags_u32, BufferUsageFlagBits)

    property size:
        def __get__(self):
            """
            Size in bytes.
            """

            return self.__buffer.get().getSize()

    property allocationInfo:
        def __get__(self):
            """
            Memory allocation information
            """

            cdef MemoryAllocationInfo allocInfo = MemoryAllocationInfo()
            allocInfo.__allocationInfo = self.__buffer.get().getAllocationInfo()
            return allocInfo

    property isMappable:
        def __get__(self):
            return self.__buffer.get().isMappable()

    def toHost(self, np.ndarray output=None, dtype=np.uint8):
        """
        Copies the content of this buffer into a numpy host array.

        Parameters
        ----------
        output : numpy.ndarray. Defaults to None.
            Numpy array where the memory content of this buffer will be
            copied to. If None, a new Numpy array will be created with the
            same size in bytes as this buffer.

        dtype : numpy.dtype. Defaults to numpy.uint8.
            Data type of the returned array.


        Returns
        -------
        output : a new numpy array if input parameter output was None
                 or the same output object otherwise.


        Raises
        ------
        RuntimeError : if the buffer is not mappable to host-visible memory.
            This might be caused by another object mapping the same memory
            page as this one or because the memory this buffer was created from
            is not mappable to host-visible memory, e.g., memories created with
            'DeviceLocal' flag.

        ValueError : if output is different than None but its size in bytes
            output.nbytes is different than this buffer's size.
        """

        cdef uint64_t sizeBytes = self.__buffer.get().getSize()

        if output is None:
            output = np.zeros(sizeBytes, dtype=np.uint8)

            # reinterpret the bytes of output array as dtype
            output.dtype = dtype

        else:
            if output.nbytes != sizeBytes:
                raise ValueError('output buffer parameter must have the same size in bytes as this buffer, got: {0} expected: {1}'.format(output.nbytes, sizeBytes))


        cdef unique_ptr[char, _BufferMapDeleter] mappedPtr

        if self.isMappable:

            mappedPtr = self.__buffer.get().map[char]()
            memcpy(<void*>output.data, <void*>mappedPtr.get(), sizeBytes)

            return output

        ######################
        # not mappable buffer!
        ######################

        # create a stage buffer and copy the content of arr to it
        mapFlags = [MemoryPropertyFlagBits.HostVisible,
                    MemoryPropertyFlagBits.HostCoherent]
        cdef Memory mappableMemory = self.session.createMemory(mapFlags,
                                                                 sizeBytes,
                                                                 False)

        stageFlags = [BufferUsageFlagBits.StorageBuffer,
                      BufferUsageFlagBits.TransferDst]
        cdef Buffer stageBuffer = mappableMemory.createBuffer(sizeBytes,
                                                              stageFlags)

        # create a command buffer to issue the copy to this buffer
        cmdBuffer = self.session.createCommandBuffer()

        cmdBuffer.begin()
        cmdBuffer.copyBuffer(self, stageBuffer)
        cmdBuffer.end()

        self.session.run(cmdBuffer)

        return stageBuffer.toHost(output, dtype)

    def fromHost(self, np.ndarray input):
        """
        Copies the content of a Numpy array to this buffer.


        Parameters
        ----------
        input : numpy.ndarray.
            Input array. Its size in bytes (input.nbytes) must be equal to
            this buffer size.


        Raises
        ------
        ValueError : if the size of the input array in bytes is different
            to this buffer size.
        """

        cdef uint64_t sizeBytes = self.__buffer.get().getSize()

        if input.nbytes != sizeBytes:
            raise ValueError('input buffer parameter must have the same size in bytes as this buffer, got: {0} expected: {1}'.format(input.nbytes, sizeBytes))

        # make the input array contiguous if it is not already
        if not input.flags['C_CONTIGUOUS']:
            input = input.copy()

        cdef unique_ptr[char, _BufferMapDeleter] mappedPtr;
        if self.isMappable:

            mappedPtr = self.__buffer.get().map[char]()
            memcpy(<void*>mappedPtr.get(), <void*>input.data, sizeBytes)

            # unmap the buffer
            mappedPtr.reset()
            return

        ######################
        # not mappable buffer!
        ######################

        # create a stage buffer and copy the content of arr to it
        mapFlags = [MemoryPropertyFlagBits.HostVisible,
                    MemoryPropertyFlagBits.HostCoherent]
        cdef Memory mappableMemory = self.session.createMemory(mapFlags,
                                                                 sizeBytes,
                                                                 False)
        stageFlags = [BufferUsageFlagBits.StorageBuffer,
                      BufferUsageFlagBits.TransferSrc]
        cdef Buffer stageBuffer = mappableMemory.createBuffer(sizeBytes,
                                                              stageFlags)

        # copy input into stageBuffer
        mappedPtr = stageBuffer.__buffer.get().map[char]()
        memcpy(<void*>mappedPtr.get(), <void*>input.data, sizeBytes)

        # unmap stageBuffer
        mappedPtr.reset()

        # create a command buffer to issue the copy to stageBuffer
        cmdBuffer = self.session.createCommandBuffer()

        cmdBuffer.begin()
        cmdBuffer.copyBuffer(stageBuffer, self)
        cmdBuffer.end()

        self.session.run(cmdBuffer)

    def copy(self, Buffer output=None):
        """
        Copies the content of this buffer into output.

        If output is None, a new buffer is created in the same memory
        as this one and with the same usage flags.


        Parameters
        ----------
        output : Buffer. Defaults to None.
            Buffer where the content of this buffer will be copied to.
            Its size must be equal as this buffer.


        Returns
        -------
        output : Buffer.
            If output parameter is None, returns a new Buffer object,
            otherwise returns the same output object.
        """

        if output is None:
            output = self.__memory.createBuffer(self.size, self.usageFlags)

        cmdBuffer = self.session.createCommandBuffer()

        cmdBuffer.begin()
        cmdBuffer.copyBuffer(self, output)
        cmdBuffer.end()

        self.session.run(cmdBuffer)

        return output
