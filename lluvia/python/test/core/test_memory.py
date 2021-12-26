
import pytest
import sys
import os


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

    session = ll.createSession(loadNodeLibrary=False)
    assert(session is not None)

    memFlags = session.getSupportedMemoryPropertyFlags()
    assert(len(memFlags) != 0)

    devFlagsFound = False
    hostFlagsFound = False

    for flags in memFlags:

        if ll.MemoryPropertyFlagBits.HostVisible in flags and ll.MemoryPropertyFlagBits.HostCoherent in flags:
            hostFlagsFound = True

        if ll.MemoryPropertyFlagBits.DeviceLocal in flags:
            devFlagsFound = True

    assert(hostFlagsFound and devFlagsFound)


def test_createMemoryWithDefaultValues():

    import lluvia as ll

    session = ll.createSession(loadNodeLibrary=False)
    assert(session is not None)

    memory = session.createMemory()
    assert(memory is not None)


def test_createHostMemory():

    import lluvia as ll

    session = ll.createSession(loadNodeLibrary=False)
    assert(session is not None)

    flags = [ll.MemoryPropertyFlagBits.HostVisible,
             ll.MemoryPropertyFlagBits.HostCoherent]

    memory = session.createMemory(flags=flags, pageSize=4*1024*1024)
    assert(memory is not None)


def test_createDeviceMemory():
    
    import lluvia as ll

    session = ll.createSession(loadNodeLibrary=False)
    assert(session is not None)

    flags = [ll.MemoryPropertyFlagBits.DeviceLocal]

    memory = session.createMemory(flags=flags, pageSize=2*1024*1024)
    assert(memory is not None)


if __name__ == "__main__":

    from config import add_lluvia_import_path
    add_lluvia_import_path()

    raise SystemExit(pytest.main([__file__]))
