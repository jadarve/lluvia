local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'imageDownsampleX'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('imageDownsampleX')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_RGBA', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_RGBA', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)

    in_RGBA = ll.castObject(node:getPort('in_RGBA'))

    outWidth = in_RGBA.width // 2

    -- ll::Memory where out_RGBA will be allocated
    memory = in_RGBA.memory

    -- out_RGBA descriptors
    imgDesc = in_RGBA.imageDescriptor
    imgDesc.width = outWidth

    imgViewDesc = in_RGBA.imageViewDescriptor

    out_RGBA = memory:createImageView(imgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_RGBA:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_RGBA', out_RGBA)
    node:configureGridShape(ll.vec3ui.new(outWidth, in_RGBA.height, 1))
end

ll.registerNodeBuilder('imageDownsampleX', builder)
