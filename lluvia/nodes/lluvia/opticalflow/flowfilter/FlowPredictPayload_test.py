import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test


def runTest(dtype, channelType):

    nodeName = 'lluvia/opticalflow/flowfilter/FlowPredictPayload'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictPayloadX.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictPayloadX.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowPredictPayloadX.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictPayloadY.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictPayloadY.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowPredictPayloadY.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictPayload.lua')

    node = session.createContainerNode(nodeName)

    memory = session.createMemory(flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_flow = memory.createImageViewFromHost(np.zeros((480, 640, 2), dtype=dtype))
    in_gray = memory.createImageViewFromHost(np.zeros((480, 640), dtype=dtype))
    in_vector = memory.createImageViewFromHost(np.zeros((480, 640, 2), dtype=dtype))

    node.setParameter('max_flow', ll.Parameter(4))
    node.bind('in_flow', in_flow)
    node.bind('in_gray', in_gray)
    node.bind('in_vector', in_vector)
    node.init()

    out_flow = node.getPort('out_flow')
    assert(out_flow is not None)
    assert(out_flow.width == in_flow.width)
    assert(out_flow.height == in_flow.height)
    assert(out_flow.depth == in_flow.depth)
    assert(out_flow.channelType == channelType)
    assert(out_flow.channels == 2)

    out_gray = node.getPort('out_gray')
    assert(out_gray is not None)
    assert(out_gray.width == in_gray.width)
    assert(out_gray.height == in_gray.height)
    assert(out_gray.depth == in_gray.depth)
    assert(out_gray.channelType == channelType)
    assert(out_gray.channels == 1)

    out_vector = node.getPort('out_vector')
    assert(out_vector is not None)
    assert(out_vector.width == in_vector.width)
    assert(out_vector.height == in_vector.height)
    assert(out_vector.depth == in_vector.depth)
    assert(out_vector.channelType == channelType)
    assert(out_vector.channels == 2)

    session.run(node)

    assert(not session.hasReceivedVulkanWarningMessages())

def test_goodUse():

    runTest(np.float32, ll.ChannelType.Float32)


def test_goodUseFloat16():

    runTest(np.float16, ll.ChannelType.Float16)


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
