"""
    lluvia.core.node.node_builder_descriptor
    ----------------------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libcpp.string cimport string

from lluvia.core.node_new.node_type cimport _NodeType


cdef extern from 'lluvia/core/node/NodeBuilderDescriptor.h' namespace 'll':

    cdef struct _NodeBuilderDescriptor 'll::NodeBuilderDescriptor':

        _NodeType nodeType
        string name
        string summary
        
cdef class NodeBuilderDescriptor:
    cdef _NodeBuilderDescriptor __descriptor
