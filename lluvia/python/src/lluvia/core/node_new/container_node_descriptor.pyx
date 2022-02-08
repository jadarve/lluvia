# cython: language_level=3, boundscheck=False, emit_code_comments=True, embedsignature=True

"""
    lluvia.core.compute_node
    ------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from lluvia.core import impl

from lluvia.core.node_new.port_descriptor cimport PortDescriptor

from lluvia.core.parameter cimport Parameter


__all__ = [
    'ContainerNodeDescriptor',
]

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

    def setParameter(self, str name, Parameter param):

        self.__descriptor.setParameter(impl.encodeString(name), param.__p)

    def getParameter(self, str name):

        cdef Parameter out = Parameter()
        out.__p = self.__descriptor.getParameter(impl.encodeString(name))
        return out

