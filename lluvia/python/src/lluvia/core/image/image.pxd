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

from lluvia.core.image.image_address_mode cimport _ImageAddressMode
from lluvia.core.image.image_axis cimport _ImageAxis
from lluvia.core.image.image_filter_mode cimport _ImageFilterMode
from lluvia.core.image.image_layout cimport _ImageLayout
from lluvia.core.image.image_tiling cimport _ImageTiling
from lluvia.core.image.image_usage_flags cimport _ImageUsageFlags

from lluvia.core.memory.memory cimport _Memory, Memory
from lluvia.core.memory.memory_allocation_info cimport _MemoryAllocationInfo

from lluvia.core cimport vulkan as vk
from lluvia.core.core_object cimport _Object

from lluvia.core.session cimport Session


cdef extern from 'lluvia/core/image/ImageDescriptor.h' namespace 'll':

    cdef cppclass _ChannelType 'll::ChannelType':
        pass

    cdef cppclass _ChannelCount 'll::ChannelCount':
        pass

    cdef enum _ChannelTypeBits 'll::ChannelType':
        _ChannelTypeBits_Uint8   'll::ChannelType::Uint8'
        _ChannelTypeBits_Int8    'll::ChannelType::Int8'

        _ChannelTypeBits_Uint16  'll::ChannelType::Uint16'
        _ChannelTypeBits_Int16   'll::ChannelType::Int16'
        _ChannelTypeBits_Float16 'll::ChannelType::Float16'

        _ChannelTypeBits_Uint32  'll::ChannelType::Uint32'
        _ChannelTypeBits_Int32   'll::ChannelType::Int32'
        _ChannelTypeBits_Float32 'll::ChannelType::Float32'

        _ChannelTypeBits_Uint64  'll::ChannelType::Uint64'
        _ChannelTypeBits_Int64   'll::ChannelType::Int64'
        _ChannelTypeBits_Float64 'll::ChannelType::Float64'


    cdef enum _ChannelCountBits 'll::ChannelCount':
        _ChannelCountBits_C1 'll::ChannelCount::C1'
        _ChannelCountBits_C2 'll::ChannelCount::C2'
        _ChannelCountBits_C3 'll::ChannelCount::C3'
        _ChannelCountBits_C4 'll::ChannelCount::C4'


    _ChannelCount castChannelCount[T](T c) except +
    uint64_t getChannelTypeSize(_ChannelType type)

    cdef cppclass _ImageDescriptor 'll::ImageDescriptor':

        _ImageDescriptor()
        _ImageDescriptor(const uint32_t width,
                    const uint32_t height,
                    const uint32_t depth,
                    _ChannelCount channelCount,
                    _ChannelType channelType,
                    _ImageUsageFlags usageFlags,
                    _ImageTiling tiling)


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


cdef extern from 'lluvia/core/image/Image.h' namespace 'll':

    cdef cppclass _Image 'll::Image' (_Object):

        const shared_ptr[_Memory]& getMemory()   const

        uint64_t getSize() const
        _MemoryAllocationInfo getAllocationInfo() const

        _ImageUsageFlags getUsageFlags() const
        _ImageLayout     getLayout()     const

        _ChannelType getChannelType()    const
        uint64_t getChannelTypeSize()    const
        T getChannelCount[T]()           const
        uint32_t getWidth()              const
        uint32_t getHeight()             const
        uint32_t getDepth()              const

        shared_ptr[_ImageView] createImageView(const _ImageViewDescriptor& descriptor) except +

        void clear() except +


cdef extern from 'lluvia/core/image/ImageViewDescriptor.h' namespace 'll':

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


cdef extern from 'lluvia/core/image/ImageView.h' namespace 'll':

    cdef cppclass _ImageView 'll::ImageView':

        const shared_ptr[_Memory]& getMemory()   const
        const shared_ptr[_Image]& getImage()     const

        _ImageViewDescriptor& getDescriptor() const

        uint64_t getSize() const
        _MemoryAllocationInfo getAllocationInfo() const

        _ImageUsageFlags getUsageFlags() const
        _ImageLayout     getLayout()     const

        _ChannelType getChannelType()    const
        uint64_t getChannelTypeSize()    const
        T getChannelCount[T]()           const
        uint32_t getWidth()              const
        uint32_t getHeight()             const
        uint32_t getDepth()              const

        void clear() except +

cpdef enum ChannelType:
    Uint8   = <uint32_t> _ChannelTypeBits_Uint8
    Int8    = <uint32_t> _ChannelTypeBits_Int8

    Uint16  = <uint32_t> _ChannelTypeBits_Uint16
    Int16   = <uint32_t> _ChannelTypeBits_Int16
    Float16 = <uint32_t> _ChannelTypeBits_Float16

    Uint32  = <uint32_t> _ChannelTypeBits_Uint32
    Int32   = <uint32_t> _ChannelTypeBits_Int32
    Float32 = <uint32_t> _ChannelTypeBits_Float32

    Uint64  = <uint32_t> _ChannelTypeBits_Uint64
    Int64   = <uint32_t> _ChannelTypeBits_Int64
    Float64 = <uint32_t> _ChannelTypeBits_Float64

cpdef enum ChannelCount:
    C1 = <uint32_t> _ChannelCountBits_C1
    C2 = <uint32_t> _ChannelCountBits_C2
    C3 = <uint32_t> _ChannelCountBits_C3
    C4 = <uint32_t> _ChannelCountBits_C4


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
