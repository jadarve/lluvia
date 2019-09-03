"""
    lluvia.core.memory
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport memory

from . import impl
from lluvia.core.enums import BufferUsageFlagBits, MemoryPropertyFlagBits

from libcpp.string cimport string
from libcpp.vector cimport vector

cimport numpy as np
import numpy as np

import  core_buffer
cimport core_buffer

# import  image
# cimport image

cimport vulkan as vk


__all__ = ['Memory']


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

            cdef uint32_t vkFlags_u32 = <uint32_t> self.__memory.get().getMemoryPropertyFlags()
            impl.expandFlagsBits(vkFlags_u32, MemoryPropertyFlagBits)

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


    def createBuffer(self, uint64_t size,
                     usageFlags=[BufferUsageFlagBits.StorageBuffer,
                                 BufferUsageFlagBits.TransferSrc,
                                 BufferUsageFlagBits.TransferDst]):
        """
        Creates a new buffer allocated into this memory.


        Parameters
        ----------
        size : uint64_t greater than zero.
            The size of the buffer in bytes.

        usageFlags : string or list of strings.
            Defaults to ['StorageBuffer', 'TransferSrc', 'TransferDst']
            Usage flags for this buffer. It must be a combination of the
            values defined in lluvia.BufferUsageFlagBits:
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

        if size <= 0:
            raise ValueError('Size must be greater than zero, got: {0}'.format(size))

        cdef uint32_t flattenFlags = impl.flattenFlagBits(usageFlags, BufferUsageFlagBits)
        cdef vk.BufferUsageFlags vkUsageFlags = <vk.BufferUsageFlags> flattenFlags

        cdef core_buffer.Buffer buf = core_buffer.Buffer()
        buf.__buffer  = self.__memory.get().createBuffer(size, vkUsageFlags)
        buf.__memory  = self
        buf.__session = self.__session

        return buf


    def createBufferFromHost(self, np.ndarray arr,
                             usageFlags=[BufferUsageFlagBits.StorageBuffer,
                                         BufferUsageFlagBits.TransferSrc,
                                         BufferUsageFlagBits.TransferDst]):
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
            values defined in lluvia.BufferUsageFlagBits:
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

        cdef core_buffer.Buffer buf = self.createBuffer(arr.nbytes, usageFlags)
        buf.fromHost(arr)
        return buf


    def createBufferLike(self, other,
                         usageFlags=[BufferUsageFlagBits.StorageBuffer,
                                     BufferUsageFlagBits.TransferSrc,
                                     BufferUsageFlagBits.TransferDst]):
        """
        Creates a Buffer with the same size in bytes as the other parameter

        The memory allocated to this buffer is not filled with any value
        at the moment of creation.

        Parameters
        ----------
        other : Numpy ndarray or lluvia Buffer or Image.

        usageFlags : string or list of strings.
            Defaults to ['StorageBuffer', 'TransferSrc', 'TransferDst'].
            Usage flags for this buffer. It must be a combination of the
            values defined in lluvia.BufferUsageFlagBits:
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

        sizeBytes = 0

        if type(other) is np.ndarray:
            sizeBytes = other.nbytes

        else:
            sizeBytes = other.size

        return self.createBuffer(sizeBytes, usageFlags)


    # def createImage(self, shape, str channelType='uint8', usageFlags=['Storage', 'Sampled', 'TransferSrc', 'TransferDst']):
    #     """
    #     Creates a new image allocated in this memory.


    #     Parameters
    #     ----------
    #     shape : list or tuple of length 1, 2, 3 or 4.
    #         Shape of the image (depth, height , width, channels). Each dimension
    #         must be greater than zero. The number of channels must in the range [1, 4].
    #         If one or more dimensions are missing, they are set to 1 by default.

    #         The shape is interpreted as follows:

    #             if len(shape) is 1:
    #                 depth    = 1
    #                 height   = 1
    #                 width    = shape[0]
    #                 channels = 1

    #             elif len(shape) is 2:
    #                 depth    = 1
    #                 height   = shape[0]
    #                 width    = shape[1]
    #                 channels = 1

    #             elif len(shape) is 3:
    #                 depth    = 1
    #                 height   = shape[0]
    #                 width    = shape[1]
    #                 channels = shape[2]

    #             else:
    #                 depth    = shape[0]
    #                 height   = shape[1]
    #                 width    = shape[2]
    #                 channels = shape[3]

    #     channelType : str. Defaults to 'uint8'.
    #         Channel type. It must be one of the strings in lluvia.ImageChannelType.

    #     usageFlags : string or list of strings.
    #         Defaults to ['Storage', 'Sampled', 'TransferSrc', 'TransferDst'].
    #         Image usage flags. It must be a combination of th strings defined
    #         in lluvia.ImageUsageFlags:

    #             - TransferSrc
    #             - TransferDst
    #             - Sampled
    #             - Storage
    #             - ColorAttachment
    #             - DepthStencilAttachment
    #             - TransientAttachment
    #             - InputAttachment

    #     Returns
    #     -------
    #     image : new Image object.


    #     Raises
    #     ------
    #     ValueError : if any parameter is not within its required range.
    #     """

    #     if len(shape) not in [1, 2, 3, 4]:
    #         raise ValueError('invalid shape length. Expected 1, 2, 3 or 4')

    #     usageFlags = impl.validateFlagStrings(image.ImageUsageFlags, usageFlags, forceList=True)
    #     channelTypeBytes = impl.validateFlagStrings(image.ImageChannelType, channelType)
        
    #     cdef uint32_t width    = 0
    #     cdef uint32_t height   = 0
    #     cdef uint32_t depth    = 0
    #     cdef uint32_t channels = 0

    #     ndim = len(shape)
    #     if ndim is 1:
    #         depth    = 1
    #         height   = 1
    #         width    = shape[0]
    #         channels = 1

    #     elif ndim is 2:
    #         depth    = 1
    #         height   = shape[0]
    #         width    = shape[1]
    #         channels = 1

    #     elif ndim is 3:
    #         depth    = 1
    #         height   = shape[0]
    #         width    = shape[1]
    #         channels = shape[2]

    #     else:
    #         depth    = shape[0]
    #         height   = shape[1]
    #         width    = shape[2]
    #         channels = shape[3]

    #     cdef image._ChannelCount cCount = image.castChannelCount[uint32_t](channels)
    #     cdef image._ChannelType cType = image.stringToChannelType(channelTypeBytes)

    #     cdef list flagsList = usageFlags
    #     cdef vk.ImageUsageFlags flags = image.vectorStringToImageUsageFlags(flagsList)

    #     cdef image._ImageDescriptor desc = image._ImageDescriptor(width, height, depth, cCount, cType, flags)

    #     cdef image.Image img = image.Image()
    #     img.__memory  = self
    #     img.__session = self.__session
    #     img.__image   = self.__memory.get().createImage(desc)

    #     img.changeLayout('General')
    #     return img


    # def createImageFromHost(self, np.ndarray arr, usageFlags=['Storage', 'Sampled', 'TransferSrc', 'TransferDst']):
    #     """
    #     Creates a lluvia.Image object from a Numpy array.

    #     The numpy array can have between 1 and 4 dimensions. Images
    #     can be 1D, 2D or 3D and can have up to 4 color channels. The
    #     width, height, depth and channels of the image are resolved
    #     as follows.

    #     if arr.ndim is 1: create a 1D image
    #     if arr.ndim is 2: create a 2D image with one color channel.
    #     if arr.ndim is 3: create a 2D image with the number of channels
    #         given by the length of the third dimension.
    #     if arr.ndim is 4: create a 3D image with the number of channels
    #         given by the length of the fourth dimension.

    #     Notice that for creating 1D images with several color channels,
    #     one needs to specify the 4 dimensions [length, 1, 1, channels].

    #     The image's channel type is deduced from arr.dtype.


    #     Parameters
    #     ----------
    #     arr : Numpy array.

    #     usageFlags : string or list of strings.
    #         Defaults to ['Storage', 'Sampled', 'TransferSrc', 'TransferDst'].
    #         Image usage flags. It must be a combination of th strings defined
    #         in lluvia.ImageUsageFlags:
    #             - TransferSrc
    #             - TransferDst
    #             - Sampled
    #             - Storage
    #             - ColorAttachment
    #             - DepthStencilAttachment
    #             - TransientAttachment
    #             - InputAttachment


    #     Returns
    #     -------
    #     img : lluvia.Image
    #         A new Image object.


    #     Raises
    #     ------
    #     RuntimeError : if the image cannot be created from this memory.
    #     """

    #     ndim = arr.ndim

    #     width    = 0
    #     height   = 0
    #     depth    = 0
    #     channels = 0

    #     if ndim is 1:
    #         depth    = 1
    #         height   = 1
    #         width    = arr.shape[0]
    #         channels = 1

    #     elif ndim is 2:
    #         depth    = 1
    #         height   = arr.shape[0]
    #         width    = arr.shape[1]
    #         channels = 1

    #     elif ndim is 3:
    #         depth    = 1
    #         height   = arr.shape[0]
    #         width    = arr.shape[1]
    #         channels = arr.shape[2]

    #     elif ndim is 4:
    #         depth    = arr.shape[0]
    #         height   = arr.shape[1]
    #         width    = arr.shape[2]
    #         channels = arr.shape[3]

    #     else:
    #         raise ValueError('arr parameter must have between 1 to 4 dimensions, got: {0}'.format(ndim))


    #     channelType = str(arr.dtype)

    #     img = self.createImage((depth, height, width, channels), channelType, usageFlags)
    #     img.fromHost(arr)
        
    #     return img
