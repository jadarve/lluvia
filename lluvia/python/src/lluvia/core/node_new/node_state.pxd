"""
    lluvia.core.node.node_state
    ---------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

cdef extern from 'lluvia/core/node/NodeState.h' namespace 'll':

    cdef enum _NodeState 'll::NodeState':
        _NodeState_Created 'll::NodeState::Created'
        _NodeState_Init    'll::NodeState::Init'


cpdef enum NodeState:
    Created = <uint32_t> _NodeState_Created
    Init    = <uint32_t> _NodeState_Init
