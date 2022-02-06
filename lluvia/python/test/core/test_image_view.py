import pytest
import numpy as np

import lluvia as ll
import lluvia.util as ll_util

def test_createFromImageWithDefaultParams():

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    mem = session.createMemory()

    devImg = mem.createImageFromHost(imgRGBA)
    assert(devImg is not None)

    imgView = devImg.createImageView()
    assert(imgView is not None)

    assert(imgView.filterMode == ll.ImageFilterMode.Nearest)
    assert(imgView.addressModeU == ll.ImageAddressMode.Repeat)
    assert(imgView.addressModeV == ll.ImageAddressMode.Repeat)
    assert(imgView.addressModeW == ll.ImageAddressMode.Repeat)
    assert(imgView.normalizedCoordinates == False)
    assert(imgView.sampled == False)

    assert(not session.hasReceivedVulkanWarningMessages())


def test_createFromMemoryWithDefaultParams():

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    mem = session.createMemory()

    imgView = mem.createImageViewFromHost(imgRGBA)
    assert(imgView is not None)

    assert(imgView.filterMode == ll.ImageFilterMode.Nearest)
    assert(imgView.addressModeU == ll.ImageAddressMode.ClampToEdge)
    assert(imgView.addressModeV == ll.ImageAddressMode.ClampToEdge)
    assert(imgView.addressModeW == ll.ImageAddressMode.ClampToEdge)
    assert(imgView.normalizedCoordinates == False)
    assert(imgView.sampled == False)

    assert(not session.hasReceivedVulkanWarningMessages())


def test_createSampled():

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    mem = session.createMemory()

    imgView = mem.createImageViewFromHost(imgRGBA, sampled=True)
    
    assert(imgView is not None)
    assert(imgView.sampled == True)

    assert(not session.hasReceivedVulkanWarningMessages())

if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
