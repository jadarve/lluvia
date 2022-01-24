import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test


def test_goodUse():

    nodeName = 'lluvia/imgproc/ImageDownsampleY_r8ui'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/imgproc/ImageDownsampleY_r8ui.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/imgproc/ImageDownsampleY_r8ui.comp.spv',
                     programName='lluvia/imgproc/ImageDownsampleY_r8ui.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    
    in_gray = memory.createImageViewFromHost(np.zeros((480, 641), dtype=np.uint8))

    node.bind('in_gray', in_gray)
    node.init()

    out_gray = node.getPort('out_gray')
    assert(out_gray is not None)
    assert(out_gray.width == in_gray.width)
    assert(out_gray.height == np.floor(in_gray.height / 2.0))
    assert(out_gray.depth == in_gray.depth)
    assert(out_gray.channelType == ll.ChannelType.Uint8)
    assert(out_gray.channels == 1)

    session.run(node)

    assert(not ll.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
