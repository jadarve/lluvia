"""
    lluvia.core.compute_node
    ------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

from libcpp.memory cimport shared_ptr
from libcpp.vector cimport vector
from libcpp.string cimport string

from object cimport _Object
from program cimport _Program
from session cimport Session


cdef extern from 'lluvia/core/ComputeNodeDescriptor.h' namespace 'll':

    cdef cppclass _ParameterType 'll::ParameterType':
        pass


    _ParameterType stringToParameterType(string&& stringValue);


    cdef cppclass _ComputeNodeDescriptor 'll::ComputeNodeDescriptor':

        _ComputeNodeDescriptor()

        _ComputeNodeDescriptor& setProgram(const shared_ptr[_Program]& program)
        _ComputeNodeDescriptor& setFunctionName(const string& name)

        _ComputeNodeDescriptor& addParameter(const _ParameterType param)

        _ComputeNodeDescriptor& setGridX(const uint32_t x)
        _ComputeNodeDescriptor& setGridY(const uint32_t y)
        _ComputeNodeDescriptor& setGridZ(const uint32_t z)

        _ComputeNodeDescriptor& setLocalX(const uint32_t x)
        _ComputeNodeDescriptor& setLocalY(const uint32_t y)
        _ComputeNodeDescriptor& setLocalZ(const uint32_t z)

        shared_ptr[_Program] getProgram() const

        string getFunctionName() const

        uint32_t getGridX() const
        uint32_t getGridY() const
        uint32_t getGridZ() const

        uint32_t getLocalX() const
        uint32_t getLocalY() const
        uint32_t getLocalZ() const


cdef extern from 'lluvia/core/ComputeNode.h' namespace 'll':
    
    cdef cppclass _ComputeNode 'll::ComputeNode':

        uint32_t getGridX() const
        uint32_t getGridY() const
        uint32_t getGridZ() const

        void setGridX(uint32_t x)
        void setGridY(uint32_t y)
        void setGridZ(uint32_t z)

        uint32_t getLocalX() const
        uint32_t getLocalY() const
        uint32_t getLocalZ() const

        void bind(uint32_t index, const shared_ptr[_Object]& obj) except +
        

cdef class ComputeNodeDescriptor:
    
    cdef _ComputeNodeDescriptor __descriptor


cdef class ComputeNode:
    
    cdef Session                  __session
    cdef shared_ptr[_ComputeNode] __node
