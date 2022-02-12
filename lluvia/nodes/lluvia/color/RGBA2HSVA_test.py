import pytest

import lluvia as ll
import lluvia.util as ll_util
import lluvia_test as ll_test


def test_goodUse():

    nodeName = 'lluvia/color/RGBA2HSVA'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/color/RGBA2HSVA.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/color/RGBA2HSVA.comp.spv',
                     programName='lluvia/color/RGBA2HSVA.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')
    in_rgba = memory.createImageViewFromHost(imgRGBA)

    node.bind('in_rgba', in_rgba)
    node.init()

    out_hsva = node.getPort('out_hsva')
    assert(out_hsva is not None)
    assert(out_hsva.width == in_rgba.width)
    assert(out_hsva.height == in_rgba.height)
    assert(out_hsva.depth == in_rgba.depth)
    assert(out_hsva.channelType == ll.ChannelType.Float32)
    assert(out_hsva.channels == 4)

    session.run(node)

    assert(not session.hasReceivedVulkanWarningMessages())


def test_goodUseHalfPrecision():

    nodeName = 'lluvia/color/RGBA2HSVA'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/color/RGBA2HSVA.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/color/RGBA2HSVA.comp.spv',
                     programName='lluvia/color/RGBA2HSVA.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')
    in_rgba = memory.createImageViewFromHost(imgRGBA)

    node.setParameter('float_precision', ll.Parameter(ll.FloatPrecision.FP16.value))
    node.bind('in_rgba', in_rgba)
    node.init()

    out_hsva = node.getPort('out_hsva')
    assert(out_hsva is not None)
    assert(out_hsva.width == in_rgba.width)
    assert(out_hsva.height == in_rgba.height)
    assert(out_hsva.depth == in_rgba.depth)
    assert(out_hsva.channelType == ll.ChannelType.Float16)
    assert(out_hsva.channels == 4)

    session.run(node)

    assert(not session.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
