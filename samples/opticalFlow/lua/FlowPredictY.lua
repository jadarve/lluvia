local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'FlowPredictY'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('FlowPredictY')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('dt', 1.0)

    return desc
end

function builder.onNodeInit(node)
    
    local dt = node:getParameter('dt')
    ll.logd('FlowPredictY', 'onNodeInit: dt', dt)

    local pushConstants = ll.PushConstants.new()
    pushConstants.float = dt
    node.pushConstants = pushConstants


    local in_flow = node:getPort('in_flow')

    -- ll::Memory where out_flow will be allocated
    local memory = in_flow.memory

    local out_flow = memory:createImageView(in_flow.imageDescriptor, in_flow.descriptor)

    -- need to change image layout before binding
    out_flow:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_flow', out_flow)

    ll.logd('FlowPredictY', 'in_flow ', string.format('[%d, %d, %d]', in_flow.width, in_flow.height, in_flow.channelCount)
                              , 'out_flow', string.format('[%d, %d, %d]', out_flow.width, out_flow.height, out_flow.channelCount))

    node:configureGridShape(ll.vec3ui.new(out_flow.width, out_flow.height, 1))

    ll.logd('FlowPredictY', 'onNodeInit: finish')
end

ll.registerNodeBuilder('FlowPredictY', builder)
