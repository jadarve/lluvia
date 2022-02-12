import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test


def runTest(dtype):

    nodeName = 'lluvia/viz/Flow2RGBA'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/viz/Flow2RGBA.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/viz/Flow2RGBA.comp.spv',
                     programName='lluvia/viz/Flow2RGBA.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_flow = memory.createImageViewFromHost(np.zeros((480, 640, 2), dtype=dtype))

    node.setParameter('max_flow', ll.Parameter(1.0))
    node.bind('in_flow', in_flow)
    node.init()

    out_rgba = node.getPort('out_rgba')
    assert(out_rgba is not None)
    assert(out_rgba.width == in_flow.width)
    assert(out_rgba.height == in_flow.height)
    assert(out_rgba.depth == in_flow.depth)
    assert(out_rgba.channelType == ll.ChannelType.Uint8)
    assert(out_rgba.channels == 4)

    session.run(node)

    assert(not session.hasReceivedVulkanWarningMessages())

def test_goodUse():

    runTest(np.float32)
    

def test_goodUseFloat16():

    runTest(np.float16)


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
