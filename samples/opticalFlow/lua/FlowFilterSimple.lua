local builder = ll.class(ll.ContainerNodeBuilder)


function builder.newDescriptor()

    ll.logd('FlowFilterSimple', 'newDescriptor')
    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = 'FlowFilterSimple'

    desc:addPort(ll.PortDescriptor.new(0, 'in_rgba', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    -- parameter with default value
    desc:addParameter('predict_iterations', 4)

    return desc
end


function builder.onNodeInit(node)

    ll.logd('FlowFilterSimple', 'onNodeInit')

    predict_iterations = node.descriptor:getParameter('predict_iterations')
    ll.logd('FlowFilterSimple', 'onNodeInit: predict_iterations', predict_iterations)

    in_rgba = node:getPort('in_rgba')

    width = in_rgba.width
    height = in_rgba.height

    ll.logd('FlowFilterSimple', string.format('onNodeInit: input shape: [%d, %d]', width, height))

    RGBA2Gray = ll.createComputeNode('RGBA2Gray')
    RGBA2Gray:bind('in_rgba', in_rgba)
    RGBA2Gray:init()

    imageModel = ll.createComputeNode('ImageModel')
    imageModel:bind('in_gray', RGBA2Gray:getPort('out_gray'))
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
    -- predictor:setParameter('iterations', predict_iterations) -- FIXME: not possible to do!!!
    predictor:bind('in_flow', predict_inflow)
    predictor:init()

    update = ll.createComputeNode('FlowUpdate')
    update:bind('in_gray', imageModel:getPort('out_gray'))
    update:bind('in_gradient', imageModel:getPort('out_gradient'))
    update:bind('in_flow', predictor:getPort('out_flow'))
    update:bind('out_flow', predict_inflow)
    update:init()

    node:bindNode('RGBA2Gray', RGBA2Gray)
    node:bindNode('ImageModel', imageModel)
    node:bindNode('Predictor', predictor)
    node:bindNode('Update', update)

    node:bind('out_flow', update:getPort('out_flow'))
    node:bind('out_gray', update:getPort('out_gray'))

    ll.logd('FlowFilterSimple', 'onNodeInit: finish')

end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd('FlowFilterSimple', 'onNodeRecord')

    RGBA2Gray = node:getNode('RGBA2Gray')
    imageModel = node:getNode('ImageModel')
    predictor = node:getNode('Predictor')
    update = node:getNode('Update')

    cmdBuffer:run(RGBA2Gray)
    cmdBuffer:memoryBarrier()
    
    -- the image model and the predictor can run concurrently
    cmdBuffer:run(imageModel)
    predictor:record(cmdBuffer)
    -- cmdBuffer:run(predictor)
    cmdBuffer:memoryBarrier()

    cmdBuffer:run(update)
    cmdBuffer:memoryBarrier()
    
    ll.logd('FlowFilterSimple', 'onNodeRecord: finish')
end


ll.registerNodeBuilder('FlowFilterSimple', builder)
