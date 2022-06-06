import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test


def runTest(precision, dtype, channelType):

    nodeName = 'lluvia/opticalflow/HornSchunck/ImageProcessor'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/HornSchunck/ImageProcessor.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/HornSchunck/ImageProcessor.comp.spv',
                     programName='lluvia/opticalflow/HornSchunck/ImageProcessor.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_gray = memory.createImageViewFromHost(np.zeros((480, 640), dtype=np.uint8))
    in_gray_old = memory.createImageViewFromHost(np.zeros((480, 640), dtype=dtype))

    node.bind('in_gray', in_gray)
    node.bind('in_gray_old', in_gray_old)

    node.setParameter('alpha_square', ll.Parameter(0.01))
    node.setParameter('float_precision', ll.Parameter(precision.value))

    node.init()

    out_gray = node.getPort('out_gray')
    assert(out_gray is not None)
    assert(out_gray.width == in_gray.width)
    assert(out_gray.height == in_gray.height)
    assert(out_gray.depth == in_gray.depth)
    assert(out_gray.channelType == channelType)
    assert(out_gray.channels == 1)

    out_image_params = node.getPort('out_image_params')
    assert(out_image_params is not None)
    assert(out_image_params.width == in_gray.width)
    assert(out_image_params.height == in_gray.height)
    assert(out_image_params.depth == in_gray.depth)
    assert(out_image_params.channelType == channelType)
    assert(out_image_params.channels == 4)

    session.run(node)

    assert(not session.hasReceivedVulkanWarningMessages())


def test_goodUse():
    runTest(ll.FloatPrecision.FP32, np.float32, ll.ChannelType.Float32)


def test_goodUseFloat16():
    runTest(ll.FloatPrecision.FP16, np.float16, ll.ChannelType.Float16)


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
