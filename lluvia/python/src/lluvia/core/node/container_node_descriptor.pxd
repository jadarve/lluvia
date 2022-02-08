"""
    lluvia.core.node.container_node_descriptor
    ------------------------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libcpp.string cimport string

from lluvia.core.parameter cimport _Parameter
from lluvia.core.node.port_descriptor cimport _PortDescriptor


cdef extern from 'lluvia/core/node/ContainerNodeDescriptor.h' namespace 'll':

    cdef cppclass _ContainerNodeDescriptor 'll::ContainerNodeDescriptor':

        _ContainerNodeDescriptor()

        _ContainerNodeDescriptor& setBuilderName(const string& name)
        string getBuilderName() const

        _ContainerNodeDescriptor& addPort(_PortDescriptor& port)
        _PortDescriptor getPort(const string& name) except +

        _ContainerNodeDescriptor& setParameter(const string& name, const _Parameter& value)
        _Parameter getParameter(const string& name) except +


cdef class ContainerNodeDescriptor:
    cdef _ContainerNodeDescriptor __descriptor
