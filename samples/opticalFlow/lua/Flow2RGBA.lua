local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'Flow2RGBA'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('Flow2RGBA')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_rgba', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('maxflow', 1.0)

    return desc
end

function builder.onNodeInit(node)
    
    local in_flow = node:getPort('in_flow')

    local pushConstants = ll.PushConstants.new()
    pushConstants:pushFloat(node:getParameter('maxflow'))
    node.pushConstants = pushConstants

    local memory = in_flow.memory

    out_rgba = memory:createImageView(
        ll.ImageDescriptor.new(1, in_flow.height, in_flow.width, ll.ChannelCount.C4, ll.ChannelType.Uint8),
        ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false))

    out_rgba:changeImageLayout(ll.ImageLayout.General)
    out_rgba:clear()

    node:bind('out_rgba', out_rgba)
    node:configureGridShape(ll.vec3ui.new(in_flow.width, in_flow.height, 1))
end

ll.registerNodeBuilder('Flow2RGBA', builder)
