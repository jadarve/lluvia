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

from object cimport _Object

cimport vulkan as vk


cdef extern from 'lluvia/core/ImageDescriptor.h' namespace 'll':
    
    cdef cppclass _ChannelType 'll::ChannelType':
        pass


    uint64_t getChannelTypeSize(_ChannelType type)
    string channelTypeToString(_ChannelType&& value)
    _ChannelType stringToChannelType(string&& stringValue)


    cdef cppclass _ImageDescriptor 'll::ImageDescriptor':

        _ImageDescriptor()
        _ImageDescriptor(const uint32_t width,
                    const uint32_t height,
                    const uint32_t depth,
                    const uint32_t channelCount,
                    _ChannelType channelType)


        _ImageDescriptor& setChannelType(const _ChannelType type)
        _ImageDescriptor& setChannelCount(const uint32_t count)
        _ImageDescriptor& setWidth(const uint32_t width)
        _ImageDescriptor& setHeight(const uint32_t height)
        _ImageDescriptor& setDepth(const uint32_t depth)

        _ChannelType getChannelType() const
        uint32_t getChannelCount()    const
        uint32_t getWidth()           const
        uint32_t getHeight()          const
        uint32_t getDepth()           const
        uint64_t getSize()            const
        vk.ImageType getImageType()   const
        vk.Format getFormat()         const



cdef extern from 'lluvia/core/Image.h' namespace 'll':

    vk.ImageUsageFlags vectorStringToImageUsageFlags(const vector[string]& flagsVector)
    vector[string] imageUsageFlagsToVectorString(const vk.ImageUsageFlags flags)

    cdef cppclass _Image 'll::Image' (_Object):

        uint64_t getSize() const

        vk.ImageUsageFlags getUsageFlags() const
        vk.ImageLayout     getLayout()     const

        _ChannelType getChannelType() const
        uint64_t getChannelTypeSize()    const
        uint32_t getChannelCount()       const
        uint32_t getWidth()              const
        uint32_t getHeight()             const
        uint32_t getDepth()              const


cdef class Image:
    
    cdef shared_ptr[_Image] __image
