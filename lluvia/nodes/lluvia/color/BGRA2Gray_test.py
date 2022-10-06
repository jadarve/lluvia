import pytest

import lluvia as ll
import lluvia.util as ll_util
import lluvia_test as ll_test


def test_goodUse():

    nodeName = 'lluvia/color/BGRA2Gray'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/color/BGRA2Gray.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/color/BGRA2Gray.comp.spv',
                     programName='lluvia/color/BGRA2Gray.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')
    in_rgba = memory.createImageViewFromHost(imgRGBA)

    node.bind('in_bgra', in_rgba)
    node.init()

    out_gray = node.getPort('out_gray')
    assert(out_gray is not None)
    assert(out_gray.width == in_rgba.width)
    assert(out_gray.height == in_rgba.height)
    assert(out_gray.depth == in_rgba.depth)
    assert(out_gray.channelType == ll.ChannelType.Uint8)
    assert(out_gray.channels == 1)

    session.run(node)

    assert(not session.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
