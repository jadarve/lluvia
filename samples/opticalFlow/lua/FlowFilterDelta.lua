local builder = ll.class(ll.ContainerNodeBuilder)


function builder.newDescriptor()

    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = 'FlowFilterDelta'

    desc:addPort(ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView))

    desc:addPort(ll.PortDescriptor.new(2, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(3, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(4, 'out_delta_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    -- parameter with default value
    desc:setParameter('max_flow', 4)
    desc:setParameter('gamma', 0.01)
    desc:setParameter('smooth_iterations', 1)

    return desc
end


function builder.onNodeInit(node)

    ll.logd('FlowFilterDelta', 'onNodeInit')

    local gamma = node:getParameter('gamma')
    local max_flow = node:getParameter('max_flow')
    local smooth_iterations = node:getParameter('smooth_iterations')

    ll.logd('FlowFilterDelta', 'onNodeInit: max_flow', max_flow)

    local in_gray = node:getPort('in_gray')
    local in_flow = node:getPort('in_flow')

    local width = in_gray.width
    local height = in_gray.height

    local memory = in_gray.memory

    ll.logd('FlowFilterDelta', string.format('onNodeInit: input shape: [%d, %d]', width, height))

    -- create SampledImageView from in_flow
    -- use normalized coordinates and sampling
    local in_flow_view = in_flow.image:createImageView(
        ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Linear, true, true))


    local imageModel = ll.createComputeNode('ImageModel')
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
    

    local predictor = ll.createContainerNode('FlowPredictPayload')
    predictor:setParameter('max_flow', max_flow)
    predictor:bind('in_flow', predict_in_flow)
    predictor:bind('in_gray', predict_in_gray)
    predictor:bind('in_vector', predict_in_delta_flow)
    predictor:init()

    local update = ll.createComputeNode('FlowUpdateDelta')
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

        local flowSmooth = ll.createComputeNode('FlowSmooth')
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

    ll.logd('FlowFilterDelta', 'onNodeInit: finish')

end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd('FlowFilterDelta', 'onNodeRecord')

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
    
    ll.logd('FlowFilterDelta', 'onNodeRecord: finish')
end


ll.registerNodeBuilder('FlowFilterDelta', builder)
