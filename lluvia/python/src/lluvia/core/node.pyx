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

from lluvia.core.node_new.node_state cimport NodeState
from lluvia.core.node_new.node_type cimport NodeType
from lluvia.core.node_new.node_type import NodeType as NodeType_t
from lluvia.core.node_new.port_direction cimport PortDirection
from lluvia.core.node_new.port_direction import PortDirection as PortDirection_t
from lluvia.core.node_new.port_type cimport PortType
from lluvia.core.node_new.port_type import PortType as PortType_t
from lluvia.core.node_new.port_descriptor cimport PortDescriptor
from lluvia.core.node_new.compute_node cimport _ComputeNode, _buildComputeNode

from lluvia.core.image.image cimport Image, ImageView, _ImageView, _buildImageView
from lluvia.core.impl.stdcpp cimport static_pointer_cast
from lluvia.core.memory.memory cimport Memory, _Memory
from lluvia.core.parameter cimport Parameter
from lluvia.core.program cimport Program, _Program
from lluvia.core.session cimport Session


__all__ = [
    'ContainerNode'
]

cdef _buildContainerNode(shared_ptr[_ContainerNode] ptr, Session session):

    cdef ContainerNode node = ContainerNode()
    node.__node = ptr
    node.__session = session

    return node

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
