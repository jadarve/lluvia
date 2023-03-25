import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test


def runTest(dtype):

    nodeName = 'lluvia/viz/ColorMap_gray'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/viz/ColorMap_gray.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/viz/ColorMap_gray.comp.spv',
                     programName='lluvia/viz/ColorMap_gray.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_image = memory.createImageViewFromHost(np.zeros((480, 640, 1), dtype=dtype))

    node.setParameter('min_value', ll.Parameter(0.0))
    node.setParameter('max_value', ll.Parameter(1.0))
    node.setParameter('alpha', ll.Parameter(1.0))
    node.bind('in_image', in_image)
    node.init()

    out_rgba = node.getPort('out_rgba')
    assert(out_rgba is not None)
    assert(out_rgba.width == in_image.width)
    assert(out_rgba.height == in_image.height)
    assert(out_rgba.depth == in_image.depth)
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
