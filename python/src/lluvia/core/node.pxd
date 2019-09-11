"""
    lluvia.core.node
    ----------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from command_buffer cimport _CommandBuffer
from core_object cimport _Object
from program cimport _Program
from session cimport Session
from types cimport _vec3ui

from libc.stdint cimport uint32_t

from libcpp.memory cimport shared_ptr
from libcpp.vector cimport vector
from libcpp.string cimport string


cdef extern from 'lluvia/core/Node.h' namespace 'll':

    cdef cppclass _NodeState 'll::NodeState':
        pass


    cdef cppclass _NodeType 'll::NodeType':
        pass


    cdef cppclass _PortDirection 'll::PortDirection':
        pass


    cdef cppclass _PortType 'll::PortType':
        pass


    cdef struct _PortDescriptor 'll::PortDescriptor':
        uint32_t binding
        string name
        _PortDirection direction
        _PortType type


    cdef cppclass _Node 'll::Node':
        pass


cdef extern from 'lluvia/core/ComputeNodeDescriptor.h' namespace 'll':

    cdef cppclass _ComputeNodeDescriptor 'll::ComputeNodeDescriptor':

        _ComputeNodeDescriptor()

        _ComputeNodeDescriptor& setProgram(const shared_ptr[_Program]& program)
        shared_ptr[_Program] getProgram() const

        _ComputeNodeDescriptor& setFunctionName(const string& name)
        string getFunctionName() const

        _ComputeNodeDescriptor& setBuilderName(const string& name)
        string getBuilderName() const

        _ComputeNodeDescriptor& addPort(_PortDescriptor& port)
        _PortDescriptor getPort(const string& name) except +

        # _ComputeNodeDescriptor& addParameter(const string& name, const _Parameter& value)
        # _Parameter getParameter(const string& name) except +

        _ComputeNodeDescriptor& setGridX(const uint32_t x)
        _ComputeNodeDescriptor& setGridY(const uint32_t y)
        _ComputeNodeDescriptor& setGridZ(const uint32_t z)

        _ComputeNodeDescriptor& setLocalX(const uint32_t x)
        _ComputeNodeDescriptor& setLocalY(const uint32_t y)
        _ComputeNodeDescriptor& setLocalZ(const uint32_t z)

        uint32_t getGridX() const
        uint32_t getGridY() const
        uint32_t getGridZ() const

        uint32_t getLocalX() const
        uint32_t getLocalY() const
        uint32_t getLocalZ() const


cdef extern from 'lluvia/core/ComputeNode.h' namespace 'll':
    
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

        void configureGridShape(const _vec3ui& globalShape)

        shared_ptr[_Object] getPort(const string& name) except +
        void bind(const string& name, const shared_ptr[_Object]& obj) except +

        void init()
        void record(_CommandBuffer& commandBuffer) const


cdef extern from 'lluvia/core/ContainerNodeDescriptor.h' namespace 'll':

    cdef cppclass _ContainerNodeDescriptor 'll::ContainerNodeDescriptor':

        _ContainerNodeDescriptor()

        _ContainerNodeDescriptor& setBuilderName(const string& name)
        string getBuilderName() const

        _ContainerNodeDescriptor& addPort(_PortDescriptor& port)
        _PortDescriptor getPort(const string& name) except +

        # _ContainerNodeDescriptor& addParameter(const string& name, const _Parameter& value)
        # _Parameter getParameter(const string& name) except +


cdef extern from 'lluvia/core/ContainerNode.h' namespace 'll':
    
    cdef cppclass _ContainerNode 'll::ContainerNode':

        _NodeType getType() const

        const _ContainerNodeDescriptor& getDescriptor() const

        shared_ptr[_Object] getPort(const string& name) except +
        void bind(const string& name, const shared_ptr[_Object]& obj) except +

        shared_ptr[_Node] getNode(const string& name) except +
        void bindNode(const string& name, const shared_ptr[_Node]& obj) except +

        void init()
        void record(_CommandBuffer& commandBuffer) const


cdef class PortDescriptor:
    
    cdef _PortDescriptor __descriptor


cdef class ComputeNodeDescriptor:
    
    cdef _ComputeNodeDescriptor __descriptor


cdef class ComputeNode:
    
    cdef Session                  __session
    cdef shared_ptr[_ComputeNode] __node


cdef class ContainerNodeDescriptor:
    
    cdef _ContainerNodeDescriptor __descriptor


cdef class ContainerNode:
    
    cdef Session                    __session
    cdef shared_ptr[_ContainerNode] __node
