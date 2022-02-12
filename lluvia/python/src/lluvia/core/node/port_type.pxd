"""
    lluvia.core.node.port_type
    --------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

cdef extern from "lluvia/core/node/PortType.h" namespace 'll':

    cdef enum _PortType 'll::PortType':
        _PortType_Buffer           'll::PortType::Buffer'
        _PortType_ImageView        'll::PortType::ImageView'
        _PortType_SampledImageView 'll::PortType::SampledImageView'


cpdef enum PortType:
    Buffer           = <uint32_t> _PortType_Buffer
    ImageView        = <uint32_t> _PortType_ImageView
    SampledImageView = <uint32_t> _PortType_SampledImageView
