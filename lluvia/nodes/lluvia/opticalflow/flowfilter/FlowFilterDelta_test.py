import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test


def runTest(precision, dtype, channelType):

    nodeName = 'lluvia/opticalflow/flowfilter/FlowFilterDelta'

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
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowUpdateDelta.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowUpdateDelta.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowUpdateDelta.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowSmooth.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowSmooth.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowSmooth.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/ImageModel.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/ImageModel.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/ImageModel.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowFilterDelta.lua')

    node = session.createContainerNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_gray = memory.createImageViewFromHost(np.zeros((480, 640), dtype=np.uint8))
    in_flow = memory.createImageViewFromHost(np.zeros((240, 320, 2), dtype=dtype))

    node.setParameter('maxflow', ll.Parameter(4))
    node.setParameter('smooth_iterations', ll.Parameter(2))
    node.setParameter('float_precision', ll.Parameter(precision.value))
    node.bind('in_gray', in_gray)
    node.bind('in_flow', in_flow)
    node.init()

    out_flow = node.getPort('out_flow')
    assert(out_flow is not None)
    assert(out_flow.width == in_gray.width)
    assert(out_flow.height == in_gray.height)
    assert(out_flow.depth == in_gray.depth)
    assert(out_flow.channelType == channelType)
    assert(out_flow.channels == 2)

    out_delta_flow = node.getPort('out_delta_flow')
    assert(out_delta_flow is not None)
    assert(out_delta_flow.width == in_gray.width)
    assert(out_delta_flow.height == in_gray.height)
    assert(out_delta_flow.depth == in_gray.depth)
    assert(out_delta_flow.channelType == channelType)
    assert(out_delta_flow.channels == 2)

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

    runTest(ll.FloatPrecision.FP32, np.float32, ll.ChannelType.Float32)
    

def test_goodUseFloat16():

    runTest(ll.FloatPrecision.FP16, np.float16, ll.ChannelType.Float16)


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
