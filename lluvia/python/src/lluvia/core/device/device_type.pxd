"""
    lluvia.core.device.device_type
    ------------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cdef extern from 'lluvia/core/device/DeviceType.h' namespace 'll':

    cdef enum _DeviceType             'll::DeviceType':
        _DeviceType_Other             'll::DeviceType::Other'
        _DeviceType_DiscreteGPU       'll::DeviceType::DiscreteGPU'
        _DeviceType_IntegratedGPU     'll::DeviceType::IntegratedGPU'
        _DeviceType_VirtualGPU        'll::DeviceType::VirtualGPU'
        _DeviceType_CPU               'll::DeviceType::CPU'


cpdef enum DeviceType:
    Other             = <uint32_t> _DeviceType_Other
    DiscreteGPU       = <uint32_t> _DeviceType_DiscreteGPU
    IntegratedGPU     = <uint32_t> _DeviceType_IntegratedGPU
    VirtualGPU        = <uint32_t> _DeviceType_VirtualGPU
    CPU               = <uint32_t> _DeviceType_CPU
