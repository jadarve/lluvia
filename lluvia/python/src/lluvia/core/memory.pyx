# cython: language_level=3, boundscheck=False, emit_code_comments=True, embedsignature=True

"""
    lluvia.core.memory
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libcpp.string cimport string
from libcpp.vector cimport vector

cimport numpy as np
import numpy as np

from lluvia.core cimport core_buffer
from lluvia.core.core_buffer import BufferUsageFlagBits
from lluvia.core cimport vulkan as vk
from lluvia.core import impl

from lluvia.core.image cimport image
from lluvia.core.image.image cimport _ChannelType, ChannelType, _ChannelCount, Image, _ImageDescriptor
from lluvia.core.image.image_filter_mode cimport ImageFilterMode
from lluvia.core.image.image_address_mode cimport ImageAddressMode
from lluvia.core.image.image_usage_flags cimport _ImageUsageFlags, ImageUsageFlagBits
from lluvia.core.image.image_layout cimport ImageLayout
from lluvia.core.image.image_tiling cimport _ImageTiling, ImageTiling

from lluvia.core.session cimport Session


__all__ = [
    'Memory',
    'MemoryAllocationInfo'
]

cdef _buildMemory(shared_ptr[_Memory] ptr, Session session):
    
    cdef Memory m = Memory()
    m.__memory = ptr
    m.__session = session

    return m


cdef class MemoryAllocationInfo:

    def __cinit__(self):
        pass

    def __init__(self):
        pass

    def __dealloc__(self):
        pass

    property offset:
        def __get__(self):
            return self.__allocationInfo.offset

    property size:
        def __get__(self):
            return self.__allocationInfo.size

    property leftPadding:
        def __get__(self):
            return self.__allocationInfo.leftPadding

    property page:
        def __get__(self):
            return self.__allocationInfo.page

    def __str__(self):
        return 'p:{0} o:{1} l:{2} s:{3}'.format(self.page,
                                                self.offset,
                                                self.leftPadding,
                                                self.size)


cdef class Memory:

    def __init__(self):
        self.__session = None
    
    def __cinit__(self):
        pass

    def __dealloc__(self):
        # nothing to do
        pass

    property session:
        def __get__(self):
            return self.__session

    property memoryFlags:
        def __get__(self):
            """
            Memory flags for this memory.
            """

            cdef uint32_t flags_u32 = <uint32_t> self.__memory.get().getMemoryPropertyFlags()
            return impl.expandFlagBits(flags_u32, MemoryPropertyFlagBits)

    property pageSize:
        def __get__(self):
            """
            Page size in bytes.
            """

            return self.__memory.get().getPageSize()

    property pageCount:
        def __get__(self):
            """
            Page size in bytes.
            """

            return self.__memory.get().getPageCount()

    property isMappable:
        def __get__(self):
            """
            Page size in bytes.
            """

            return self.__memory.get().isMappable()

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

        usageFlags : BufferUsageFlagBits or list of BufferUsageFlagBits.
            Defaults to:
                [BufferUsageFlagBits.StorageBuffer,
                 BufferUsageFlagBits.TransferSrc,
                 BufferUsageFlagBits.TransferDst]
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
        cdef core_buffer.BufferUsageFlags _usageFlags = <core_buffer.BufferUsageFlags> flattenFlags

        return core_buffer._buildBuffer(self.__memory.get().createBuffer(size, _usageFlags), self.session, self)


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

        usageFlags : BufferUsageFlagBits or list of BufferUsageFlagBits.
            Defaults to:
                [BufferUsageFlagBits.StorageBuffer,
                 BufferUsageFlagBits.TransferSrc,
                 BufferUsageFlagBits.TransferDst]
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

        usageFlags : BufferUsageFlagBits or list of BufferUsageFlagBits.
            Defaults to:
                [BufferUsageFlagBits.StorageBuffer,
                 BufferUsageFlagBits.TransferSrc,
                 BufferUsageFlagBits.TransferDst]
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


    def createImage(self, shape,
                    ChannelType channelType=ChannelType.Uint8,
                    usageFlags=[ImageUsageFlagBits.Storage,
                                ImageUsageFlagBits.Sampled,
                                ImageUsageFlagBits.TransferSrc,
                                ImageUsageFlagBits.TransferDst]):
        """
        Creates a new image allocated in this memory.


        Parameters
        ----------
        shape : list or tuple of length 1, 2, 3 or 4.
            Shape of the image (depth, height , width, channels). Each dimension
            must be greater than zero. The number of channels must in the range [1, 4].
            If one or more dimensions are missing, they are set to 1 by default.

            The shape is interpreted as follows:

                if len(shape) is 1:
                    depth    = 1
                    height   = 1
                    width    = shape[0]
                    channels = 1

                elif len(shape) is 2:
                    depth    = 1
                    height   = shape[0]
                    width    = shape[1]
                    channels = 1

                elif len(shape) is 3:
                    depth    = 1
                    height   = shape[0]
                    width    = shape[1]
                    channels = shape[2]

                else:
                    depth    = shape[0]
                    height   = shape[1]
                    width    = shape[2]
                    channels = shape[3]

        channelType : ll.ChannelType. Defaults to ll.ChannelType.Uint8.
            Channel type.

        usageFlags : ImageUsageFlagBits or list of ImageUsageFlagBits.
            Defaults to ['Storage', 'Sampled', 'TransferSrc', 'TransferDst'].
            Image usage flags. It must be a combination of th strings defined
            in lluvia.ImageUsageFlags:

                - TransferSrc
                - TransferDst
                - Sampled
                - Storage
                - ColorAttachment
                - DepthStencilAttachment
                - TransientAttachment
                - InputAttachment

        Returns
        -------
        image : new Image object.


        Raises
        ------
        ValueError   : if the number of dimensions is not in [1, 2, 3, 4] or
                       if any parameter is not within its required range.
        """

        depth, height, width, channels = self.__getImageShape(shape)

        cdef uint32_t flattenFlags = impl.flattenFlagBits(usageFlags, ImageUsageFlagBits)
        cdef _ImageUsageFlags vkUsageFlags = <_ImageUsageFlags> flattenFlags
        cdef _ImageTiling tiling = <_ImageTiling> ImageTiling.Optimal

        cdef _ChannelType cType = <_ChannelType> channelType

        cdef _ChannelCount cCount = image.castChannelCount[uint32_t](channels)

        cdef _ImageDescriptor desc = image._ImageDescriptor(depth, height, width, cCount, cType, vkUsageFlags, tiling)

        cdef Image img = image._buildImage(self.__memory.get().createImage(desc), self.session, self)
        img.changeLayout(ImageLayout.General)

        return img


    def createImageFromHost(self,
                            np.ndarray arr,
                            usageFlags=[ImageUsageFlagBits.Storage,
                                        ImageUsageFlagBits.Sampled, 
                                        ImageUsageFlagBits.TransferSrc, 
                                        ImageUsageFlagBits.TransferDst]):
        """
        Creates a lluvia.Image object from a Numpy array.

        The numpy array can have between 1 and 4 dimensions. Images
        can be 1D, 2D or 3D and can have up to 4 color channels. The
        width, height, depth and channels of the image are resolved
        as follows.

        if arr.ndim is 1: create a 1D image
        if arr.ndim is 2: create a 2D image with one color channel.
        if arr.ndim is 3: create a 2D image with the number of channels
            given by the length of the third dimension.
        if arr.ndim is 4: create a 3D image with the number of channels
            given by the length of the fourth dimension.

        Notice that for creating 1D images with several color channels,
        one needs to specify the 4 dimensions [length, 1, 1, channels].

        The image's channel type is deduced from arr.dtype.


        Parameters
        ----------
        arr : Numpy array.

        usageFlags : string or list of strings.
            Defaults to ['Storage', 'Sampled', 'TransferSrc', 'TransferDst'].
            Image usage flags. It must be a combination of th strings defined
            in lluvia.ImageUsageFlags:
                - TransferSrc
                - TransferDst
                - Sampled
                - Storage
                - ColorAttachment
                - DepthStencilAttachment
                - TransientAttachment
                - InputAttachment


        Returns
        -------
        img : lluvia.Image
            A new Image object.


        Raises
        ------
        ValueError   : if the number of dimensions is not in [1, 2, 3, 4] or
                       if arr.dtype is incompatible with image ChannelType.
        RuntimeError : if the image cannot be created from this memory.
        """

        from lluvia.core.image import ImageChannelTypeToNumpyMap

        shape = [arr.shape[n] for n in range(arr.ndim)]
        depth, height, width, channels = self.__getImageShape(shape)

        channelType = None
        for llChannelType, dtype in ImageChannelTypeToNumpyMap.items():
            if dtype == arr.dtype:
                channelType = llChannelType
                break

        if channelType is None:
            raise ValueError('arr parameter has unsupported dtype: {0}'.format(arr.dtype))

        img = self.createImage((depth, height, width, channels), channelType, usageFlags)
        img.fromHost(arr)
        
        return img

    def createImageViewFromHost(self,
                            np.ndarray arr,
                            usageFlags=[ImageUsageFlagBits.Storage,
                                        ImageUsageFlagBits.Sampled, 
                                        ImageUsageFlagBits.TransferSrc, 
                                        ImageUsageFlagBits.TransferDst],
                            ImageFilterMode filterMode=ImageFilterMode.Nearest,
                            ImageAddressMode addressMode=ImageAddressMode.Repeat,
                            bool normalizedCoordinates=False,
                            bool sampled=False):
        """
        Creates a lluvia.Image object from a Numpy array.

        The numpy array can have between 1 and 4 dimensions. Images
        can be 1D, 2D or 3D and can have up to 4 color channels. The
        width, height, depth and channels of the image are resolved
        as follows.

        if arr.ndim is 1: create a 1D image
        if arr.ndim is 2: create a 2D image with one color channel.
        if arr.ndim is 3: create a 2D image with the number of channels
            given by the length of the third dimension.
        if arr.ndim is 4: create a 3D image with the number of channels
            given by the length of the fourth dimension.

        Notice that for creating 1D images with several color channels,
        one needs to specify the 4 dimensions [length, 1, 1, channels].

        The image's channel type is deduced from arr.dtype.


        Parameters
        ----------
        arr : Numpy array.

        usageFlags : string or list of strings.
            Defaults to ['Storage', 'Sampled', 'TransferSrc', 'TransferDst'].
            Image usage flags. It must be a combination of th strings defined
            in lluvia.ImageUsageFlags:
                - TransferSrc
                - TransferDst
                - Sampled
                - Storage
                - ColorAttachment
                - DepthStencilAttachment
                - TransientAttachment
                - InputAttachment

        filterMode : str. Defaults to 'Nearest'.
            Filtering more for reading pixels within a shader. Possible
            values are defined in lluvia.ImageFilterMode:
                - Nearest
                - Linear

        addressMode : str. Defaults to 'Repeat'.
            Addressing mode for reading pixels that are outside of the image
            boundaries. Possible values are defined in lluvia.ImageAddressMode:
                - Repeat
                - MirroredRepeat
                - ClampToEdge
                - ClampToBorder
                - MirrorClampToEdge

        normalizedCoordinates : bool. Defaults to False.
            Tells whether or not to use normalized coordinates to read
            pixels within a shader.

        sampled : bool. Defaults to False.
            Tells whether or not to use a sampler object for reading
            pixels within a shader.
        
        Returns
        -------
        img : lluvia.Image
            A new Image object.


        Raises
        ------
        ValueError   : if the number of dimensions is not in [1, 2, 3, 4] or
                       if arr.dtype is incompatible with image ChannelType.
        RuntimeError : if the image cannot be created from this memory.
        """

        image = self.createImageFromHost(arr, usageFlags)

        return image.createImageView(filterMode, addressMode, normalizedCoordinates, sampled)

    def __getImageShape(self, shape):
        """
        Calculates the lluvia image shape given a tuple
        """

        if len(shape) not in [1, 2, 3, 4]:
            raise ValueError('invalid number of dimensions. Expected 1, 2, 3 or 4')

        cdef uint32_t width    = 0
        cdef uint32_t height   = 0
        cdef uint32_t depth    = 0
        cdef uint32_t channels = 0

        ndim = len(shape)
        if ndim is 1:
            depth    = 1
            height   = 1
            width    = shape[0]
            channels = 1

        elif ndim is 2:
            depth    = 1
            height   = shape[0]
            width    = shape[1]
            channels = 1

        elif ndim is 3:
            depth    = 1
            height   = shape[0]
            width    = shape[1]
            channels = shape[2]

        else:
            depth    = shape[0]
            height   = shape[1]
            width    = shape[2]
            channels = shape[3]

        return depth, height, width, channels
