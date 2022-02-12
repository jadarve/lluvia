# cython: language_level=3, boundscheck=False, emit_code_comments=True, embedsignature=True

"""
    lluvia.core.node.node_builder_descriptor
    ----------------------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t

from lluvia.core import impl

from lluvia.core.node.node_type cimport NodeType
from lluvia.core.node.node_type import NodeType as NodeType_t


__all__ = [
    'NodeBuilderDescriptor',
]

cdef class NodeBuilderDescriptor:

    def __init__(self, NodeType nodeType, str name, str summary):

        self.__descriptor.nodeType = <_NodeType> nodeType
        self.__descriptor.name = impl.encodeString(name)
        self.__descriptor.summary = impl.encodeString(summary)

    property nodeType:
        def __get__(self):
            return NodeType_t(<uint32_t> self.__descriptor.nodeType)

    property name:
        def __get__(self):
            return str(self.__descriptor.name, 'utf-8')

    property summary:
        def __get__(self):
            return str(self.__descriptor.summary, 'utf-8')
    
    def __str__(self):
        return '{0:40s} : {1:9s} : {2:s}'.format(self.name, self.nodeType.name, self.summary)
