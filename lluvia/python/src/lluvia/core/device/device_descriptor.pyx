"""
    lluvia.core.device.device_type
    ------------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

__all__ = [
    'DeviceDescriptor'
]


from lluvia.core.device.device_type import DeviceType
from lluvia.core.device.device_type cimport _DeviceType, DeviceType

cdef class DeviceDescriptor:

    def __init__(self):
        pass
    
    def __cinit__(self):
        pass
    
    property id:
        def __get__(self):
            return self.__desc.id
    
    property name:
        def __get__(self):
            return str(self.__desc.name, 'utf-8')

    property deviceType:
        def __get__(self):
            return DeviceType(<uint32_t> self.__desc.deviceType)

    def __str__(self):
        return 'id: {0} type: {1} name: {2}'.format(self.id, self.deviceType.name, self.name)

    def __eq__(self, other):

        cdef DeviceDescriptor otherCasted
        if isinstance(other, DeviceDescriptor):
            otherCasted = other
            return self.__desc == otherCasted.__desc
        
        return False
