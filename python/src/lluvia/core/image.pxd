"""
    lluvia.core.image
    -----------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from memory cimport Memory
from core_object cimport _Object
from session cimport Session

cimport enums.image as img_enums

cimport vulkan as vk

from libc.stdint cimport uint64_t, uint32_t

from libcpp cimport bool
from libcpp.memory cimport shared_ptr, unique_ptr
from libcpp.vector cimport vector
from libcpp.string cimport string


cdef extern from 'lluvia/core/ImageDescriptor.h' namespace 'll':

    img_enums._ChannelCount castChannelCount[T](T c)
    uint64_t getChannelTypeSize(img_enums._ChannelType type)
    string channelTypeToString(img_enums._ChannelType&& value)
    # img_enums._ChannelType stringToChannelType(string&& stringValue) except +


    cdef cppclass _ImageDescriptor 'll::ImageDescriptor':

        _ImageDescriptor()
        _ImageDescriptor(const uint32_t width,
                    const uint32_t height,
                    const uint32_t depth,
                    img_enums._ChannelCount channelCount,
                    img_enums._ChannelType channelType,
                    const img_enums._ImageUsageFlags usageFlags)


        _ImageDescriptor& setChannelType(const img_enums._ChannelType type)
        _ImageDescriptor& setChannelCount(const img_enums._ChannelCount count)
        _ImageDescriptor& setWidth(const uint32_t width)
        _ImageDescriptor& setHeight(const uint32_t height)
        _ImageDescriptor& setDepth(const uint32_t depth)

        img_enums._ChannelType getChannelType() const
        T getChannelCount[T]()        const
        uint32_t getWidth()           const
        uint32_t getHeight()          const
        uint32_t getDepth()           const
        uint64_t getSize()            const
        img_enums._ImageType getImageType()   const
        img_enums._Format getFormat()         const


cdef extern from 'lluvia/core/Image.h' namespace 'll':

    cdef cppclass _Image 'll::Image' (_Object):

        uint64_t getSize() const

        img_enums._ImageUsageFlags getUsageFlags() const
        img_enums._ImageLayout     getLayout()     const

        img_enums._ChannelType getChannelType()    const
        uint64_t getChannelTypeSize()    const
        T getChannelCount[T]()           const
        uint32_t getWidth()              const
        uint32_t getHeight()             const
        uint32_t getDepth()              const

        shared_ptr[_ImageView] createImageView(const _ImageViewDescriptor& descriptor) except +


cdef extern from 'lluvia/core/ImageViewDescriptor.h' namespace 'll':

    cdef cppclass _ImageViewDescriptor 'll::ImageViewDescriptor':

        _ImageViewDescriptor()

        _ImageViewDescriptor& setFilterMode(img_enums._ImageFilterMode filterMode)
        img_enums._ImageFilterMode getFilterMode() const

        _ImageViewDescriptor& setAddressMode(img_enums._ImageAddressMode addressMode)
        _ImageViewDescriptor& setAddressMode(img_enums._ImageAxis axis, _ImageAddressMode addressMode)
    
        img_enums._ImageAddressMode getAddressModeU() const
        img_enums._ImageAddressMode getAddressModeV() const
        img_enums._ImageAddressMode getAddressModeW() const

        _ImageViewDescriptor& setNormalizedCoordinates(bool normalizedCoordinates)
        bool isNormalizedCoordinates() const

        _ImageViewDescriptor& setIsSampled(bool isSampled)
        bool isSampled() const


cdef extern from 'lluvia/core/ImageView.h' namespace 'll':
    
    cdef cppclass _ImageView 'll::ImageView':

        shared_ptr[_Image] getImage() const
        _ImageViewDescriptor& getDescriptor() const


cdef class Image:
    
    cdef Memory             __memory
    cdef Session            __session
    cdef shared_ptr[_Image] __image


cdef class ImageView:

    cdef Image                  __image
    cdef shared_ptr[_ImageView] __imageView
