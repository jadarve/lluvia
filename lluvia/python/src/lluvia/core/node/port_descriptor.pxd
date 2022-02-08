"""
    lluvia.core.node.port_descriptor
    --------------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t
from libcpp.string cimport string

from lluvia.core.node.port_direction cimport _PortDirection
from lluvia.core.node.port_type cimport _PortType

cdef extern from 'lluvia/core/node/PortDescriptor.h' namespace 'll':

    cdef struct _PortDescriptor 'll::PortDescriptor':
        uint32_t binding
        string name
        _PortDirection direction
        _PortType type


cdef class PortDescriptor:
    cdef _PortDescriptor __descriptor
