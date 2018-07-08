"""
    lluvia.core.image
    -----------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport image

import  numpy as np
cimport numpy as np

from . import impl


__all__ = ['Image',
           'ImageView',
           'ImageUsageFlags',
           'ImageChannelType',
           'ImageChannelTypeNumpyMap',
           'ImageFilterMode',
           'ImageAddressMode']


ImageUsageFlags = ['TransferSrc',
                   'TransferDst',
                   'Sampled',
                   'Storage',
                   'ColorAttachment',
                   'DepthStencilAttachment',
                   'TransientAttachment',
                   'InputAttachment']


ImageChannelType = ['uint8',
                    'int8',
                    'uint16',
                    'int16',
                    'float16',
                    'uint32',
                    'int32',
                    'float32',
                    'uint64',
                    'int64',
                    'float64']


ImageChannelTypeNumpyMap = {'uint8'   : np.uint8,
                            'int8'    : np.int8,
                            'uint16'  : np.uint16,
                            'int16'   : np.int16,
                            'float16' : np.float16,
                            'uint32'  : np.uint32,
                            'int32'   : np.int32,
                            'float32' : np.float32,
                            'uint64'  : np.uint64,
                            'int64'   : np.int64,
                            'float64' : np.float64}


ImageFilterMode = ['Nearest',
                   'Linear']


ImageAddressMode = ['Repeat',
                    'MirroredRepeat',
                    'ClampToEdge',
                    'ClampToBorder',
                    'MirrorClampToEdge']


ImageLayout      = ['Undefined',
                    'General',
                    'ColorAttachmentOptimal',
                    'DepthStencilAttachmentOptimal',
                    'DepthStencilReadOnlyOptimal',
                    'ShaderReadOnlyOptimal',
                    'TransferSrcOptimal',
                    'TransferDstOptimal',
                    'Preinitialized',
                    'PresentSrcKHR',
                    'SharedPresentKHR',
                    'DepthReadOnlyStencilAttachmentOptimalKHR',
                    'DepthAttachmentStencilReadOnlyOptimalKHR']


cdef class Image:
    
    def __cinit__(self):
        pass


    def __dealloc__(self):
        pass


    property width:
        def __get__(self):
            """
            Width in pixels
            """
            
            return self.__image.get().getWidth()

        def __set__(self, value):
            raise RuntimeError('width cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property height:
        def __get__(self):
            """
            Height in pixels
            """
            
            return self.__image.get().getHeight()

        def __set__(self, value):
            raise RuntimeError('height cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property depth:
        def __get__(self):
            """
            Depth in pixels
            """
            
            return self.__image.get().getDepth()

        def __set__(self, value):
            raise RuntimeError('depth cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property channels:
        def __get__(self):
            return self.__image.get().getChannelCount()

        def __set__(self, value):
            raise RuntimeError('channels cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property shape:
        def __get__(self):
            return (self.depth, self.height, self.width, self.channels)

        def __set__(self, value):
            raise RuntimeError('shape cannot be set')

        def __del__(self):
            # nothing to do
            pass
        

    property channelType:
        def __get__(self):
            return channelTypeToString(self.__image.get().getChannelType())

        def __set__(self, value):
            raise RuntimeError('channelType cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property usageFlags:
        def __get__(self):
            cdef vk.ImageUsageFlags flags = self.__image.get().getUsageFlags()
            return imageUsageFlagsToVectorString(flags)

        def __set__(self, value):
            raise RuntimeError('usageFlags cannot be set')

        def __del__(self):
            # nothing to do
            pass

    
    property size:
        def __get__(self):
            return self.__image.get().getSize()

        def __set__(self, value):
            raise RuntimeError('size cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property layout:
        def __get__(self):
            return imageLayoutToString(self.__image.get().getLayout())

        def __set__(self, value):
            raise RuntimeError('layout cannot be set')

        def __del__(self):
            # nothing to do
            pass


    def fromHost(self, np.ndarray arr):
        """
        Copies the content of a numpy array to this image.

        The shape of the array must comply with the following rules.

        * If arr.ndim == 1, then this image must be 1D
            (height, depth and channels equal to 1) and img.width == arr.shape[0]

        * If arr.ndim == 2, then this image must be 2D
            (depth and channels equal to 1) and
            img.height == arr.shape[0] and img.width == arr.shape[1].

        * If arr.ndim == 3, then this image must be 2D (depth equal to 1)
            with the number of channels equal to arr.shape[2].

        * If arr.ndim == 4, then this image must be 3D and
            img.depth == arr.shape[0] and img.height == arr.shape[1] and
            img.width == arr.shape[2] and img.chhanels == arr.chape[3]


        Parameters
        ----------
        arr : numpy.ndarray.


        Raises
        ------
        ValueError : if arr does not match this image shape.
        """

        self.__validateNumpyShape(arr)

        currentLayout = self.layout
        nextLayout    = currentLayout if currentLayout not in ['Undefined', 'Preinitialized'] else 'General'
        stageBuffer   = self.__memory.createBufferFromHost(arr)
        cmdBuffer     = self.__session.createCommandBuffer()

        cmdBuffer.begin()
        cmdBuffer.changeImageLayout(self, 'TransferDstOptimal')
        cmdBuffer.copyBufferToImage(stageBuffer, self)
        cmdBuffer.changeImageLayout(self, nextLayout)
        cmdBuffer.end()

        self.__session.run(cmdBuffer)


    def toHost(self, np.ndarray output=None):
        """
        Copies the content of this image into a numpy host array.


        Parameters
        ----------
        output : numpy.ndarray. Defaults to None.


        Raises
        ------
        ValueError : if output is different than None and does not match this image shape.
        """
        
        if output is None:
            
            h1 = self.height   == 1
            d1 = self.depth    == 1
            c1 = self.channels == 1

            shape = None

            if h1 and d1 and c1:
                shape = [self.width]

            elif not h1 and d1 and c1:
                shape = [self.height, self.width]

            elif not h1 and d1 and not c1:
                shape = [self.height, self.width, self.channels]

            else:
                shape = [self.depth, self.height, self.width, self.channels]


            output = np.zeros(shape, dtype=ImageChannelTypeNumpyMap[self.channelType])

        else:
            self.__validateNumpyShape(output)
        
        
        currentLayout = self.layout
        nextLayout    = currentLayout if currentLayout not in ['Undefined', 'Preinitialized'] else 'General'
        stageBuffer   = self.__memory.createBuffer(output.size, ['StorageBuffer', 'TransferSrc', 'TransferDst'])
        cmdBuffer     = self.__session.createCommandBuffer()

        cmdBuffer.begin()
        cmdBuffer.changeImageLayout(self, 'TransferSrcOptimal')
        cmdBuffer.copyImageToBuffer(self, stageBuffer)
        cmdBuffer.changeImageLayout(self, nextLayout)
        cmdBuffer.end()

        self.__session.run(cmdBuffer)

        return stageBuffer.toHost(output)


    def createImageView(self, str filterMode='Nearest', str addressMode='Repeat', bool normalizedCoordinates=False, bool sampled=False):
        """
        Creates a new image view from this image.


        Parameters
        ----------
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
        imgView : lluvia.ImageView
            Image view object.


        Raises
        ------
        ValueError : if either filterMode or addressMode parameter have incorrect values.

        RuntimeError : if the image view cannot be created.
        """

        impl.validateFlagStrings(ImageFilterMode, [filterMode])
        impl.validateFlagStrings(ImageAddressMode, [addressMode])

        cdef _ImageViewDescriptor desc = _ImageViewDescriptor()
        desc.setFilterMode(stringToImageFilterMode(filterMode))
        desc.setAddressMode(stringToImageAddressMode(addressMode))
        desc.setNormalizedCoordinates(normalizedCoordinates)
        desc.setIsSampled(sampled)

        cdef ImageView view = ImageView()
        view.__image     = self
        view.__imageView = self.__image.get().createImageView(desc)

        return view


    def __validateNumpyShape(self, arr):

        shape = arr.shape

        # 1D
        if arr.ndim is 1:
            if shape[0] != self.width and self.height != 1 and self.depth != 1 and self.channels != 1:
                raise ValueError('arr parameter must be a 1D array of length {0}, got length: {1}'.format(self.width, shape[0]))

        # 2D with channels == 1
        elif arr.ndim is 2:
            if shape[0] != self.height and shape[1] != self.width and self.channels != 1 and self.depth != 1:
                raise ValueError('arr parameter must be a 2D array of shape [{0}, {1}], got shape: [{2}, {3}]'.format(self.height, self.width, shape[0], shape[1]))

        # 2D with channels != 1
        elif arr.ndim is 3:
            if shape[0] != self.height and shape[1] != self.width and self.channels != shape[2] and self.depth != 1:
                raise ValueError('arr parameter must be a 3D array of shape [{0}, {1}, {2}], got shape: [{3}, {4}, {5}]'.format(self.height, self.width, self.channels, shape[0], shape[1], shape[2]))

        # 3D
        elif arr.ndim is 4:
            if shape[0] != self.depth and shape[1] != self.height and shape[2] != self.width and self.channels != shape[3]:
                raise ValueError('arr parameter must be a 4D array of shape [{0}, {1}, {2}, {3}], got shape: [{4}, {5}, {6}, {7}]'.format(self.depth, self.height, self.width, self.channels, shape[0], shape[1], shape[2], shape[3]))

        else:
            raise ValueError('arr parameter must have between 1 to 4 dimensions, got: {0}'.format(arr.ndim))


cdef class ImageView:

    def __cinit__(self):
        pass


    def __dealloc__(self):
        pass


    property filterMode:
        def __get__(self):
            return imageFilterModeToString(self.__imageView.get().getDescriptor().getFilterMode())

        def __set__(self, value):
            raise RuntimeError('filterMode cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property addressModeU:
        def __get__(self):
            return imageAddressModeToString(self.__imageView.get().getDescriptor().getAddressModeU())

        def __set__(self, value):
            raise RuntimeError('addressModeU cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property addressModeV:
        def __get__(self):
            return imageAddressModeToString(self.__imageView.get().getDescriptor().getAddressModeV())

        def __set__(self, value):
            raise RuntimeError('addressModeV cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property addressModeW:
        def __get__(self):
            return imageAddressModeToString(self.__imageView.get().getDescriptor().getAddressModeW())

        def __set__(self, value):
            raise RuntimeError('addressModeW cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property normalizedCoordinates:
        def __get__(self):
            return self.__imageView.get().getDescriptor().isNormalizedCoordinates()

        def __set__(self, value):
            raise RuntimeError('addressModeW cannot be set')

        def __del__(self):
            # nothing to do
            pass


    property sampled:
        def __get__(self):
            return self.__imageView.get().getDescriptor().isSampled()

        def __set__(self, value):
            raise RuntimeError('addressModeW cannot be set')

        def __del__(self):
            # nothing to do
            pass


    def fromHost(self, np.ndarray arr):
        """
        Copies the content of a numpy array to this image view.

        The shape of the array must comply with the following rules.

        * If arr.ndim == 1, then this image must be 1D
            (height, depth and channels equal to 1) and img.width == arr.shape[0]

        * If arr.ndim == 2, then this image must be 2D
            (depth and channels equal to 1) and
            img.height == arr.shape[0] and img.width == arr.shape[1].

        * If arr.ndim == 3, then this image must be 2D (depth equal to 1)
            with the number of channels equal to arr.shape[2].

        * If arr.ndim == 4, then this image must be 3D and
            img.depth == arr.shape[0] and img.height == arr.shape[1] and
            img.width == arr.shape[2] and img.chhanels == arr.chape[3]


        Parameters
        ----------
        arr : numpy.ndarray.


        Raises
        ------
        ValueError : if arr does not match this image shape.
        """

        self.__image.fromHost(arr)


    def toHost(self, np.ndarray output=None):
        """
        Copies the content of this image view into a numpy host array.


        Parameters
        ----------
        output : numpy.ndarray. Defaults to None.


        Raises
        ------
        ValueError : if output is different than None and does not match this image shape.
        """
        
        return self.__image.toHost(output)
