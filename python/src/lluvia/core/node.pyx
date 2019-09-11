"""
    lluvia.core.compute_node
    ------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from .enums.node cimport NodeState, NodeType, PortDirection, PortType
from .enums.node import PortDirection as PortDirection_t
from .enums.node import PortType as PortType_t

from core_buffer import Buffer
from core_buffer cimport Buffer

from command_buffer import CommandBuffer
from command_buffer cimport CommandBuffer

from image import ImageView
from image cimport ImageView

from program import Program
from program cimport Program, _Program

from . import impl
from .impl.stdcpp cimport static_pointer_cast

import sys

from libc.stdint cimport uint32_t

from libcpp.memory cimport shared_ptr
from libcpp.string cimport string

from cython.operator cimport dereference as deref


__all__ = [
    'PortDescriptor',
    'ComputeNodeDescriptor',
    'ComputeNode',
    'ContainerNodeDescriptor',
    'ContainerNode'
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


cdef class ComputeNodeDescriptor:

    def __cinit__(self):
        self.__descriptor = _ComputeNodeDescriptor()

    def __dealloc__(self):
        pass

    property program:
        def __get__(self):

            cdef Program prog = Program()
            prog.__program = self.__descriptor.getProgram()
            return prog

        def __set__(self, Program p):
            self.__descriptor.setProgram(p.__program)

    property functionName:
        def __get__(self):
            return str(self.__descriptor.getFunctionName(), 'utf-8')

        def __set__(self, str functionName):
            self.__descriptor.setFunctionName(impl.encodeString(functionName))

    property builderName:
        def __get__(self):
            return str(self.__descriptor.getBuilderName(), 'utf-8')

        def __set__(self, str builderName):
            self.__descriptor.setBuilderName(impl.encodeString(builderName))

    property grid:
        def __get__(self):
            return (self.gridX, self.gridY, self.gridZ)

        def __set__(self, v):
            assert (len(v) == 3)
            self.gridX, self.gridY, self.gridZ = v

    property local:
        def __get__(self):
            return (self.localX, self.localY, self.localZ)

        def __set__(self, v):
            assert (len(v) == 3)
            self.localX, self.localY, self.localZ = v

    property gridX:
        def __get__(self):
            return self.__descriptor.getGridX()

        def __set__(self, uint32_t x):
            self.__descriptor.setGridX(x)

    property gridY:
        def __get__(self):
            return self.__descriptor.getGridY()

        def __set__(self, uint32_t y):
            self.__descriptor.setGridY(y)

    property gridZ:
        def __get__(self):
            return self.__descriptor.getGridZ()

        def __set__(self, uint32_t z):
            self.__descriptor.setGridZ(z)

    property localX:
        def __get__(self):
            return self.__descriptor.getLocalX()

        def __set__(self, uint32_t x):
            self.__descriptor.setLocalX(x)

    property localY:
        def __get__(self):
            return self.__descriptor.getLocalY()

        def __set__(self, uint32_t y):
            self.__descriptor.setLocalY(y)

    property localZ:
        def __get__(self):
            return self.__descriptor.getLocalZ()

        def __set__(self, uint32_t z):
            self.__descriptor.setLocalZ(z)

    def addPort(self, PortDescriptor portDesc):

        self.__descriptor.addPort(portDesc.__descriptor)


cdef class ComputeNode:

    def __cinit__(self):
        pass

    def __dealloc__(self):
        pass

    property grid:
        def __get__(self):
            return (self.gridX, self.gridY, self.gridZ)

        def __set__(self, v):
            assert (len(v) == 3)
            self.gridX, self.gridY, self.gridZ = v

    property local:
        def __get__(self):
            return (self.localX, self.localY, self.localZ)

    property gridX:
        def __get__(self):
            return self.__node.get().getGridX()

        def __set__(self, uint32_t x):
            self.__node.get().setGridX(x)

    property gridY:
        def __get__(self):
            return self.__node.get().getGridY()

        def __set__(self, uint32_t y):
            self.__node.get().setGridY(y)

    property gridZ:
        def __get__(self):
            return self.__node.get().getGridZ()

    property localX:
        def __get__(self):
            return self.__node.get().getLocalX()

    property localY:
        def __get__(self):
            return self.__node.get().getLocalY()

    property localZ:
        def __get__(self):
            return self.__node.get().getLocalZ()

    def bind(self, str name, obj):
        """
        Binds an object as parater to this node.


        Parameters
        ----------
        index : str
            Name of the object to bind

        obj : lluvia.Buffer or lluvia.ImageView
            Parameter to bind.
        """

        cdef Buffer    buf     = None
        cdef ImageView imgView = None

        if type(obj) == Buffer:
            buf = obj
            self.__node.get().bind(impl.encodeString(name),
                                   static_pointer_cast[_Object](buf.__buffer))

        if type(obj) == ImageView:
            imgView = obj
            self.__node.get().bind(impl.encodeString(name),
                                   static_pointer_cast[_Object](imgView.__imageView))

    def init(self):

        self.__node.get().init()

    def run(self):
        """
        Runs this node
        """

        self.__session.run(self)

    def record(self, CommandBuffer cmdBuffer):
        self.__node.get().record(deref(cmdBuffer.__commandBuffer.get()))


cdef class ContainerNodeDescriptor:

    def __cinit__(self):
        self.__descriptor = _ContainerNodeDescriptor()

    def __dealloc__(self):
        pass

    property builderName:
        def __get__(self):
            return str(self.__descriptor.getBuilderName(), 'utf-8')

        def __set__(self, str builderName):
            self.__descriptor.setBuilderName(impl.encodeString(builderName))

    def addPort(self, PortDescriptor portDesc):

        self.__descriptor.addPort(portDesc.__descriptor)


cdef class ContainerNode:

    def __cinit__(self):
        pass

    def __dealloc__(self):
        pass

    def bind(self, str name, obj):
        """
        Binds an object as parater to this node.


        Parameters
        ----------
        index : str
            Name of the object to bind

        obj : lluvia.Buffer or lluvia.ImageView
            Parameter to bind.
        """

        cdef Buffer    buf     = None
        cdef ImageView imgView = None

        if type(obj) == Buffer:
            buf = obj
            self.__node.get().bind(impl.encodeString(name),
                                   static_pointer_cast[_Object](buf.__buffer))

        if type(obj) == ImageView:
            imgView = obj
            self.__node.get().bind(impl.encodeString(name),
                                   static_pointer_cast[_Object](imgView.__imageView))

    def init(self):

        self.__node.get().init()

    def run(self):
        """
        Runs this node
        """

        self.__session.run(self)

    def record(self, CommandBuffer cmdBuffer):
        self.__node.get().record(deref(cmdBuffer.__commandBuffer.get()))