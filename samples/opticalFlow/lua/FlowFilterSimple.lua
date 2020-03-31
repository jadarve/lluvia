local builder = ll.class(ll.ContainerNodeBuilder)


function builder.newDescriptor()

    ll.logd('FlowFilterSimple', 'newDescriptor')
    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = 'FlowFilterSimple'

    desc:addPort(ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    -- parameter with default value
    desc:setParameter('max_flow', 4)
    desc:setParameter('gamma', 0.01)
    desc:setParameter('smooth_iterations', 1)

    return desc
end


function builder.onNodeInit(node)

    ll.logd('FlowFilterSimple', 'onNodeInit')

    gamma = node:getParameter('gamma')
    max_flow = node:getParameter('max_flow')
    smooth_iterations = node:getParameter('smooth_iterations')

    ll.logd('FlowFilterSimple', 'onNodeInit: max_flow', max_flow)

    in_gray = node:getPort('in_gray')

    width = in_gray.width
    height = in_gray.height

    ll.logd('FlowFilterSimple', string.format('onNodeInit: input shape: [%d, %d]', width, height))

    imageModel = ll.createComputeNode('ImageModel')
    imageModel:bind('in_gray', in_gray)
    imageModel:init()


    memory = imageModel:getPort('out_gradient').memory

    -- HACK!!! pass in_rgba.imageDescriptor so that the usage flags of predict_inflow are set
    inflowImgDesc = ll.ImageDescriptor.new(in_rgba.imageDescriptor)
    inflowImgDesc.width = width
    inflowImgDesc.height = height
    inflowImgDesc.depth = 1
    inflowImgDesc.channelCount = 2
    inflowImgDesc.channelType = ll.ChannelType.Float32
    -- FIXME: usage flags

    inflowViewDesc = ll.ImageViewDescriptor.new()
    inflowViewDesc:setAddressMode(ll.ImageAddressMode.MirroredRepeat)
    inflowViewDesc.filterMode = ll.ImageFilterMode.Nearest
    inflowViewDesc.isSampled = false
    inflowViewDesc.normalizedCoordinates = false

    predict_inflow = memory:createImageView(inflowImgDesc, inflowViewDesc)
    predict_inflow:changeImageLayout(ll.ImageLayout.General)

    predictor = ll.createContainerNode('FlowPredict')
    predictor:setParameter('max_flow', max_flow)
    predictor:bind('in_flow', predict_inflow)
    predictor:init()

    update = ll.createComputeNode('FlowUpdate')
    update:setParameter('gamma', gamma)
    update:setParameter('max_flow', max_flow)
    update:bind('in_gray', imageModel:getPort('out_gray'))
    update:bind('in_gradient', imageModel:getPort('out_gradient'))
    update:bind('in_flow', predictor:getPort('out_flow'))
    -- update:bind('out_flow', predict_inflow)
    update:init()

    smooth_in_flow = update:getPort('out_flow')
    for i = 1, smooth_iterations do

        flowSmooth = ll.createComputeNode('FlowSmooth')
        flowSmooth:bind('in_flow', smooth_in_flow)
        
        if i == smooth_iterations then
            flowSmooth:setParameter('allocate_output', false)
            flowSmooth:bind('out_flow', predict_inflow)
        end

        flowSmooth:init()
        node:bindNode(string.format('flowSmooth_%d', i), flowSmooth)
        smooth_in_flow = flowSmooth:getPort('out_flow')
    end

    node:bindNode('ImageModel', imageModel)
    node:bindNode('Predictor', predictor)
    node:bindNode('Update', update)

    -- node:bind('out_flow', update:getPort('out_flow'))
    node:bind('out_flow', smooth_in_flow)
    node:bind('out_gray', update:getPort('out_gray'))

    ll.logd('FlowFilterSimple', 'onNodeInit: finish')

end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd('FlowFilterSimple', 'onNodeRecord')

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
    
    ll.logd('FlowFilterSimple', 'onNodeRecord: finish')
end


ll.registerNodeBuilder('FlowFilterSimple', builder)
