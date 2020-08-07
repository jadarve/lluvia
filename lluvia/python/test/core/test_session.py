import sys
import os

import pytest
import lluvia as ll



def test_session():

    # TODO: This still does not work with the py_wheel data dependency
    #       AttributeError: module 'lluvia' has no attribute 'createSession'
    session = ll.createSession()


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

    session = ll.createSession()
    memFlags = session.getSupportedMemoryPropertyFlags()

    devFlagsFound = False
    hostFlagsFound = False

    for flags in memFlags:

        if ll.MemoryPropertyFlagBits.HostVisible in flags and ll.MemoryPropertyFlagBits.HostCoherent in flags:
            hostFlagsFound = True

        if ll.MemoryPropertyFlagBits.DeviceLocal in flags:
            devFlagsFound = True

    assert(hostFlagsFound and devFlagsFound)


if __name__ == "__main__":
    raise SystemExit(pytest.main([__file__]))
