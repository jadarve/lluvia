import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test


nodeName = 'lluvia/imgproc/ImagePyramid_r8ui'

def loadNodes(session):

    # load the container node dependecies
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/imgproc/ImageDownsampleX_r8ui.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/imgproc/ImageDownsampleX_r8ui.comp.spv',
                     programName='lluvia/imgproc/ImageDownsampleX_r8ui.comp')
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/imgproc/ImageDownsampleY_r8ui.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/imgproc/ImageDownsampleY_r8ui.comp.spv',
                     programName='lluvia/imgproc/ImageDownsampleY_r8ui.comp')

    # load the container node builder
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/imgproc/ImagePyramid_r8ui.lua')


def test_deafaultLevelsValue():

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    loadNodes(session)

    node = session.createContainerNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_gray = memory.createImageViewFromHost(
        np.zeros((1920, 1080), dtype=np.uint8))

    node.bind('in_gray', in_gray)
    node.init()

    out_gray_0 = node.getPort('out_gray_0')
    assert(out_gray_0 is not None)
    assert(out_gray_0.width == in_gray.width)
    assert(out_gray_0.height == in_gray.height)
    assert(out_gray_0.depth == in_gray.depth)
    assert(out_gray_0.channelType == ll.ChannelType.Uint8)
    assert(out_gray_0.channels == 1)

    session.run(node)

    assert(not ll.hasReceivedVulkanWarningMessages())


def test_withSeveralLevels():

    levels = 4

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    loadNodes(session)

    node = session.createContainerNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_gray = memory.createImageViewFromHost(np.zeros((1080, 1920), dtype=np.uint8))

    node.setParameter('levels', ll.Parameter(4))
    node.bind('in_gray', in_gray)
    node.init()

    width = in_gray.width
    height = in_gray.height

    for i in range(levels):

        out_gray_i = node.getPort('out_gray_{0}'.format(i))
        assert(out_gray_i is not None)
        assert(out_gray_i.width == width)
        assert(out_gray_i.height == height)
        assert(out_gray_i.depth == in_gray.depth)
        assert(out_gray_i.channelType == ll.ChannelType.Uint8)
        assert(out_gray_i.channels == 1)

        width = np.floor(width / 2.0)
        height = np.floor(height / 2.0)

    session.run(node)

    assert(not ll.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
