import pytest
import numpy as np

import lluvia as ll
import lluvia.util as ll_util

def test_createFromImageWithDefaultParams():

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')

    session = ll.createSession(loadNodeLibrary=False)
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


def test_createFromMemoryWithDefaultParams():

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')

    session = ll.createSession(loadNodeLibrary=False)
    mem = session.createMemory()

    imgView = mem.createImageViewFromHost(imgRGBA)
    assert(imgView is not None)

    assert(imgView.filterMode == ll.ImageFilterMode.Nearest)
    assert(imgView.addressModeU == ll.ImageAddressMode.Repeat)
    assert(imgView.addressModeV == ll.ImageAddressMode.Repeat)
    assert(imgView.addressModeW == ll.ImageAddressMode.Repeat)
    assert(imgView.normalizedCoordinates == False)
    assert(imgView.sampled == False)


def test_createSampled():

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')

    session = ll.createSession(loadNodeLibrary=False)
    mem = session.createMemory()

    imgView = mem.createImageViewFromHost(imgRGBA, sampled=True)
    
    assert(imgView is not None)
    assert(imgView.sampled == True)

if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
