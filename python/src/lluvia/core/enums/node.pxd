"""
    lluvia.core.enums.node
    ----------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cdef extern from "lluvia/core/Node.h" namespace 'll::NodeState':

    cdef enum _NodeState 'll::NodeState':
        _NodeState_Created 'll::NodeState::Created'
        _NodeState_Init    'll::NodeState::Init'

    cdef enum _NodeType 'll::NodeType':
        _NodeType_Compute    'll::NodeType::Compute'
        _NodeType_Container  'll::NodeType::Container'

    cdef enum _PortDirection 'll::PortDirection':
        _PortDirection_In  'll::PortDirection::In'
        _PortDirection_Out 'll::PortDirection::Out'

    cdef enum _PortType 'll::PortType':
        _PortType_Buffer           'll::PortType::Buffer'
        _PortType_ImageView        'll::PortType::ImageView'
        _PortType_SampledImageView 'll::PortType::SampledImageView'


cpdef enum NodeState:
    Created = <uint32_t> _NodeState_Created
    Init    = <uint32_t> _NodeState_Init


cpdef enum NodeType:
    Compute   = <uint32_t> _NodeType_Compute
    Container = <uint32_t> _NodeType_Container


cpdef enum PortDirection:
    In  = <uint32_t> _PortDirection_In
    Out = <uint32_t> _PortDirection_Out


cpdef enum PortType:
    Buffer           = <uint32_t> _PortType_Buffer
    ImageView        = <uint32_t> _PortType_ImageView
    SampledImageView = <uint32_t> _PortType_SampledImageView
