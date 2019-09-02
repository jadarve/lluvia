"""
    lluvia.core.node
    ----------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cdef extern from "lluvia/core/Node.h" namespace 'll::NodeState':

    cdef enum _NodeState 'll::NodeState':
        _Created 'll::NodeState::Created'
        _Init    'll::NodeState::Init'

    cdef enum _NodeType 'll::NodeType':
        _Compute    'll::NodeType::Compute'
        _Container  'll::NodeType::Container'

    cdef enum _PortDirection 'll::PortDirection':
        _In  'll::PortDirection::In'
        _Out 'll::PortDirection::Out'

    cdef enum _PortType 'll::PortType':
        _Buffer           'll::PortType::Buffer'
        _ImageView        'll::PortType::ImageView'
        _SampledImageView 'll::PortType::SampledImageView'


cpdef enum NodeState:
    Created = <uint32_t> _Created
    Init    = <uint32_t> _Init


cpdef enum NodeType:
    Compute   = <uint32_t> _Compute
    Container = <uint32_t> _Container


cpdef enum PortDirection:
    In  = <uint32_t> _In
    Out = <uint32_t> _Out


cpdef enum PortType:
    Buffer           = <uint32_t> _Buffer
    ImageView        = <uint32_t> _ImageView
    SampledImageView = <uint32_t> _SampledImageView
