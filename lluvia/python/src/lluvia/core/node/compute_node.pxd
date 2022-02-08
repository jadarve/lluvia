"""
    lluvia.core.node.compute_node
    -----------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

from libcpp.memory cimport shared_ptr
from libcpp.string cimport string

from lluvia.core.command_buffer cimport _CommandBuffer
from lluvia.core.core_object cimport _Object
from lluvia.core.program cimport _Program
from lluvia.core.session cimport Session
from lluvia.core.types cimport _vec3ui

from lluvia.core.node.compute_node_descriptor cimport _ComputeNodeDescriptor
from lluvia.core.node.node_type cimport _NodeType
from lluvia.core.node.parameter cimport _Parameter
from lluvia.core.node.push_constants cimport _PushConstants

cdef extern from 'lluvia/core/node/ComputeNode.h' namespace 'll':

    cdef cppclass _ComputeNode 'll::ComputeNode':

        _NodeType getType() const

        string getFunctionName() const
        shared_ptr[_Program] getProgram() const

        const _ComputeNodeDescriptor& getDescriptor() const

        uint32_t getGridX() const
        uint32_t getGridY() const
        uint32_t getGridZ() const

        void setGridX(uint32_t x)
        void setGridY(uint32_t y)
        void setGridZ(uint32_t z)

        uint32_t getLocalX() const
        uint32_t getLocalY() const
        uint32_t getLocalZ() const

        void setPushConstants(const _PushConstants&)
        const _PushConstants getPushConstants() const

        void setParameter(const string& name, const _Parameter& value)
        const _Parameter& getParameter(const string& name) except +

        void configureGridShape(const _vec3ui& globalShape)

        shared_ptr[_Object] getPort(const string& name) except +
        void bind(const string& name, const shared_ptr[_Object]& obj) except +

        void init() except +
        void record(_CommandBuffer& commandBuffer) except +


cdef class ComputeNode:
    cdef shared_ptr[_ComputeNode] __node
    cdef Session                  __session


cdef _buildComputeNode(shared_ptr[_ComputeNode] ptr, Session session)
