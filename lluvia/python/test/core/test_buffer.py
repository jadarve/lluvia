import sys
sys.path.append('../build/python/lib.linux-x86_64-3.6')

import pytest
import numpy as np
import lluvia as ll


def test_creation():

    session = ll.createSession()
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

    session = ll.createSession()
    mem = session.createMemory()

    dtype = np.uint8

    arr = np.arange(0, 64, dtype=dtype)
    buf = mem.createBufferFromHost(arr)
    copy = buf.toHost(dtype=dtype)

    for i in range(len(arr)):
        assert(arr[i] == copy[i])
