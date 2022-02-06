import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test


def test_goodUse():

    nodeName = 'lluvia/opticalflow/flowfilter/FlowPredictPayloadX'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictPayloadX.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictPayloadX.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowPredictPayloadX.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_flow = memory.createImageViewFromHost(np.zeros((480, 640, 2), dtype=np.float32))
    in_gray = memory.createImageViewFromHost(np.zeros((480, 640), dtype=np.float32))
    in_vector = memory.createImageViewFromHost(np.zeros((480, 640, 2), dtype=np.float32))

    node.bind('in_flow', in_flow)
    node.bind('in_gray', in_gray)
    node.bind('in_vector', in_vector)
    node.init()

    out_flow = node.getPort('out_flow')
    assert(out_flow is not None)
    assert(out_flow.width == in_flow.width)
    assert(out_flow.height == in_flow.height)
    assert(out_flow.depth == in_flow.depth)
    assert(out_flow.channelType == ll.ChannelType.Float32)
    assert(out_flow.channels == 2)

    out_gray = node.getPort('out_gray')
    assert(out_gray is not None)
    assert(out_gray.width == in_gray.width)
    assert(out_gray.height == in_gray.height)
    assert(out_gray.depth == in_gray.depth)
    assert(out_gray.channelType == ll.ChannelType.Float32)
    assert(out_gray.channels == 1)

    out_vector = node.getPort('out_vector')
    assert(out_vector is not None)
    assert(out_vector.width == in_vector.width)
    assert(out_vector.height == in_vector.height)
    assert(out_vector.depth == in_vector.depth)
    assert(out_vector.channelType == ll.ChannelType.Float32)
    assert(out_vector.channels == 2)

    session.run(node)

    assert(not session.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
