import pytest
import imageio
import numpy as np


def test_creation():

    import lluvia as ll

    session = ll.createSession(loadNodeLibrary = False)

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

    assert(len(flags) == len(img.usageFlags))
    for f in img.usageFlags:
        assert(f in flags)


def test_incorrectSize():

    import lluvia as ll

    session = ll.createSession(loadNodeLibrary = False)

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


def test_imageFromHost():

    import lluvia as ll

    img = imageio.imread('lluvia/resources/mouse.jpg')
    imgRGBA = np.zeros(img.shape[:-1] + tuple([4]), dtype=img.dtype)
    imgRGBA[..., :3] = img

    session = ll.createSession(loadNodeLibrary = False)
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


if __name__ == "__main__":

    from config import add_lluvia_import_path
    add_lluvia_import_path()

    raise SystemExit(pytest.main([__file__]))
