"""
    lluvia.core.io
    --------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from cython.operator cimport dereference as deref

from compute_node cimport ComputeNode, ComputeNodeDescriptor

from . import impl


__all__ = [ 'writeComputeNode',
            'writeComputeNodeDescriptor']


def writeComputeNodeDescriptor(ComputeNodeDescriptor desc, filePath):
    """
    Write a compute node descriptor as a JSON file.

    The JSON file has the following structure:

        {
            "function"   : "function name",
            "grid_x"     : int,
            "grid_y"     : int,
            "grid_z"     : int,
            "local_x"    : int,
            "local_y"    : int,
            "local_z"    : int,
            "parameters" : [
                "Buffer",
                "ImageView",
                "SampledImageView",
                ...
            ],
            "spirv"      : "base 64 SPIR-V code"
        }

    Parameters
    ----------
    desc : ComputeNodeDescriptor.
        The descriptor to write

    filePath : str
        The file path.
    """

    filePath = impl.encodeString(filePath)
    _writeComputeNodeDescriptor(desc.__descriptor, filePath)


def writeComputeNode(ComputeNode node, filePath):
    """
    Write a compute node as a JSON file.

    The JSON file has the following structure:

        {
            "function"   : "function name",
            "grid_x"     : int,
            "grid_y"     : int,
            "grid_z"     : int,
            "local_x"    : int,
            "local_y"    : int,
            "local_z"    : int,
            "parameters" : [
                "Buffer",
                "ImageView",
                "SampledImageView",
                ...
            ],
            "spirv"      : "base 64 SPIR-V code"
        }

    Parameters
    ----------
    desc : ComputeNodeDescriptor.
        The descriptor to write

    filePath : str
        The file path.
    """

    filePath = impl.encodeString(filePath)
    _writeComputeNode(deref(node.__node.get()), filePath)
