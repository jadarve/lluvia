import sys
sys.path.append('../build/python/lib.linux-x86_64-2.7')

import pytest
import lluvia as ll


def test_session():
    
    session = ll.Session()
    

def test_memoryFlags():
    """
    Test that the returned memory flags meet the Vulkan speficiation.

    From the Vulkan specification:
    
        There must be at least one memory type with both the 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT and 
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bits set in its propertyFlags.
        There must be at least one memory type with the 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit set in its propertyFlags.
    """

    session = ll.Session()
    memFlags = session.getSupportedMemoryPropertyFlags()

    devFlagsFound = False
    hostFlagsFound = False

    for flags in memFlags:

        if 'HostVisible' in flags and 'HostCoherent' in flags:
            hostFlagsFound = True

        if 'DeviceLocal' in flags:
            devFlagsFound = True


    assert(hostFlagsFound and devFlagsFound)
