import pytest

import lluvia as ll
import lluvia.util as ll_util
import lluvia_test as ll_test


def test_goodUse():

    nodeName = 'lluvia/color/RGBA2BGRA'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/color/RGBA2BGRA.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/color/RGBA2BGRA.comp.spv',
                     programName='lluvia/color/RGBA2BGRA.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')
    in_rgba = memory.createImageViewFromHost(imgRGBA)

    node.bind('in_rgba', in_rgba)
    node.init()

    out_bgra = node.getPort('out_bgra')
    assert(out_bgra is not None)
    assert(out_bgra.width == in_rgba.width)
    assert(out_bgra.height == in_rgba.height)
    assert(out_bgra.depth == in_rgba.depth)
    assert(out_bgra.channelType == ll.ChannelType.Uint8)
    assert(out_bgra.channels == 4)

    session.run(node)

    out_bgra_host = out_bgra.toHost()

    # assert pixel content by channel
    assert((imgRGBA[..., 0] == out_bgra_host[..., 2]).all())
    assert((imgRGBA[..., 1] == out_bgra_host[..., 1]).all())
    assert((imgRGBA[..., 3] == out_bgra_host[..., 3]).all())

    assert(not session.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
