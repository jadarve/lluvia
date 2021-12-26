"""
    lluvia.core.image.image_usage_flags
    -----------------------------------

    :copyright: 2021, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

cdef extern from 'lluvia/core/image/ImageUsageFlags.h' namespace 'll':

    cdef cppclass _ImageUsageFlags 'll::ImageUsageFlags':
        pass
    
    cdef enum _ImageUsageFlagBits 'll::ImageUsageFlagBits':
        _ImageUsageFlagBits_TransferSrc  'll::ImageUsageFlagBits::TransferSrc'
        _ImageUsageFlagBits_TransferDst  'll::ImageUsageFlagBits::TransferDst'
        _ImageUsageFlagBits_Sampled      'll::ImageUsageFlagBits::Sampled'
        _ImageUsageFlagBits_Storage      'll::ImageUsageFlagBits::Storage'


cpdef enum ImageUsageFlagBits:
    TransferSrc = <uint32_t> _ImageUsageFlagBits_TransferSrc
    TransferDst = <uint32_t> _ImageUsageFlagBits_TransferDst
    Sampled     = <uint32_t> _ImageUsageFlagBits_Sampled  
    Storage     = <uint32_t> _ImageUsageFlagBits_Storage
