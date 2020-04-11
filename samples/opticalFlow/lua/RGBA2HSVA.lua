local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'RGBA2HSVA'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('RGBA2HSVA')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_rgba', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_hsva', ll.PortDirection.Out, ll.PortType.ImageView))

    -- minimum chroma value in the range [0, 1]
    desc:setParameter('min_chroma', 0.05)

    return desc
end

function builder.onNodeInit(node)
    
    local in_rgba = node:getPort('in_rgba')
    local memory = in_rgba.memory

    local pushConstants = ll.PushConstants.new()
    pushConstants:pushFloat(node:getParameter('min_chroma'))
    node.pushConstants = pushConstants

    local out_hsva = memory:createImageView(
        ll.ImageDescriptor.new(1, in_rgba.height, in_rgba.width, ll.ChannelCount.C4, ll.ChannelType.Float32),
        ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false))

    out_hsva:changeImageLayout(ll.ImageLayout.General)
    out_hsva:clear()

    node:bind('out_hsva', out_hsva)
    node:configureGridShape(ll.vec3ui.new(out_hsva.width, out_hsva.height, 1))
end

ll.registerNodeBuilder('RGBA2HSVA', builder)
