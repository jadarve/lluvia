import pytest
import time
import lluvia as ll


def test_hasReceivedVulkanWarningMessages():

    devices = ll.getAvailableDevices()
    assert(devices is not None)

    for dev in devices:
        session = ll.createSession(
            enableDebug=True, loadNodeLibrary=None, device=dev)
        assert(session is not None)
        assert(dev == session.deviceDescriptor)

        memory = session.createMemory(
            ll.MemoryPropertyFlagBits.DeviceLocal, pageSize=0)
        assert(memory is not None)
        
        # create a source buffer without the ll.BufferUsageFlagBits.TransferSrc flag bit
        sourceBuffer = memory.createBuffer(1024, [ll.BufferUsageFlagBits.StorageBuffer,
                                                  ll.BufferUsageFlagBits.TransferDst])
        assert(sourceBuffer is not None)

        # // create a destination buffer without the ll.BufferUsageFlagBits.TransferDst flag bit
        destinationBuffer = memory.createBuffer(1024, [ll.BufferUsageFlagBits.StorageBuffer,
                                                       ll.BufferUsageFlagBits.TransferSrc])
        assert(sourceBuffer is not None)

        cmdBuffer = session.createCommandBuffer()
        assert(cmdBuffer is not None)

        cmdBuffer.begin()
        cmdBuffer.copyBuffer(sourceBuffer, destinationBuffer)
        cmdBuffer.end()

        session.run(cmdBuffer)

        assert(session.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
