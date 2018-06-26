"""
    lluvia.core.memory
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport memory

from libcpp.string cimport string
from libcpp.vector cimport vector

cimport numpy as np
import numpy as np

import  buffer
cimport buffer

import  image
cimport image

cimport vulkan as vk

from . import impl


__all__ = ['Memory', 'MemoryPropertyFlags']


MemoryPropertyFlags = ['DeviceLocal',
                       'HostCached',
                       'HostCoherent',
                       'HostVisible',
                       'LazylyAllocated']


cdef class Memory:

    def __cinit__(self):
        
        self.__session = None


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


    def createBuffer(self, uint64_t size, usageFlags=['StorageBuffer', 'TransferSrc', 'TransferDst']):
        """
        Creates a new buffer allocated into this memory.


        Parameters
        ----------
        size : uint64_t greater than zero.
            The size of the buffer in bytes.

        usageFlags : string or list of strings.
            Defaults to ['StorageBuffer', 'TransferSrc', 'TransferDst']
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
        buf.__buffer  = self.__memory.get().createBuffer(size, vkUsageFlags)
        buf.__memory  = self
        buf.__session = self.__session

        return buf


    def createImage(self, shape, uint32_t channels=1, str channelType='uint8', usageFlags=['Storage', 'TransferSrc', 'TransferDst']):
        """
        Creates a new image allocated in this memory.


        Parameters
        ----------
        shape : list or tuple of length 1, 2 or 3.
            Shape of the image (width, height , depth). Each dimension
            must be greater than zero. If one or more dimensions are missing,
            they are set to 1 by default.

        channels : uint32. Defaults to 1.
            Number of channels in each pixel location. The value
            must be in the range [1, 4].

        channelType : str. Defaults to 'uint8'.
            Channel type. It must be one of the strings in lluvia.ImageChannelType.

        usageFlags : string or list of strings.
            Defaults to ['Storage', 'TransferSrc', 'TransferDst'].
            Image usage flags. It must be a combination of th strings defined
            in lluvia.ImageUsageFlags.


        Returns
        -------
        image : new Image object.


        Raises
        ------
        ValueError : if any parameter is not within its required range.
        """

        if len(shape) not in [1, 2, 3]:
            raise ValueError('invalid shape length. Expected a ')

        if type(usageFlags) is str:
            usageFlags = [usageFlags]

        impl.validateFlagStrings(image.ImageUsageFlags, usageFlags)

        impl.validateFlagStrings(image.ImageChannelType, [channelType])

        cdef uint32_t width  = shape[0]
        cdef uint32_t height = shape[1] if len(shape) >= 2 else 1
        cdef uint32_t depth  = shape[2] if len(shape) == 3 else 1

        cdef image._ChannelType cType = image.stringToChannelType(channelType)

        cdef image._ImageDescriptor desc = image._ImageDescriptor(width, height, depth, channels, cType)

        cdef list flagsList = usageFlags
        cdef vk.ImageUsageFlags flags = image.vectorStringToImageUsageFlags(flagsList)

        cdef image.Image img = image.Image()
        img.__image = self.__memory.get().createImage(desc, flags)

        return img


    def createBufferFromHost(self, np.ndarray arr, usageFlags=['StorageBuffer', 'TransferSrc', 'TransferDst']):
        """
        Creates a buffer from a numpy array.


        Parameters
        ----------
        arr : np.ndarray.
            Numpy array from which the buffer will be created from. The content
            of the array will be copied into the buffer.

        usageFlags : string or list of strings.
            Defaults to ['StorageBuffer', 'TransferSrc', 'TransferDst'].
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
        buffer : Buffer.
            Buffer object with the same content as the input array parameter.
        """

        # cdef uint64_t size = arr.nbytes
        cdef buffer.Buffer buf = self.createBuffer(arr.nbytes, usageFlags)
        buf.fromHost(arr)
        return buf


    def createImageFromHost(self, np.ndarray arr):

        # some memories do not support the creation of images.

        pass
