
"""
    lluvia.core.node.port_descriptor
    --------------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

from libcpp.string cimport string

from lluvia.core import impl

from lluvia.core.node_new.port_direction cimport PortDirection
from lluvia.core.node_new.port_direction import PortDirection as PortDirection_t
from lluvia.core.node_new.port_type cimport PortType
from lluvia.core.node_new.port_type import PortType as PortType_t


__all__ = [
    'PortDescriptor',
]


cdef class PortDescriptor:

    def __init__(self, uint32_t binding, str name, PortDirection direction, PortType type):

            self.__descriptor.binding = binding
            self.__descriptor.name = impl.encodeString(name)
            self.__descriptor.direction = <_PortDirection> direction
            self.__descriptor.type = <_PortType> type

    def __cinit__(self):
        pass

    def __dealloc__(self):
        pass

    property binding:
        def __get__(self):
            return self.__descriptor.binding

    property name:
        def __get__(self):
            return str(self.__descriptor.name, 'utf-8')

    property direction:
        def __get__(self):
            return PortDirection_t(<uint32_t> self.__descriptor.direction)

    property type:
        def __get__(self):
            return PortType_t(<uint32_t> self.__descriptor.type)

    def __str__(self):
        return '{0}:{1:s}:{2}:{3}'.format(self.binding,
                                          self.name,
                                          self.direction.name,
                                          self.type.name)

