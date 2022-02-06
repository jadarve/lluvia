
import pytest
import numpy as np
import lluvia as ll

def test_create():

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    mem = session.createMemory(ll.MemoryPropertyFlagBits.DeviceLocal)

    size = 512
    # some flags different to the default values of mem.createBuffer()
    flags = [ll.BufferUsageFlagBits.UniformBuffer]

    buf = mem.createBuffer(size, flags)

    assert(buf.size == size)
    assert(len(flags) == len(buf.usageFlags))

    # check that all flags have the correct value
    for f in buf.usageFlags:
        assert(f in flags)
    
    assert(not session.hasReceivedVulkanWarningMessages())


def test_fromHost():

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    mem = session.createMemory()

    dtype = np.uint8

    arr = np.arange(0, 64, dtype=dtype)
    buf = mem.createBufferFromHost(arr)
    copy = buf.toHost(dtype=dtype)

    for i in range(len(arr)):
        assert(arr[i] == copy[i])
    
    assert(not session.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
