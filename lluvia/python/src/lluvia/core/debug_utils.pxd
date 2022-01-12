"""
    lluvia.core.debug_utils
    -----------------------

    :copyright: 2020, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libcpp cimport bool

cdef extern from 'lluvia/core/debug_utils.h' namespace 'll':

    bool _hasReceivedVulkanWarningMessages 'll::hasReceivedVulkanWarningMessages'() 
