local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'FlowPredictPayloadX'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('FlowPredictPayloadX')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'in_vector', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(3, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(4, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(5, 'out_vector', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('dt', 1.0)

    return desc
end

function builder.onNodeInit(node)

    local dt = node:getParameter('dt')

    ll.logd('FlowPredictPayloadX', 'onNodeInit: dt', dt)

    local pushConstants = ll.PushConstants.new()
    pushConstants.float = dt
    node.pushConstants = pushConstants

    local in_flow = node:getPort('in_flow')
    local in_gray = node:getPort('in_gray')
    local in_vector = node:getPort('in_vector')

    -- ll::Memory where out_flow will be allocated
    local memory = in_flow.memory

    local out_flow = memory:createImageView(in_flow.imageDescriptor, in_flow.descriptor)
    local out_gray = memory:createImageView(in_gray.imageDescriptor, in_gray.descriptor)
    local out_vector = memory:createImageView(in_vector.imageDescriptor, in_vector.descriptor)

    -- need to change image layout before binding
    out_flow:changeImageLayout(ll.ImageLayout.General)
    out_gray:changeImageLayout(ll.ImageLayout.General)
    out_vector:changeImageLayout(ll.ImageLayout.General)

    out_flow:clear()
    out_gray:clear()
    out_vector:clear()

    node:bind('out_flow', out_flow)
    node:bind('out_gray', out_gray)
    node:bind('out_vector', out_vector)

    ll.logd('FlowPredictPayloadX', 'in_flow ', string.format('[%d, %d, %d]', in_flow.width, in_flow.height, in_flow.channelCount)
                              , 'out_flow', string.format('[%d, %d, %d]', out_flow.width, out_flow.height, out_flow.channelCount))

    node:configureGridShape(ll.vec3ui.new(out_flow.width, out_flow.height, 1))

    ll.logd('FlowPredictPayloadX', 'onNodeInit: finish')
end

ll.registerNodeBuilder('FlowPredictPayloadX', builder)
