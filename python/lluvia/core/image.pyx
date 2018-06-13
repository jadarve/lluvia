"""
    lluvia.core.image
    -----------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport image

from . import impl


__all__ = ['Image',
           'ImageView',
           'ImageUsageFlags',
           'ImageChannelType',
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


ImageFilterMode = ['Nearest',
                   'Linear']


ImageAddressMode = ['Repeat',
                    'MirroredRepeat',
                    'ClampToEdge',
                    'ClampToBorder',
                    'MirrorClampToEdge']


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


    def createImageView(self, str filterMode, str addressMode, bool normalizedCoordinates=False, bool sampled=False):
        """
        Creates a new image view from this image.


        Parameters
        ----------


        Returns
        -------


        Raises
        ------
        """

        impl.validateFlagStrings(ImageFilterMode, [filterMode])
        impl.validateFlagStrings(ImageAddressMode, [addressMode])

        cdef _ImageViewDescriptor desc = _ImageViewDescriptor()
        desc.setFilterMode(stringToImageFilterMode(filterMode))
        desc.setAddressMode(stringToImageAddressMode(addressMode))
        desc.setNormalizedCoordinates(normalizedCoordinates)
        desc.setIsSampled(sampled)

        cdef ImageView view = ImageView()
        view.__imageView = self.__image.get().createImageView(desc)

        return view


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
