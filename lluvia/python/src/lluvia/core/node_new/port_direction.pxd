"""
    lluvia.core.node.port_direction
    -------------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cdef extern from "lluvia/core/Node.h" namespace 'll':

    cdef enum _PortDirection 'll::PortDirection':
        _PortDirection_In  'll::PortDirection::In'
        _PortDirection_Out 'll::PortDirection::Out'

cpdef enum PortDirection:
    In  = <uint32_t> _PortDirection_In
    Out = <uint32_t> _PortDirection_Out
