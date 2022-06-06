import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test


def runTest(dtype, channelType):

    nodeName = 'lluvia/opticalflow/HornSchunck/NumericIteration'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/HornSchunck/NumericIteration.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/HornSchunck/NumericIteration.comp.spv',
                     programName='lluvia/opticalflow/HornSchunck/NumericIteration.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_flow = memory.createImageViewFromHost(np.zeros((480, 640, 2), dtype=dtype))
    in_image_params = memory.createImageViewFromHost(np.zeros((480, 640, 4), dtype=dtype))

    node.bind('in_flow', in_flow)
    node.bind('in_image_params', in_image_params)
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
