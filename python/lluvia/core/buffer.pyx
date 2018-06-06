"""
    lluvia.core.buffer
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport buffer

import cython
from cython.operator cimport dereference as deref

from libc.stdint cimport uint64_t
from libc.string cimport memcpy
from libcpp.memory cimport unique_ptr

cimport numpy as np
import numpy as np


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
        output : a new numpy array if input parameter output was None or the same
            output object otherwise.


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

        if not self.__buffer.get().isMappable():
            raise RuntimeError('buffer cannot be mapped to host memory. '\
                               'Either the buffer is not host-visible or the '\
                               'memory page is currently mapped by another object.')

        cdef unique_ptr[char, _BufferMapDeleter] ptr = self.__buffer.get().map[char]()
        cdef uint64_t sizeBytes = self.__buffer.get().getSize()

        if output is None:
            output = np.zeros(sizeBytes, dtype=np.uint8)

            # reinterpret the bytes of output array as dtype
            output.dtype = dtype

        else:
            if output.nbytes != sizeBytes:
                raise ValueError('output buffer parameter must have the same size in bytes as this buffer, got: {0} expected: {1}'.format(output.nbytes, sizeBytes))

        memcpy(<void*>output.data, <void*>ptr.get(), sizeBytes)

        return output


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

        cdef unique_ptr[char, _BufferMapDeleter] ptr = self.__buffer.get().map[char]()
        memcpy(<void*>ptr.get(), <void*>input.data, sizeBytes)
