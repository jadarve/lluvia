local builder = ll.class(ll.ContainerNodeBuilder)


function builder.newDescriptor()

    ll.logd('FlowFilterDelta', 'newDescriptor')
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

    gamma = node:getParameter('gamma')
    max_flow = node:getParameter('max_flow')
    smooth_iterations = node:getParameter('smooth_iterations')

    ll.logd('FlowFilterDelta', 'onNodeInit: max_flow', max_flow)

    in_gray = node:getPort('in_gray')
    in_flow = node:getPort('in_flow')

    width = in_gray.width
    height = in_gray.height

    ll.logd('FlowFilterDelta', string.format('onNodeInit: input shape: [%d, %d]', width, height))

    -- create SampledImageView from in_flow
    -- TODO: constructor will all common arguments
    inFlowSampledDesc = ll.ImageViewDescriptor.new()
    inFlowSampledDesc:setAddressMode(ll.ImageAddressMode.MirroredRepeat)
    inFlowSampledDesc.filterMode = ll.ImageFilterMode.Linear
    inFlowSampledDesc.isSampled = true
    inFlowSampledDesc.normalizedCoordinates = true

    in_flow_view = in_flow.image:createImageView(inFlowSampledDesc)


    imageModel = ll.createComputeNode('ImageModel')
    imageModel:bind('in_gray', in_gray)
    imageModel:init()

    out_gradient = imageModel:getPort('out_gradient')
    ll.logd('FlowFilterDelta', 'out_gradient', out_gradient.descriptor.isSampled)


    memory = imageModel:getPort('out_gradient').memory

    -------------------------------------------------------
    -- HACK!!! pass in_rgba.imageDescriptor so that the usage flags of predict_inflow are set
    predictInflowImgDesc = ll.ImageDescriptor.new(in_rgba.imageDescriptor)
    predictInflowImgDesc.width = width
    predictInflowImgDesc.height = height
    predictInflowImgDesc.depth = 1
    predictInflowImgDesc.channelCount = 2
    predictInflowImgDesc.channelType = ll.ChannelType.Float32

    predictInflowViewDesc = ll.ImageViewDescriptor.new()
    predictInflowViewDesc:setAddressMode(ll.ImageAddressMode.MirroredRepeat)
    predictInflowViewDesc.filterMode = ll.ImageFilterMode.Nearest
    predictInflowViewDesc.isSampled = false
    predictInflowViewDesc.normalizedCoordinates = false

    predict_in_flow = memory:createImageView(predictInflowImgDesc, predictInflowViewDesc)
    predict_in_flow:changeImageLayout(ll.ImageLayout.General)
    -------------------------------------------------------

    -------------------------------------------------------
    -- I can recycle the descriptors
    predict_in_delta_flow = memory:createImageView(predictInflowImgDesc, predictInflowViewDesc)
    predict_in_delta_flow:changeImageLayout(ll.ImageLayout.General)
    -------------------------------------------------------

    -------------------------------------------------------
    -- HACK!!! pass in_rgba.imageDescriptor so that the usage flags of predict_in_flow are set
    predictInGrayImgDesc = ll.ImageDescriptor.new(in_rgba.imageDescriptor)
    predictInGrayImgDesc.width = width
    predictInGrayImgDesc.height = height
    predictInGrayImgDesc.depth = 1
    predictInGrayImgDesc.channelCount = 1
    predictInGrayImgDesc.channelType = ll.ChannelType.Float32

    predictInGrayViewDesc = ll.ImageViewDescriptor.new()
    predictInGrayViewDesc:setAddressMode(ll.ImageAddressMode.MirroredRepeat)
    predictInGrayViewDesc.filterMode = ll.ImageFilterMode.Nearest
    predictInGrayViewDesc.isSampled = false
    predictInGrayViewDesc.normalizedCoordinates = false

    predict_in_gray = memory:createImageView(predictInGrayImgDesc, predictInGrayViewDesc)
    predict_in_gray:changeImageLayout(ll.ImageLayout.General)
    -------------------------------------------------------


    predictor = ll.createContainerNode('FlowPredictPayload')
    predictor:setParameter('max_flow', max_flow)
    predictor:bind('in_flow', predict_in_flow)
    predictor:bind('in_gray', predict_in_gray)
    predictor:bind('in_vector', predict_in_delta_flow)
    predictor:init()

    update = ll.createComputeNode('FlowUpdateDelta')
    update:setParameter('gamma', gamma)
    update:setParameter('max_flow', max_flow)
    update:setParameter('allocate_output', 0)
    update:bind('in_gray', imageModel:getPort('out_gray'))
    update:bind('in_gradient', imageModel:getPort('out_gradient'))
    update:bind('in_delta_flow', predictor:getPort('out_vector'))
    update:bind('in_gray_old', predictor:getPort('out_gray'))
    update:bind('in_flow', in_flow_view)
    update:bind('out_delta_flow', predict_in_delta_flow)
    update:bind('out_gray', predict_in_gray)
    update:init()

    smooth_in_flow = update:getPort('out_flow')
    for i = 1, smooth_iterations do

        flowSmooth = ll.createComputeNode('FlowSmooth')
        flowSmooth:bind('in_flow', smooth_in_flow)
        
        if i == smooth_iterations then
            flowSmooth:setParameter('allocate_output', false)
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

    imageModel = node:getNode('ImageModel')
    predictor = node:getNode('Predictor')
    update = node:getNode('Update')
    
    -- the image model and the predictor can run concurrently
    cmdBuffer:run(imageModel)
    predictor:record(cmdBuffer)
    cmdBuffer:memoryBarrier()

    cmdBuffer:run(update)
    cmdBuffer:memoryBarrier()

    smooth_iterations = node:getParameter('smooth_iterations')
    for i = 1, smooth_iterations do
        flowSmooth = node:getNode(string.format('flowSmooth_%d', i))
        cmdBuffer:run(flowSmooth)
        cmdBuffer:memoryBarrier()
    end
    
    ll.logd('FlowFilterDelta', 'onNodeRecord: finish')
end


ll.registerNodeBuilder('FlowFilterDelta', builder)
