local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'FlowUpdateDelta'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('FlowUpdateDelta')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'in_gradient', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'in_delta_flow', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(3, 'in_gray_old', ll.PortDirection.In, ll.PortType.ImageView))

    desc:addPort(ll.PortDescriptor.new(4, 'in_flow', ll.PortDirection.In, ll.PortType.SampledImageView))

    desc:addPort(ll.PortDescriptor.new(5, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(6, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(7, 'out_delta_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('gamma', 0.01)
    desc:setParameter('maxflow', 1.0)

    -- controls the allocation of out_gray and out_delta_flow
    desc:setParameter('allocate_output', 1)
    
    return desc
end

function builder.onNodeInit(node)
    ll.logd('FlowUpdateDelta', 'onNodeInit')

    gamma = node:getParameter('gamma')
    maxflow = node:getParameter('maxflow')
    allocate_output = node:getParameter('allocate_output')

    pushConstants = ll.PushConstants.new()
    pushConstants:pushFloat(gamma)
    pushConstants:pushFloat(maxflow)

    in_flow = node:getPort('in_flow')
    in_gray = node:getPort('in_gray')

    -- ll::Memory where out_flow will be allocated
    memory = in_flow.memory

    outFlowImgDesc = ll.ImageDescriptor.new(in_flow.imageDescriptor)
    outFlowImgDesc.width = in_gray.width
    outFlowImgDesc.height = in_gray.height

    outFlowViewDesc = ll.ImageViewDescriptor.new()
    outFlowViewDesc:setAddressMode(ll.ImageAddressMode.MirroredRepeat)
    outFlowViewDesc.filterMode = ll.ImageFilterMode.Nearest
    outFlowViewDesc.isSampled = false
    outFlowViewDesc.normalizedCoordinates = false

    out_flow = memory:createImageView(outFlowImgDesc, outFlowViewDesc)
    out_flow:changeImageLayout(ll.ImageLayout.General)
    node:bind('out_flow', out_flow)

    if allocate_output ~= 0 then

        out_gray = memory:createImageView(in_gray.imageDescriptor, in_gray.descriptor)
        out_delta_flow = memory:createImageView(outFlowImgDesc, outFlowViewDesc)

        out_gray:changeImageLayout(ll.ImageLayout.General)
        out_delta_flow:changeImageLayout(ll.ImageLayout.General)

        node:bind('out_gray', out_gray)
        node:bind('out_delta_flow', out_delta_flow)
    end

    out_delta_flow = node:getPort('out_delta_flow')
    ll.logd('FlowUpdateDelta', 'in_flow ', string.format('[%d, %d, %d]', in_flow.width, in_flow.height, in_flow.channelCount)
                             , 'out_flow', string.format('[%d, %d, %d]', out_flow.width, out_flow.height, out_flow.channelCount)
                             , 'out_delta_flow', string.format('[%d, %d, %d]', out_delta_flow.width, out_delta_flow.height, out_delta_flow.channelCount))
    

    node.pushConstants = pushConstants
    node:configureGridShape(ll.vec3ui.new(out_flow.width, out_flow.height, 1))

    -- clear outputs
    node:getPort('out_gray'):clear()
    node:getPort('out_flow'):clear()
    node:getPort('out_delta_flow'):clear()

    ll.logd('FlowUpdateDelta', 'onNodeInit: finish')
end

ll.registerNodeBuilder('FlowUpdateDelta', builder)
