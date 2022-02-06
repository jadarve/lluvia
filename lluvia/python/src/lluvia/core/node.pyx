# cython: language_level=3, boundscheck=False, emit_code_comments=True, embedsignature=True

"""
    lluvia.core.compute_node
    ------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

import sys

from libc.stdint cimport uint32_t

from libcpp.memory cimport shared_ptr
from libcpp.string cimport string

from cython.operator cimport dereference as deref

from lluvia.core import impl
from lluvia.core.command_buffer cimport CommandBuffer
from lluvia.core.buffer.buffer cimport Buffer, _Buffer, _buildBuffer
from lluvia.core.core_object cimport _Object

from lluvia.core.enums.core_object import ObjectType
from lluvia.core.enums.node cimport PortDirection, PortType
from lluvia.core.enums.node import PortDirection as PortDirection_t
from lluvia.core.enums.node import PortType as PortType_t

from lluvia.core.node_new.node_state cimport NodeState
from lluvia.core.node_new.node_type cimport NodeType
from lluvia.core.node_new.node_type import NodeType as NodeType_t

from lluvia.core.image.image cimport Image, ImageView, _ImageView, _buildImageView
from lluvia.core.impl.stdcpp cimport static_pointer_cast
from lluvia.core.memory.memory cimport Memory, _Memory
from lluvia.core.parameter cimport Parameter
from lluvia.core.program cimport Program, _Program
from lluvia.core.session cimport Session


__all__ = [
    'PortDescriptor',
    'ComputeNodeDescriptor',
    'ComputeNode',
    'ContainerNodeDescriptor',
    'ContainerNode'
]

cdef _buildComputeNode(shared_ptr[_ComputeNode] ptr, Session session):

    cdef ComputeNode node = ComputeNode()
    node.__node = ptr
    node.__session = session

    return node


cdef _buildContainerNode(shared_ptr[_ContainerNode] ptr, Session session):

    cdef ContainerNode node = ContainerNode()
    node.__node = ptr
    node.__session = session

    return node


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

cdef class NodeBuilderDescriptor:

    def __init__(self, NodeType nodeType, str name, str summary):

        self.__descriptor.nodeType = <_NodeType> nodeType
        self.__descriptor.name = impl.encodeString(name)
        self.__descriptor.summary = impl.encodeString(summary)

    property nodeType:
        def __get__(self):
            return NodeType_t(<uint32_t> self.__descriptor.nodeType)

    property name:
        def __get__(self):
            return str(self.__descriptor.name, 'utf-8')

    property summary:
        def __get__(self):
            return str(self.__descriptor.summary, 'utf-8')
    
    def __str__(self):
        return '{0:40s} : {1:9s} : {2:s}'.format(self.name, self.nodeType.name, self.summary)

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

        cdef PortDescriptor out = PortDescriptor(0, 'dummy', PortDirection.In, PortType.Buffer)
        out.__descriptor = self.__descriptor.getPort(impl.encodeString(name))
        return out

    def setParameter(self, str name, Parameter param):

        self.__descriptor.setParameter(impl.encodeString(name), param.__p)

    def getParameter(self, str name):

        cdef Parameter out = Parameter()
        out.__p = self.__descriptor.getParameter(impl.encodeString(name))
        return out


cdef class ComputeNode:

    def __cinit__(self):
        self.__session = None

    def __dealloc__(self):
        pass

    property type:
        def __get__(self):
            return NodeType_t(<uint32_t> self.__node.get().getType())

    property session:
        def __get__(self):
            return self.__session

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

    def setParameter(self, str name, Parameter param):

        self.__node.get().setParameter(impl.encodeString(name), param.__p)

    def getParameter(self, str name):

        cdef Parameter out = Parameter()
        out.__p = self.__node.get().getParameter(impl.encodeString(name))
        return out

    def bind(self, str name, obj):
        """
        Binds an object as parameter to this node.


        Parameters
        ----------
        index : str
            Name of the object to bind

        obj : lluvia.Buffer or lluvia.ImageView
            Parameter to bind.
        """

        cdef Buffer    buf     = None
        cdef ImageView imgView = None

        objType = type(obj)

        if objType == Buffer:
            buf = obj
            self.__node.get().bind(impl.encodeString(name),
                                   static_pointer_cast[_Object](buf.__buffer))
        
        elif objType == ImageView:
            imgView = obj
            self.__node.get().bind(impl.encodeString(name),
                                   static_pointer_cast[_Object](imgView.__imageView))
            
        else:
            raise RuntimeError('Unsupported obj type {0}. Valid types are ll.Buffer and ll.ImageView.'.format(type(obj)))

    def getPort(self, str name):

        cdef shared_ptr[_Object] obj = self.__node.get().getPort(impl.encodeString(name))

        oType = ObjectType(<uint32_t> obj.get().getType())

        # cdef Buffer bufObj = Buffer(self.session, None)
        if oType == ObjectType.Buffer:
            return _buildBuffer(static_pointer_cast[_Buffer](obj),
                                self.session, None)

        if oType == ObjectType.ImageView:
            return _buildImageView(static_pointer_cast[_ImageView](obj),
                                   self.session, None)

        raise RuntimeError('Unsupported object type {0}'.format(oType))

    def init(self):
        """
        Init the node
        """

        self.__node.get().init()

    def run(self):
        """
        Runs this node
        """

        self.session.run(self)

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

    def setParameter(self, str name, Parameter param):

        self.__descriptor.setParameter(impl.encodeString(name), param.__p)

    def getParameter(self, str name):

        cdef Parameter out = Parameter()
        out.__p = self.__descriptor.getParameter(impl.encodeString(name))
        return out


cdef class ContainerNode:

    def __cinit__(self):
        self.__session = None

    def __dealloc__(self):
        pass

    property type:
        def __get__(self):
            return NodeType_t(<uint32_t> self.__node.get().getType())

    property session:
        def __get__(self):
            return self.__session

    def setParameter(self, str name, Parameter param):

        self.__node.get().setParameter(impl.encodeString(name), param.__p)

    def getParameter(self, str name):

        cdef Parameter out = Parameter()
        out.__p = self.__node.get().getParameter(impl.encodeString(name))
        return out

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

    def getPort(self, str name):

        cdef shared_ptr[_Object] obj = self.__node.get().getPort(impl.encodeString(name))
        oType = ObjectType(<uint32_t> obj.get().getType())

        if oType == ObjectType.Buffer:
            return _buildBuffer(static_pointer_cast[_Buffer](obj),
                                self.session, None)

        if oType == ObjectType.ImageView:
            return _buildImageView(static_pointer_cast[_ImageView](obj),
                                   self.session, None)

        raise RuntimeError('Unsupported object type {0}'.format(oType))

    def getNode(self, str name):

        cdef shared_ptr[_Node] node = self.__node.get().getNode(impl.encodeString(name))
        cdef NodeType nType = NodeType_t(<uint32_t> node.get().getType())

        if nType == NodeType.Compute:
            return _buildComputeNode(static_pointer_cast[_ComputeNode](node), self.session)

        if nType == NodeType.Container:
            return _buildContainerNode(static_pointer_cast[_ContainerNode](node), self.session)

        raise RuntimeError('Unsupported node type {0}'.format(nType))

    def bindNode(self, node):
        pass

    def init(self):
        """
        Init the node.
        """

        self.__node.get().init()

    def run(self):
        """
        Runs this node
        """

        self.session.run(self)

    def record(self, CommandBuffer cmdBuffer):
        self.__node.get().record(deref(cmdBuffer.__commandBuffer.get()))
