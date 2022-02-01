"""
    lluvia.core.device.device_descriptor
    ------------------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t
from libcpp.string cimport string

from lluvia.core.device.device_type cimport _DeviceType

cdef extern from 'lluvia/core/device/DeviceDescriptor.h' namespace 'll':

    cdef struct _DeviceDescriptor 'll::DeviceDescriptor':
        uint32_t id
        _DeviceType deviceType
        string name


cdef class DeviceDescriptor:
    cdef _DeviceDescriptor __desc
