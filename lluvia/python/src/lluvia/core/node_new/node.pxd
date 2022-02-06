"""
    lluvia.core.node.node
    ---------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from lluvia.core.node_new.node_type cimport _NodeType

cdef extern from 'lluvia/core/node/Node.h' namespace 'll':

    cdef cppclass _Node 'll::Node':
        _NodeType getType() const
