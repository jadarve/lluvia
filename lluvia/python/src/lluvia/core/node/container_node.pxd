"""
    lluvia.core.node.container_node
    -------------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libcpp.memory cimport shared_ptr
from libcpp.string cimport string

from lluvia.core.command_buffer cimport _CommandBuffer
from lluvia.core.core_object cimport _Object
from lluvia.core.parameter cimport _Parameter
from lluvia.core.session cimport Session

from lluvia.core.node.node_type cimport _NodeType
from lluvia.core.node.node cimport _Node
from lluvia.core.node.container_node_descriptor cimport _ContainerNodeDescriptor

cdef extern from 'lluvia/core/node/ContainerNode.h' namespace 'll':

    cdef cppclass _ContainerNode 'll::ContainerNode':

        _NodeType getType() const

        const _ContainerNodeDescriptor& getDescriptor() const

        shared_ptr[_Object] getPort(const string& name) except +
        void bind(const string& name, const shared_ptr[_Object]& obj) except +

        shared_ptr[_Node] getNode(const string& name) except +
        void bindNode(const string& name, const shared_ptr[_Node]& obj) except +

        void setParameter(const string& name, const _Parameter& value)
        const _Parameter& getParameter(const string& name) except +

        void init() except +
        void record(_CommandBuffer& commandBuffer) except +


cdef class ContainerNode:
    cdef shared_ptr[_ContainerNode] __node
    cdef Session                    __session


cdef _buildContainerNode(shared_ptr[_ContainerNode] ptr, Session session)
