
import pytest
import sys
import os
from rules_python.python.runfiles import runfiles


def test_session():

    import lluvia as ll

    ll.createSession(loadNodeLibrary = False)


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

    import lluvia as ll

    session = ll.createSession(loadNodeLibrary = False)
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
    
    from config import add_lluvia_import_path
    add_lluvia_import_path()

    raise SystemExit(pytest.main([__file__]))
