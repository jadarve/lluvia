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

    cdef cppclass _PortDescriptor 'll::PortDescriptor':

        _PortDescriptor()
        _PortDescriptor(uint32_t,  string&, _PortDirection, _PortType) except +

        uint32_t getBinding() const
        const string& getName() const
        _PortDirection getDirection() const
        _PortType getPortType() const


cdef class PortDescriptor:
    cdef _PortDescriptor __descriptor
