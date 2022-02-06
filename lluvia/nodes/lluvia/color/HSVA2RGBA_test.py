import pytest
import numpy as np

import lluvia as ll
import lluvia.util as ll_util
import lluvia_test as ll_test


def test_goodUse():

    nodeName = 'lluvia/color/HSVA2RGBA'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/color/HSVA2RGBA.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/color/HSVA2RGBA.comp.spv',
                     programName='lluvia/color/HSVA2RGBA.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    imgHSVA = np.zeros((640, 480, 4), dtype=np.float32)
    in_hsva = memory.createImageViewFromHost(imgHSVA)

    node.bind('in_hsva', in_hsva)
    node.init()

    out_rgba = node.getPort('out_rgba')
    assert(out_rgba is not None)
    assert(out_rgba.width == in_hsva.width)
    assert(out_rgba.height == in_hsva.height)
    assert(out_rgba.depth == in_hsva.depth)
    assert(out_rgba.channelType == ll.ChannelType.Uint8)
    assert(out_rgba.channels == 4)

    session.run(node)

    assert(not session.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
