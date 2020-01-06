local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'ImageDownsampleX_gray'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('ImageDownsampleX_gray')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)

    in_gray = node:getPort('in_gray')

    -- ll::Memory where out_gray will be allocated
    memory = in_gray.memory

    -- out_gray descriptors
    imgDesc = ll.ImageDescriptor.new(in_gray.imageDescriptor)
    imgDesc.width = in_gray.width // 2

    imgViewDesc = ll.ImageViewDescriptor.new(in_gray.descriptor)
    
    out_gray = memory:createImageView(imgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_gray:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_gray', out_gray)

    ll.logd('ImageDownsampleX_gray', 'in_gray ', string.format('[%d, %d]', in_gray.width, in_gray.height)
                              , 'out_gray', string.format('[%d, %d]', out_gray.width, out_gray.height))

    node:configureGridShape(ll.vec3ui.new(out_gray.width, out_gray.height, 1))
end

ll.registerNodeBuilder('ImageDownsampleX_gray', builder)
