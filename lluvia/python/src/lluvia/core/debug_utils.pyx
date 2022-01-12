"""
    lluvia.core.debug_utils
    -----------------------

    :copyright: 2020, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libcpp cimport bool

__all__ = [
    'hasReceivedVulkanWarningMessages'
]

def hasReceivedVulkanWarningMessages():
    """
    returns whether or not the application has received messages through the debug callback.
    """

    cdef bool hasWarnings = _hasReceivedVulkanWarningMessages()
    return hasWarnings
