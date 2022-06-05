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
                     builderPath='lluvia/lluvia/nodes/lluvia/opticalflow/common/ImageModel.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/opticalflow/common/ImageModel.comp.spv',
                     programName='lluvia/opticalflow/common/ImageModel.comp'
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


def runTest(levels, precision, channelType):

    nodeName = 'lluvia/opticalflow/flowfilter/FlowFilter'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    loadNodes(session)

    node = session.createContainerNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_gray = memory.createImageViewFromHost(np.zeros((480, 640), dtype=np.uint8))

    node.setParameter('levels', ll.Parameter(levels))
    node.setParameter('maxflow', ll.Parameter(4))
    node.setParameter('smooth_iterations', ll.Parameter(2))
    node.setParameter('float_precision', ll.Parameter(precision.value))
    node.bind('in_gray', in_gray)
    node.init()

    out_flow = node.getPort('out_flow')
    assert(out_flow is not None)
    assert(out_flow.width == in_gray.width)
    assert(out_flow.height == in_gray.height)
    assert(out_flow.depth == in_gray.depth)
    assert(out_flow.channelType == channelType)
    assert(out_flow.channels == 2)

    out_gray = node.getPort('out_gray')
    assert(out_gray is not None)
    assert(out_gray.width == in_gray.width)
    assert(out_gray.height == in_gray.height)
    assert(out_gray.depth == in_gray.depth)
    assert(out_gray.channelType == channelType)
    assert(out_gray.channels == 1)

    session.run(node)

    assert(not session.hasReceivedVulkanWarningMessages())



def test_singleLevel():

    runTest(1, ll.FloatPrecision.FP32, ll.ChannelType.Float32)


def test_singleLevelFloat16():

    runTest(1, ll.FloatPrecision.FP16, ll.ChannelType.Float16)


def test_multipleLevels():

    runTest(3, ll.FloatPrecision.FP32, ll.ChannelType.Float32)


def test_multipleLevelsFloat16():

    runTest(3, ll.FloatPrecision.FP16, ll.ChannelType.Float16)

if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
