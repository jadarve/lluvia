import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test


def runTest(dtype, channelType):

    nodeName = 'lluvia/opticalflow/flowfilter/FlowUpdateDelta'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowUpdateDelta.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowUpdateDelta.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowUpdateDelta.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_gray = memory.createImageViewFromHost(np.zeros((480, 640), dtype=dtype))
    in_gradient = memory.createImageViewFromHost(np.zeros((480, 640, 2), dtype=dtype))
    in_delta_flow = memory.createImageViewFromHost(np.zeros((480, 640, 2), dtype=dtype))
    in_gray_old = memory.createImageViewFromHost(np.zeros((480, 640), dtype=dtype))
    in_flow = memory.createImageViewFromHost(np.zeros((240, 320, 2), dtype=dtype),
                        normalizedCoordinates=True, sampled=True)

    # out_gray and out_delta_flow must be allocated externally
    out_gray = memory.createImageViewFromHost(np.zeros((480, 640), dtype=dtype))
    out_delta_flow = memory.createImageViewFromHost(np.zeros((480, 640, 2), dtype=dtype))

    node.bind('in_gray', in_gray)
    node.bind('in_gradient', in_gradient)
    node.bind('in_delta_flow', in_delta_flow)
    node.bind('in_gray_old', in_gray_old)
    node.bind('in_flow', in_flow)
    node.bind('out_gray', out_gray)
    node.bind('out_delta_flow', out_delta_flow)

    node.init()

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

    runTest(np.float32, ll.ChannelType.Float32)


def test_goodUseFloat16():

    runTest(np.float16, ll.ChannelType.Float16)


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
