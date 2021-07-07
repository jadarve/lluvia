
import pytest
import numpy as np


def test_creation():

    import lluvia as ll

    session = ll.createSession(loadNodeLibrary = False)
    mem = session.createMemory(ll.MemoryPropertyFlagBits.DeviceLocal)

    size = 512
    # some flags different to the default values of mem.createBuffer()
    flags = [ll.BufferUsageFlagBits.IndexBuffer,
             ll.BufferUsageFlagBits.UniformBuffer]

    buf = mem.createBuffer(size, flags)

    assert(buf.size == size)
    assert(len(flags) == len(buf.usageFlags))

    # check that all flags have the correct value
    for f in buf.usageFlags:
        assert(f in flags)


def test_fromHost():

    import lluvia as ll

    session = ll.createSession(loadNodeLibrary = False)
    mem = session.createMemory()

    dtype = np.uint8

    arr = np.arange(0, 64, dtype=dtype)
    buf = mem.createBufferFromHost(arr)
    copy = buf.toHost(dtype=dtype)

    for i in range(len(arr)):
        assert(arr[i] == copy[i])


if __name__ == "__main__":

    from config import add_lluvia_import_path
    add_lluvia_import_path()

    raise SystemExit(pytest.main([__file__]))
