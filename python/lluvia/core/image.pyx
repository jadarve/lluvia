"""
    lluvia.core.image
    -----------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport image


__all__ = ['Image', 'ImageUsageFlags']

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
