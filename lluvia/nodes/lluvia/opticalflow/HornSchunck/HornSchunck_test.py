import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test


def runTest(precision, channelType):

    nodeName = 'lluvia/opticalflow/HornSchunck/HornSchunck'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/HornSchunck/ImageProcessor.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/HornSchunck/ImageProcessor.comp.spv',
                     programName='lluvia/opticalflow/HornSchunck/ImageProcessor.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/HornSchunck/NumericIteration.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/HornSchunck/NumericIteration.comp.spv',
                     programName='lluvia/opticalflow/HornSchunck/NumericIteration.comp'
                     )
    
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/HornSchunck/HornSchunck.lua')

    node = session.createContainerNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_gray = memory.createImageViewFromHost(
        np.zeros((480, 640), dtype=np.uint8))

    node.setParameter('iterations', ll.Parameter(5))
    node.setParameter('float_precision', ll.Parameter(precision.value))
    node.bind('in_gray', in_gray)
    node.init()

    out_flow = node.getPort('out_flow')
    assert(out_flow is not None)
    assert(out_flow.width == in_gray.width)
    assert(out_flow.height == in_gray.height)
    assert(out_flow.depth == in_gray.depth)
    assert(out_flow.channelType == channelType)
    assert(out_flow.channels == 2)

    out_gray = node.getPort('out_gray')
    assert(out_gray is not None)
    assert(out_gray.width == in_gray.width)
    assert(out_gray.height == in_gray.height)
    assert(out_gray.depth == in_gray.depth)
    assert(out_gray.channelType == channelType)
    assert(out_gray.channels == 1)

    session.run(node)

    assert(not session.hasReceivedVulkanWarningMessages())


def test_goodUse():

    runTest(ll.FloatPrecision.FP32, ll.ChannelType.Float32)


def test_goodUseFloat16():

    runTest(ll.FloatPrecision.FP16, ll.ChannelType.Float16)


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
