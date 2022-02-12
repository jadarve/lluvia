import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test


def runTest(dtype, channelType):

    nodeName = 'lluvia/opticalflow/flowfilter/FlowPredict'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictX.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictX.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowPredictX.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictY.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictY.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowPredictY.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredict.lua')

    node = session.createContainerNode(nodeName)

    memory = session.createMemory(flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_flow = memory.createImageViewFromHost(np.zeros((480, 640, 2), dtype=dtype))

    node.setParameter('max_flow', ll.Parameter(4))
    node.bind('in_flow', in_flow)
    node.init()

    out_flow = node.getPort('out_flow')
    assert(out_flow is not None)
    assert(out_flow.width == in_flow.width)
    assert(out_flow.height == in_flow.height)
    assert(out_flow.depth == in_flow.depth)
    assert(out_flow.channelType == channelType)
    assert(out_flow.channels == 2)

    session.run(node)

    assert(not session.hasReceivedVulkanWarningMessages())


def test_goodUse():

    runTest(np.float32, ll.ChannelType.Float32)


def test_goodUseFloat16():

    runTest(np.float16, ll.ChannelType.Float16)

if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
