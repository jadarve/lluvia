import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test

def loadNodes(session):

    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/imgproc/ImageDownsampleX_r8ui.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/imgproc/ImageDownsampleX_r8ui.comp.spv',
                     programName='lluvia/imgproc/ImageDownsampleX_r8ui.comp')
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/imgproc/ImageDownsampleY_r8ui.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/imgproc/ImageDownsampleY_r8ui.comp.spv',
                     programName='lluvia/imgproc/ImageDownsampleY_r8ui.comp')
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/imgproc/ImagePyramid_r8ui.lua')
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictX.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictX.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowPredictX.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictY.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictY.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowPredictY.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredict.lua')
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowUpdate.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowUpdate.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowUpdate.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowSmooth.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowSmooth.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowSmooth.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/ImageModel.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/ImageModel.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/ImageModel.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowFilterSimple.lua')
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictPayloadX.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictPayloadX.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowPredictPayloadX.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictPayloadY.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictPayloadY.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowPredictPayloadY.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowPredictPayload.lua')
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowUpdateDelta.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowUpdateDelta.comp.spv',
                     programName='lluvia/opticalflow/flowfilter/FlowUpdateDelta.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowFilterDelta.lua')
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/flowfilter/FlowFilter.lua')


def test_multipleLevels():

    nodeName = 'lluvia/opticalflow/flowfilter/FlowFilter'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    loadNodes(session)

    node = session.createContainerNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_gray = memory.createImageViewFromHost(
        np.zeros((480, 640), dtype=np.uint8))

    node.setParameter('maxflow', ll.Parameter(4))
    # node.setParameter('levels', ll.Parameter(1))
    node.setParameter('smooth_iterations', ll.Parameter(2))
    node.bind('in_gray', in_gray)
    node.init()

    out_flow = node.getPort('out_flow')
    assert(out_flow is not None)
    assert(out_flow.width == in_gray.width)
    assert(out_flow.height == in_gray.height)
    assert(out_flow.depth == in_gray.depth)
    assert(out_flow.channelType == ll.ChannelType.Float32)
    assert(out_flow.channels == 2)

    out_gray = node.getPort('out_gray')
    assert(out_gray is not None)
    assert(out_gray.width == in_gray.width)
    assert(out_gray.height == in_gray.height)
    assert(out_gray.depth == in_gray.depth)
    assert(out_gray.channelType == ll.ChannelType.Float32)
    assert(out_gray.channels == 1)

    session.run(node)

    assert(not ll.hasReceivedVulkanWarningMessages())

def test_multipleLevels():

    nodeName = 'lluvia/opticalflow/flowfilter/FlowFilter'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    loadNodes(session)

    node = session.createContainerNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_gray = memory.createImageViewFromHost(
        np.zeros((480, 640), dtype=np.uint8))

    node.setParameter('maxflow', ll.Parameter(4))
    node.setParameter('levels', ll.Parameter(2))
    node.setParameter('smooth_iterations', ll.Parameter(2))
    node.bind('in_gray', in_gray)
    node.init()

    out_flow = node.getPort('out_flow')
    assert(out_flow is not None)
    assert(out_flow.width == in_gray.width)
    assert(out_flow.height == in_gray.height)
    assert(out_flow.depth == in_gray.depth)
    assert(out_flow.channelType == ll.ChannelType.Float32)
    assert(out_flow.channels == 2)

    out_gray = node.getPort('out_gray')
    assert(out_gray is not None)
    assert(out_gray.width == in_gray.width)
    assert(out_gray.height == in_gray.height)
    assert(out_gray.depth == in_gray.depth)
    assert(out_gray.channelType == ll.ChannelType.Float32)
    assert(out_gray.channels == 1)

    session.run(node)

    assert(not ll.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
