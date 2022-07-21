import pytest
import numpy as np
import lluvia as ll

def test_create():

    session = ll.createSession(enableDebug=True, loadNodeLibrary = False)

    mem = session.createMemory(ll.MemoryPropertyFlagBits.DeviceLocal)

    width = 640
    height = 480
    flags = [
        ll.ImageUsageFlagBits.TransferSrc,
        ll.ImageUsageFlagBits.TransferDst,
        ll.ImageUsageFlagBits.Storage,
        ll.ImageUsageFlagBits.Sampled
    ]
    channelType = ll.ChannelType.Uint8

    img = mem.createImage([height, width, 1], channelType, flags)

    assert(img.width == width)
    assert(img.height == height)
    assert(img.depth == 1)
    assert(img.channels == 1)
    assert(img.channelType == channelType)
    assert(img.shape == (1, height, width, 1))
    assert(img.layout == ll.ImageLayout.General)

    assert(len(flags) == len(img.usageFlags))
    for f in img.usageFlags:
        assert(f in flags)
    
    assert(not session.hasReceivedVulkanWarningMessages())


def test_incorrectSize():

    session = ll.createSession(enableDebug=True, loadNodeLibrary = False)

    mem = session.createMemory(ll.MemoryPropertyFlagBits.DeviceLocal)

    width = 640
    height = 480
    depth = 10
    channels = 4
    channelType = ll.ChannelType.Uint8

    # wrong shapes that should raise RuntimeError
    shapes = [(0, height, width, channels),
              (depth, 0, width, channels),
              (depth, height, 0, channels),
              (depth, height, width, 0),
              (depth, height, height, 5)]

    with pytest.raises(RuntimeError):

        for shape in shapes:
            _ = mem.createImage(shape, channelType)
    
    assert(not session.hasReceivedVulkanWarningMessages())


def test_createFromHost():
    import lluvia.util as ll_util

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')

    session = ll.createSession(enableDebug=True, loadNodeLibrary = False)
    mem = session.createMemory()

    devImg = mem.createImageFromHost(imgRGBA)

    assert(devImg.depth == 1)
    assert(devImg.height == imgRGBA.shape[0])
    assert(devImg.width == imgRGBA.shape[1])
    assert(devImg.channels == imgRGBA.shape[2])

    hostImg = devImg.toHost()
    assert(hostImg.shape == imgRGBA.shape)

    # pixel-wise check
    assert((hostImg[:] == imgRGBA[:]).all())

    assert(not session.hasReceivedVulkanWarningMessages())

def test_fromAndToHost():
    import lluvia.util as ll_util

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    mem = session.createMemory()

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')

    devImg = mem.createImage(imgRGBA.shape, channelType=ll.ChannelType.Uint8)
    assert(devImg is not None)

    devImg.fromHost(imgRGBA)

    hostImg = devImg.toHost()
    assert(hostImg.shape == imgRGBA.shape)

    # pixel-wise check
    assert((hostImg[:] == imgRGBA[:]).all())

    assert(not session.hasReceivedVulkanWarningMessages())

def test_changeLayout():

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    mem = session.createMemory(ll.MemoryPropertyFlagBits.DeviceLocal)

    devImg = mem.createImage((640, 480), ll.ChannelType.Uint8)

    devImg.changeLayout(ll.ImageLayout.TransferDstOptimal)
    assert(devImg.layout == ll.ImageLayout.TransferDstOptimal)

    assert(not session.hasReceivedVulkanWarningMessages())


def test_clear():

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    mem = session.createMemory(ll.MemoryPropertyFlagBits.DeviceLocal)

    devImg = mem.createImage((640, 480), ll.ChannelType.Uint8)
    devImg.clear()

    hostImg = devImg.toHost()

    # pixel-wise check
    assert((hostImg[:] == 0).all())

    assert(not session.hasReceivedVulkanWarningMessages())


def test_copyImageToImage():

    import lluvia.util as ll_util

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    mem = session.createMemory()

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')

    devImg = mem.createImage(imgRGBA.shape, channelType=ll.ChannelType.Uint8)
    assert(devImg is not None)

    # copy pixel content to device image
    devImg.fromHost(imgRGBA)

    # image copy allocated with undefined memory content
    devImgCopy = mem.createImage(imgRGBA.shape, channelType=ll.ChannelType.Uint8)

    devImg.copyTo(devImgCopy)

    hostImg = devImg.toHost()
    hostImgCopy = devImgCopy.toHost()

    # pixel-wise check
    assert((hostImg[:] == hostImgCopy[:]).all())

    assert(not session.hasReceivedVulkanWarningMessages())


def test_copyImageToImageView():

    import lluvia.util as ll_util

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    mem = session.createMemory()

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')

    devImg = mem.createImage(imgRGBA.shape, channelType=ll.ChannelType.Uint8)
    assert(devImg is not None)

    # copy pixel content to device image
    devImg.fromHost(imgRGBA)

    # image copy allocated with undefined memory content
    devImgCopy = mem.createImage(imgRGBA.shape, channelType=ll.ChannelType.Uint8)
    devImgViewCopy = devImgCopy.createImageView()

    devImg.copyTo(devImgViewCopy)

    hostImg = devImg.toHost()
    hostImgViewCopy = devImgViewCopy.toHost()

    # pixel-wise check
    assert((hostImg[:] == hostImgViewCopy[:]).all())

    assert(not session.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
