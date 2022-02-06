import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test


def test_goodUse():

    nodeName = 'lluvia/opticalflow/flowfilter/FlowSmooth'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowSmooth.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowSmooth.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowSmooth.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_flow = memory.createImageViewFromHost(
        np.zeros((480, 640, 2), dtype=np.float32))

    node.bind('in_flow', in_flow)
    node.init()

    out_flow = node.getPort('out_flow')
    assert(out_flow is not None)
    assert(out_flow.width == in_flow.width)
    assert(out_flow.height == in_flow.height)
    assert(out_flow.depth == in_flow.depth)
    assert(out_flow.channelType == ll.ChannelType.Float32)
    assert(out_flow.channels == 2)

    session.run(node)

    assert(not session.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
