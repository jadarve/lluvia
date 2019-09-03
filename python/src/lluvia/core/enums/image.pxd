"""
    lluvia.core.enums.image
    -----------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cdef extern from 'lluvia/core/ImageDescriptor.h' namespace 'll':
    
    cdef enum _ChannelType 'll::ChannelType':
        _ChannelType_Uint8   'll::ChannelType::Uint8'
        _ChannelType_Int8    'll::ChannelType::Int8'

        _ChannelType_Uint16  'll::ChannelType::Uint16'
        _ChannelType_Int16   'll::ChannelType::Int16'
        _ChannelType_Float16 'll::ChannelType::Float16'

        _ChannelType_Uint32  'll::ChannelType::Uint32'
        _ChannelType_Int32   'll::ChannelType::Int32'
        _ChannelType_Float32 'll::ChannelType::Float32'

        _ChannelType_Uint64  'll::ChannelType::Uint64'
        _ChannelType_Int64   'll::ChannelType::Int64'
        _ChannelType_Float64 'll::ChannelType::Float64'


    cdef enum _ChannelCount 'll::ChannelCount':
        _ChannelCount_C1 'll::ChannelCount::C1'
        _ChannelCount_C2 'll::ChannelCount::C2'
        _ChannelCount_C3 'll::ChannelCount::C3'
        _ChannelCount_C4 'll::ChannelCount::C4'


cpdef enum ChannelType:
    Uint8   = <uint32_t> _ChannelType_Uint8
    Int8    = <uint32_t> _ChannelType_Int8

    Uint16  = <uint32_t> _ChannelType_Uint16
    Int16   = <uint32_t> _ChannelType_Int16
    Float16 = <uint32_t> _ChannelType_Float16

    Uint32  = <uint32_t> _ChannelType_Uint32
    Int32   = <uint32_t> _ChannelType_Int32
    Float32 = <uint32_t> _ChannelType_Float32

    Uint64  = <uint32_t> _ChannelType_Uint64
    Int64   = <uint32_t> _ChannelType_Int64
    Float64 = <uint32_t> _ChannelType_Float64


cpdef enum ChannelCount:
    C1 = <uint32_t> _ChannelCount_C1
    C2 = <uint32_t> _ChannelCount_C2
    C3 = <uint32_t> _ChannelCount_C3
    C4 = <uint32_t> _ChannelCount_C4
