import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test


def runTest(precision, channelType):

    nodeName = 'lluvia/opticalflow/flowfilter/ImageModel'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/ImageModel.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/ImageModel.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/ImageModel.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_gray = memory.createImageViewFromHost(np.zeros((480, 640), dtype=np.uint8))

    node.setParameter('float_precision', ll.Parameter(precision.value))
    node.bind('in_gray', in_gray)
    node.init()


    out_gray = node.getPort('out_gray')
    assert(out_gray is not None)
    assert(out_gray.width == in_gray.width)
    assert(out_gray.height == in_gray.height)
    assert(out_gray.depth == in_gray.depth)
    assert(out_gray.channelType == channelType)
    assert(out_gray.channels == 1)

    out_gradient = node.getPort('out_gradient')
    assert(out_gradient is not None)
    assert(out_gradient.width == in_gray.width)
    assert(out_gradient.height == in_gray.height)
    assert(out_gradient.depth == in_gray.depth)
    assert(out_gradient.channelType == channelType)
    assert(out_gradient.channels == 2)

    session.run(node)

    assert(not session.hasReceivedVulkanWarningMessages())


def test_goodUse():

    runTest(ll.FloatPrecision.FP32, ll.ChannelType.Float32)


def test_goodUseFloat16():

    runTest(ll.FloatPrecision.FP16, ll.ChannelType.Float16)

if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
