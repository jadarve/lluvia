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
        """
        The device ID

        Returns
        -------
        id : uint
        """

        def __get__(self):
            return self.__desc.id
    
    property name:
        """
        The device name

        Returns
        -------
        name : str
        """

        def __get__(self):
            return str(self.__desc.name, 'utf-8')

    property deviceType:
        """
        The device type

        Returns
        -------
        deviceType : DeviceType
        """

        def __get__(self):
            return DeviceType(<uint32_t> self.__desc.deviceType)

    def __str__(self):
        return 'id: {0:<4d} type: {1:13s} name: {2}'.format(self.id, self.deviceType.name, self.name)

    def __eq__(self, other):

        cdef DeviceDescriptor otherCasted
        if isinstance(other, DeviceDescriptor):
            otherCasted = other
            return self.__desc == otherCasted.__desc
        
        return False
