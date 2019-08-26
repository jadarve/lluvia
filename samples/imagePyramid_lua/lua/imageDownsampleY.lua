local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'imageDownsampleY'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('imageDownsampleY')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_RGBA', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_RGBA', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)

    -- in_RGBA = ll.castObject(node:getPort('in_RGBA'))
    in_RGBA = node:getPort('in_RGBA')

    outHeight = in_RGBA.height // 2

    -- ll::Memory where out_RGBA will be allocated
    memory = in_RGBA.memory

    -- out_RGBA descriptors
    imgDesc = ll.ImageDescriptor.new(in_RGBA.imageDescriptor)
    imgDesc.height = outHeight

    imgViewDesc = ll.ImageViewDescriptor.new(in_RGBA.descriptor)
    
    out_RGBA = memory:createImageView(imgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_RGBA:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_RGBA', out_RGBA)
    -- ll.bindImageView(node, 'out_RGBA', out_RGBA)

    ll.logd('imageDownsampleY', 'in_RGBA ', string.format('[%d, %d]', in_RGBA.width, in_RGBA.height)
                              , 'out_RGBA', string.format('[%d, %d]', out_RGBA.width, out_RGBA.height))
    
    node:configureGridShape(ll.vec3ui.new(out_RGBA.width, out_RGBA.height, 1))
end

ll.registerNodeBuilder('imageDownsampleY', builder)
