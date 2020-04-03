local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'ImageDownsampleX_8ui'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('ImageDownsampleX_8ui')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)

    local in_gray = node:getPort('in_gray')

    -- ll::Memory where out_gray will be allocated
    local memory = in_gray.memory

    -- out_gray descriptors
    local imgDesc = ll.ImageDescriptor.new(in_gray.imageDescriptor)
    imgDesc.width = in_gray.width // 2
    
    local out_gray = memory:createImageView(imgDesc, in_gray.descriptor)

    -- need to change image layout before binding
    out_gray:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_gray', out_gray)

    ll.logd('ImageDownsampleX_8ui', 'in_gray ', string.format('[%d, %d, %d]', in_gray.width, in_gray.height, in_gray.channelCount)
                              , 'out_gray', string.format('[%d, %d, %d]', out_gray.width, out_gray.height, out_gray.channelCount))

    node:configureGridShape(ll.vec3ui.new(out_gray.width, out_gray.height, 1))
end

ll.registerNodeBuilder('ImageDownsampleX_8ui', builder)
