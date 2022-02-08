"""
    lluvia.core.node
    ----------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport int32_t, uint32_t, uint64_t

from libcpp.memory cimport shared_ptr
from libcpp.vector cimport vector
from libcpp.string cimport string

from lluvia.core.command_buffer cimport _CommandBuffer
from lluvia.core.core_object cimport _Object
from lluvia.core.parameter cimport _Parameter
from lluvia.core.program cimport _Program
from lluvia.core.session cimport Session
from lluvia.core.types cimport _vec3ui

from lluvia.core.node_new.node_type cimport _NodeType
from lluvia.core.node_new.port_direction cimport _PortDirection
from lluvia.core.node_new.port_type cimport _PortType
from lluvia.core.node_new.port_descriptor cimport _PortDescriptor
from lluvia.core.node_new.node cimport _Node
from lluvia.core.node_new.container_node_descriptor cimport _ContainerNodeDescriptor

cdef extern from 'lluvia/core/ContainerNode.h' namespace 'll':

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


cdef class ContainerNodeDescriptor:
    cdef _ContainerNodeDescriptor __descriptor


cdef class ContainerNode:
    cdef shared_ptr[_ContainerNode] __node
    cdef Session                    __session


cdef _buildContainerNode(shared_ptr[_ContainerNode] ptr, Session session)
