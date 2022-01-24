local builder = ll.class(ll.ContainerNodeBuilder)

builder.name = 'lluvia/opticalflow/flowfilter/FlowFilterSimple'
builder.doc = [[
A single pyramid level version of the filtering algorithm for computing optical flow.

This node can be used as a stand-alone algorithm when the optical flow magnitude is
expected to be small (~2pix). Otherwise, it is recommended to use the FlowFilter node
for a pyramidal version of the algorithm.

Parameters
----------
gamma : float. Defaults to 0.01
    The filter gains for the update step.

max_flow : float. Defaults to 1.0
    The max magnitude allowed for the optical flow output.

smooth_iterations : int. Defaults to 1.
    The number of smooth iterations to apply to the estimated flow.

Inputs
------
in_gray : ImageView
    r8ui image. The input gray-scale image.

Outputs
-------
out_gray : ImageView
    r32f image. The gray-scale image after one iteration of the algorithm.

out_flow : ImageView
    rg32f image. The estimated optical flow.

]]

function builder.newDescriptor()

    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = builder.name

    desc:addPort(ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    -- parameter with default value
    desc:setParameter('max_flow', 1)
    desc:setParameter('gamma', 0.01)
    desc:setParameter('smooth_iterations', 1)

    return desc
end


function builder.onNodeInit(node)

    ll.logd(node.descriptor.builderName, 'onNodeInit')

    local gamma = node:getParameter('gamma')
    local max_flow = node:getParameter('max_flow')
    local smooth_iterations = node:getParameter('smooth_iterations')

    ll.logd(node.descriptor.builderName, 'onNodeInit: max_flow', max_flow)

    local in_gray = node:getPort('in_gray')

    local width = in_gray.width
    local height = in_gray.height

    ll.logd(node.descriptor.builderName, string.format('onNodeInit: input shape: [%d, %d]', width, height))

    local imageModel = ll.createComputeNode('lluvia/opticalflow/flowfilter/ImageModel')
    imageModel:bind('in_gray', in_gray)
    imageModel:init()


    local memory = imageModel:getPort('out_gradient').memory

    -- local inflowImgDesc  = ll.ImageDescriptor.new(1, height, width, ll.ChannelCount.C2, ll.ChannelType.Float32)
    -- local inflowViewDesc = ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false)

    local predict_inflow = memory:createImageView(
        ll.ImageDescriptor.new(1, height, width, ll.ChannelCount.C2, ll.ChannelType.Float32),
        ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false))
    predict_inflow:changeImageLayout(ll.ImageLayout.General)

    local predictor = ll.createContainerNode('lluvia/opticalflow/flowfilter/FlowPredict')
    predictor:setParameter('max_flow', max_flow)
    predictor:bind('in_flow', predict_inflow)
    predictor:init()

    local update = ll.createComputeNode('lluvia/opticalflow/flowfilter/FlowUpdate')
    update:setParameter('gamma', gamma)
    update:setParameter('max_flow', max_flow)
    update:bind('in_gray', imageModel:getPort('out_gray'))
    update:bind('in_gradient', imageModel:getPort('out_gradient'))
    update:bind('in_flow', predictor:getPort('out_flow'))
    update:init()

    local smooth_in_flow = update:getPort('out_flow')
    for i = 1, smooth_iterations do

        local flowSmooth = ll.createComputeNode('lluvia/opticalflow/flowfilter/FlowSmooth')
        flowSmooth:bind('in_flow', smooth_in_flow)
        
        if i == smooth_iterations then
            flowSmooth:setParameter('allocate_output', 0)
            flowSmooth:bind('out_flow', predict_inflow)
        end

        flowSmooth:init()
        node:bindNode(string.format('flowSmooth_%d', i), flowSmooth)
        smooth_in_flow = flowSmooth:getPort('out_flow')
    end

    node:bindNode('ImageModel', imageModel)
    node:bindNode('Predictor', predictor)
    node:bindNode('Update', update)

    node:bind('out_flow', smooth_in_flow)
    node:bind('out_gray', update:getPort('out_gray'))

    ll.logd(node.descriptor.builderName, 'onNodeInit: finish')

end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd(node.descriptor.builderName, 'onNodeRecord')

    local imageModel = node:getNode('ImageModel')
    local predictor = node:getNode('Predictor')
    local update = node:getNode('Update')
    
    -- the image model and the predictor can run concurrently
    cmdBuffer:run(imageModel)
    predictor:record(cmdBuffer)
    cmdBuffer:memoryBarrier()

    cmdBuffer:run(update)
    cmdBuffer:memoryBarrier()

    local smooth_iterations = node:getParameter('smooth_iterations')
    for i = 1, smooth_iterations do
        local flowSmooth = node:getNode(string.format('flowSmooth_%d', i))
        cmdBuffer:run(flowSmooth)
        cmdBuffer:memoryBarrier()
    end
    
    ll.logd(node.descriptor.builderName, 'onNodeRecord: finish')
end


ll.registerNodeBuilder(builder)
