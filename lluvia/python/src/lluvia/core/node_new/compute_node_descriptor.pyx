# cython: language_level=3, boundscheck=False, emit_code_comments=True, embedsignature=True

"""
    lluvia.core.compute_node
    ------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

from libcpp.memory cimport shared_ptr
from libcpp.string cimport string

from cython.operator cimport dereference as deref

from lluvia.core import impl

from lluvia.core.node_new.port_direction cimport PortDirection
from lluvia.core.node_new.port_direction import PortDirection as PortDirection_t
from lluvia.core.node_new.port_type cimport PortType
from lluvia.core.node_new.port_type import PortType as PortType_t

from lluvia.core.node_new.port_descriptor cimport PortDescriptor

from lluvia.core.parameter cimport Parameter
from lluvia.core.program cimport Program, _Program


__all__ = [
    'ComputeNodeDescriptor'
]

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

    def getPort(self, str name):

        # FIXME
        cdef PortDescriptor out = PortDescriptor(0, 'dummy', PortDirection.In, PortType.Buffer)
        out.__descriptor = self.__descriptor.getPort(impl.encodeString(name))
        return out

    def setParameter(self, str name, Parameter param):

        self.__descriptor.setParameter(impl.encodeString(name), param.__p)

    def getParameter(self, str name):

        cdef Parameter out = Parameter()
        out.__p = self.__descriptor.getParameter(impl.encodeString(name))
        return out
