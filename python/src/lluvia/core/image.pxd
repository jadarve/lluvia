"""
    lluvia.core.image
    -----------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint64_t, uint32_t

from libcpp cimport bool
from libcpp.memory cimport shared_ptr, unique_ptr
from libcpp.vector cimport vector
from libcpp.string cimport string

from lluvia.core cimport vulkan as vk
from lluvia.core.core_object cimport _Object
from lluvia.core.enums cimport image as img_enums
from lluvia.core.memory cimport _Memory, _MemoryAllocationInfo
from lluvia.core.memory cimport Memory
from lluvia.core.session cimport Session


cdef extern from 'lluvia/core/ImageDescriptor.h' namespace 'll':

    cdef cppclass _ChannelType 'll::ChannelType':
        pass

    cdef cppclass _ChannelCount 'll::ChannelCount':
        pass

    _ChannelCount castChannelCount[T](T c) except +
    uint64_t getChannelTypeSize(_ChannelType type)

    cdef cppclass _ImageDescriptor 'll::ImageDescriptor':

        _ImageDescriptor()
        _ImageDescriptor(const uint32_t width,
                    const uint32_t height,
                    const uint32_t depth,
                    _ChannelCount channelCount,
                    _ChannelType channelType,
                    vk.ImageUsageFlags usageFlags,
                    vk.ImageTiling tiling)


        _ImageDescriptor& setChannelType(const _ChannelType type)
        _ImageDescriptor& setChannelCount(const _ChannelCount count)
        _ImageDescriptor& setWidth(const uint32_t width)
        _ImageDescriptor& setHeight(const uint32_t height)
        _ImageDescriptor& setDepth(const uint32_t depth)

        _ChannelType getChannelType() const
        T getChannelCount[T]()        const
        uint32_t getWidth()           const
        uint32_t getHeight()          const
        uint32_t getDepth()           const
        uint64_t getSize()            const
        vk.ImageType getImageType()   const
        vk.Format getFormat()         const


cdef extern from 'lluvia/core/Image.h' namespace 'll':

    cdef cppclass _Image 'll::Image' (_Object):

        const shared_ptr[_Memory]& getMemory()   const

        uint64_t getSize() const
        _MemoryAllocationInfo getAllocationInfo() const

        vk.ImageUsageFlags getUsageFlags() const
        vk.ImageLayout     getLayout()     const

        _ChannelType getChannelType()    const
        uint64_t getChannelTypeSize()    const
        T getChannelCount[T]()           const
        uint32_t getWidth()              const
        uint32_t getHeight()             const
        uint32_t getDepth()              const

        shared_ptr[_ImageView] createImageView(const _ImageViewDescriptor& descriptor) except +

        void clear() except +


cdef extern from 'lluvia/core/ImageViewDescriptor.h' namespace 'll':

    cdef cppclass _ImageAxis 'll::ImageAxis':
        pass


    cdef cppclass _ImageFilterMode 'll::ImageFilterMode':
        pass


    cdef cppclass _ImageAddressMode 'll::ImageAddressMode':
        pass


    cdef cppclass _ImageViewDescriptor 'll::ImageViewDescriptor':

        _ImageViewDescriptor()

        _ImageViewDescriptor& setFilterMode(_ImageFilterMode filterMode)
        _ImageFilterMode getFilterMode() const

        _ImageViewDescriptor& setAddressMode(_ImageAddressMode addressMode)
        _ImageViewDescriptor& setAddressMode(_ImageAxis axis, _ImageAddressMode addressMode)

        _ImageAddressMode getAddressModeU() const
        _ImageAddressMode getAddressModeV() const
        _ImageAddressMode getAddressModeW() const

        _ImageViewDescriptor& setNormalizedCoordinates(bool normalizedCoordinates)
        bool isNormalizedCoordinates() const

        _ImageViewDescriptor& setIsSampled(bool isSampled)
        bool isSampled() const


cdef extern from 'lluvia/core/ImageView.h' namespace 'll':

    cdef cppclass _ImageView 'll::ImageView':

        const shared_ptr[_Memory]& getMemory()   const
        const shared_ptr[_Image]& getImage()     const

        _ImageViewDescriptor& getDescriptor() const

        uint64_t getSize() const
        _MemoryAllocationInfo getAllocationInfo() const

        vk.ImageUsageFlags getUsageFlags() const
        vk.ImageLayout     getLayout()     const

        _ChannelType getChannelType()    const
        uint64_t getChannelTypeSize()    const
        T getChannelCount[T]()           const
        uint32_t getWidth()              const
        uint32_t getHeight()             const
        uint32_t getDepth()              const

        void clear() except +


cdef _buildImage(shared_ptr[_Image] ptr, Session session, Memory memory)

cdef _buildImageView(shared_ptr[_ImageView] ptr, Session session, Image image)

cdef class Image:
    cdef shared_ptr[_Image] __image
    cdef Session            __session
    cdef Memory             __memory


cdef class ImageView:
    cdef shared_ptr[_ImageView] __imageView
    cdef Session                __session
    cdef Image                  __image
