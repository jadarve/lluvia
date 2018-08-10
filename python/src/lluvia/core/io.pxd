"""
    lluvia.core.io
    --------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libcpp.memory cimport shared_ptr
from libcpp.string cimport string

from compute_node cimport _ComputeNodeDescriptor, _ComputeNode


cdef extern from 'lluvia/core/io.h' namespace 'll':

    void _writeComputeNodeDescriptor 'll::writeComputeNodeDescriptor' (const _ComputeNodeDescriptor& descriptor, const string& filePath) except +
    void _writeComputeNode 'll::writeComputeNode' (const _ComputeNode& node, const string& filePath) except +
