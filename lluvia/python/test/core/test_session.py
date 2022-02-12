import pytest
import lluvia as ll


def test_getAvailableDevices():

    devices = ll.getAvailableDevices()
    assert(devices is not None)
    assert(len(devices) >= 1)

    for dev in devices:
        assert(dev is not None)
        print(dev)


def test_session():

    session = ll.createSession(enableDebug=True, loadNodeLibrary = False)
    assert(session is not None)

    assert(not session.hasReceivedVulkanWarningMessages())


def test_multipleDevices():

    devices = ll.getAvailableDevices()
    assert(devices is not None)

    for dev in devices:
        session = ll.createSession(enableDebug=True, loadNodeLibrary = None, device=dev)
        assert(session is not None)
        assert(dev == session.deviceDescriptor)

    assert(not session.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
