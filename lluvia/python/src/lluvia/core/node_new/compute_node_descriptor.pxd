"""
    lluvia.core.node.compute_node_descriptor
    ----------------------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

from libcpp.memory cimport shared_ptr
from libcpp.string cimport string

from lluvia.core.parameter cimport _Parameter
from lluvia.core.program cimport _Program
from lluvia.core.node cimport _PushConstants

from lluvia.core.node_new.port_descriptor cimport _PortDescriptor


cdef extern from 'lluvia/core/node/ComputeNodeDescriptor.h' namespace 'll':

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

        _ComputeNodeDescriptor& setPushConstants(const _PushConstants&)
        const _PushConstants getPushConstants() const

        _ComputeNodeDescriptor& setParameter(const string& name, const _Parameter& value)
        _Parameter getParameter(const string& name) except +

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


cdef class ComputeNodeDescriptor:
    cdef _ComputeNodeDescriptor __descriptor
