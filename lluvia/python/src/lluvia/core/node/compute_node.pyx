# cython: language_level=3, boundscheck=False, emit_code_comments=True, embedsignature=True

"""
    lluvia.core.node.compute_node
    -----------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

from libcpp.memory cimport shared_ptr

from cython.operator cimport dereference as deref

from lluvia.core import impl
from lluvia.core.command_buffer cimport CommandBuffer
from lluvia.core.buffer.buffer cimport Buffer, _Buffer, _buildBuffer
from lluvia.core.core_object cimport _Object

from lluvia.core.enums.core_object import ObjectType

from lluvia.core.image.image cimport Image, ImageView, _ImageView, _buildImageView
from lluvia.core.impl.stdcpp cimport static_pointer_cast
from lluvia.core.session cimport Session

from lluvia.core.node.node_type cimport NodeType as NodeType_t
from lluvia.core.node.parameter cimport Parameter

__all__ = [
    'ComputeNode',
]

cdef _buildComputeNode(shared_ptr[_ComputeNode] ptr, Session session):

    cdef ComputeNode node = ComputeNode()
    node.__node = ptr
    node.__session = session

    return node

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
