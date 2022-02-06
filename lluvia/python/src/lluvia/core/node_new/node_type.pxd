"""
    lluvia.core.node
    ----------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

cdef extern from 'lluvia/core/node/NodeType.h' namespace 'll':

    cdef enum _NodeType 'll::NodeType':
        _NodeType_Compute    'll::NodeType::Compute'
        _NodeType_Container  'll::NodeType::Container'

cpdef enum NodeType:
    Compute   = <uint32_t> _NodeType_Compute
    Container = <uint32_t> _NodeType_Container
