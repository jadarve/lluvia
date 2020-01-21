local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'RGBA2Gray'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('RGBA2Gray')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_rgba', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)
    
    in_RGBA = node:getPort('in_rgba')

    -- ll::Memory where out_gray will be allocated
    memory = in_RGBA.memory

    -- out_gray descriptors
    imgDesc = ll.ImageDescriptor.new(in_RGBA.imageDescriptor)
    imgDesc.channelCount = 1

    imgViewDesc = ll.ImageViewDescriptor.new(in_RGBA.descriptor)
    
    out_gray = memory:createImageView(imgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_gray:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_gray', out_gray)
    node:configureGridShape(ll.vec3ui.new(out_gray.width, out_gray.height, 1))
end

ll.registerNodeBuilder('RGBA2Gray', builder)
