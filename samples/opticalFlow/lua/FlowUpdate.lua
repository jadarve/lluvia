local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'FlowUpdate'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('FlowUpdate')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'in_gradient', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView))

    desc:addPort(ll.PortDescriptor.new(3, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    
    -- out_flow is initialized externally in FlowFilterSimple and bound to this node. This way,
    -- the loop between FlowPredict and FlowUpdate can be broken.
    desc:addPort(ll.PortDescriptor.new(4, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('gamma', 0.01)
    desc:setParameter('maxflow', 1.0)
    
    return desc
end

function builder.onNodeInit(node)
    ll.logd('FlowUpdate', 'onNodeInit')

    gamma = node:getParameter('gamma')
    maxflow = node:getParameter('maxflow')

    pushConstants = ll.PushConstants.new()
    pushConstants:pushFloat(gamma)
    pushConstants:pushFloat(maxflow)

    in_flow = node:getPort('in_flow')
    in_gray = node:getPort('in_gray')

    -- ll::Memory where out_flow will be allocated
    memory = in_flow.memory

    out_gray = memory:createImageView(in_gray.imageDescriptor, in_gray.descriptor)
    out_flow = memory:createImageView(in_flow.imageDescriptor, in_flow.descriptor)

    -- need to change image layout before binding
    out_gray:changeImageLayout(ll.ImageLayout.General)
    out_flow:changeImageLayout(ll.ImageLayout.General)

    -- node:bind('in_gray_old', in_gray_old)
    node:bind('out_gray', out_gray)
    node:bind('out_flow', out_flow)
    node.pushConstants = pushConstants

    ll.logd('FlowUpdate', 'in_gray ', string.format('[%d, %d, %d]', in_gray.width, in_gray.height, in_gray.channelCount)
                              , 'out_gray', string.format('[%d, %d, %d]', out_gray.width, out_gray.height, out_gray.channelCount))
    ll.logd('FlowUpdate', 'in_flow ', string.format('[%d, %d, %d]', in_flow.width, in_flow.height, in_flow.channelCount)
                              , 'out_flow', string.format('[%d, %d, %d]', out_flow.width, out_flow.height, out_flow.channelCount))
    
    node:configureGridShape(ll.vec3ui.new(out_flow.width, out_flow.height, 1))

    -- clear outputs
    node:getPort('out_gray'):clear()
    node:getPort('out_flow'):clear()

    ll.logd('FlowUpdate', 'onNodeInit: finish')
end

ll.registerNodeBuilder('FlowUpdate', builder)
