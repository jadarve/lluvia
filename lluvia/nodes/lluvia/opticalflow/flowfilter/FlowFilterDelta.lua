local builder = ll.class(ll.ContainerNodeBuilder)

builder.name = 'lluvia/opticalflow/flowfilter/FlowFilterDelta'
builder.doc = [[
An intermediate pyramid-level filter for computing optical flow.

The in_gray input is the gray-scale image at this particular pyramid level. in_flow
corresponds to the estimated flow from a higher pyramid level, and its resolution is
half of that of in_gray.

The output out_flow is the estimated optical flow for this level. It refines the
information in in_flow using higher resolution data from this level.

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

in_flow : ImageView
    rg32f image. The input optical flow.

Outputs
-------
out_gray : ImageView
    r32f image. The gray-scale image after one iteration of the algorithm.

out_flow : ImageView
    rg32f image. The estimated optical flow.

out_delta_flow : ImageView
    rg32f image. The estimated optical flow.

]]

function builder.newDescriptor()

    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = builder.name

    desc:addPort(ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView))

    desc:addPort(ll.PortDescriptor.new(2, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(3, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(4, 'out_delta_flow', ll.PortDirection.Out, ll.PortType.ImageView))

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
    local in_flow = node:getPort('in_flow')

    local width = in_gray.width
    local height = in_gray.height

    local memory = in_gray.memory

    ll.logd(node.descriptor.builderName, string.format('onNodeInit: input shape: [%d, %d]', width, height))

    -- create SampledImageView from in_flow
    -- use normalized coordinates and sampling
    local in_flow_view = in_flow.image:createImageView(
        ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Linear, true, true))


    local imageModel = ll.createComputeNode('lluvia/opticalflow/flowfilter/ImageModel')
    imageModel:bind('in_gray', in_gray)
    imageModel:init()

    
    local predict_in_flow = memory:createImageView(
        ll.ImageDescriptor.new(1, height, width, ll.ChannelCount.C2, ll.ChannelType.Float32),
        ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false))
    predict_in_flow:changeImageLayout(ll.ImageLayout.General)
    predict_in_flow:clear()
        
    local predict_in_delta_flow = memory:createImageView(
        ll.ImageDescriptor.new(1, height, width, ll.ChannelCount.C2, ll.ChannelType.Float32),
        ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false))
    predict_in_delta_flow:changeImageLayout(ll.ImageLayout.General)
    predict_in_delta_flow:clear()
    
    local predict_in_gray = memory:createImageView(
        ll.ImageDescriptor.new(1, height, width, ll.ChannelCount.C1, ll.ChannelType.Float32),
        ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false))
    predict_in_gray:changeImageLayout(ll.ImageLayout.General)
    predict_in_gray:clear()
    

    local predictor = ll.createContainerNode('lluvia/opticalflow/flowfilter/FlowPredictPayload')
    predictor:setParameter('max_flow', max_flow)
    predictor:bind('in_flow', predict_in_flow)
    predictor:bind('in_gray', predict_in_gray)
    predictor:bind('in_vector', predict_in_delta_flow)
    predictor:init()

    local update = ll.createComputeNode('lluvia/opticalflow/flowfilter/FlowUpdateDelta')
    update:setParameter('gamma', gamma)
    update:setParameter('max_flow', max_flow)
    update:bind('in_gray', imageModel:getPort('out_gray'))
    update:bind('in_gradient', imageModel:getPort('out_gradient'))
    update:bind('in_delta_flow', predictor:getPort('out_vector'))
    update:bind('in_gray_old', predictor:getPort('out_gray'))
    update:bind('in_flow', in_flow_view)
    update:bind('out_delta_flow', predict_in_delta_flow)
    update:bind('out_gray', predict_in_gray)
    update:init()

    local smooth_in_flow = update:getPort('out_flow')
    for i = 1, smooth_iterations do

        local flowSmooth = ll.createComputeNode('lluvia/opticalflow/flowfilter/FlowSmooth')
        flowSmooth:bind('in_flow', smooth_in_flow)
        
        if i == smooth_iterations then
            flowSmooth:setParameter('allocate_output', 0)
            flowSmooth:bind('out_flow', predict_in_flow)
        end

        flowSmooth:init()
        node:bindNode(string.format('flowSmooth_%d', i), flowSmooth)
        smooth_in_flow = flowSmooth:getPort('out_flow')
    end

    node:bindNode('ImageModel', imageModel)
    node:bindNode('Predictor', predictor)
    node:bindNode('Update', update)

    node:bind('out_delta_flow', update:getPort('out_delta_flow'))
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
